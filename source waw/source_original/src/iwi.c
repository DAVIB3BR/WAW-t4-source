/*
 * iwi.c - leitura de cabecalho + decodificacao BC1(DXT1)/BC3(DXT5) -> RGBA8.
 *
 * Decodificadores de bloco escritos a mao a partir da especificacao S3TC:
 *   BC1: u16 c0, u16 c1, u32 indices[4] (2 bits/pixel, 4x4)
 *   BC3: u8 a0, u8 a1, u8 alpha_bits[6] (u64 de 3 bits/pixel) + sub-bloco BC1
 */
/*
 * Formatos comprovados por verificacao de contas sobre as 9.331 texturas do
 * jogo (tools/iwi_survey.py + tools/iwi_format_probe.py):
 *
 *   1  = BGRA8 nao comprimido        (4 B/pixel, sem mips)        48 arq.
 *   3  = A8L8                        (2 B/pixel: [0]=alpha,[1]=luma) 46
 *   4  = L8/A8                       (1 B/pixel)                  22
 *   11 = A8_DXT1  -> BC1             (8 B/bloco)                2.667
 *   12 = A8_DXT5  -> BC3/DXT5        (16 B/bloco)                 803
 *   13 = DXN      -> BC5 (mapa normal, 2 canais)  (16 B/bloco)  5.665
 *   2, 6, 7, 8, 9 = nao identificados ainda (80 arq., 0,86%)
 *
 * Cobre 9.251 de 9.331 texturas = 99,1%.
 */
#include "iwi.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char g_err[256];
const char *t4_iwi_error(void) { return g_err; }

static int fail(const char *m)
{
    snprintf(g_err, sizeof(g_err), "%s", m);
    return -1;
}

static uint16_t r16(const uint8_t *p) { return (uint16_t)(p[0] | (p[1] << 8)); }
static uint32_t r32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

int t4_iwi_parse(const uint8_t *data, size_t len, T4IwiInfo *out)
{
    memset(out, 0, sizeof(*out));
    if (len < T4_IWI_HEADER_SIZE) return fail("arquivo menor que o cabecalho (28 bytes)");
    if (data[0] != 'I' || data[1] != 'W' || data[2] != 'i') return fail("magic \"IWi\" ausente");

    out->version = data[3];
    out->format  = data[4];
    out->flags   = data[5];
    out->width   = r16(data + 6);
    out->height  = r16(data + 8);
    out->field_0x0A = r16(data + 10);
    out->file_size_field = r32(data + 12);

    out->payload = data + T4_IWI_HEADER_SIZE;
    out->payload_len = len - T4_IWI_HEADER_SIZE;

    if (out->width <= 0 || out->height <= 0 || out->width > 16384 || out->height > 16384)
        return fail("dimensoes invalidas");

    /* coerencia: o u32 repetido deve ser o tamanho do arquivo */
    if (out->file_size_field != (uint32_t)len) {
        snprintf(g_err, sizeof(g_err),
                 "aviso: campo de tamanho (%u) difere do tamanho real (%zu)",
                 out->file_size_field, len);
    }
    return 0;
}

/* ---- BC1/DXT1 ---------------------------------------------------------- */

static void rgb565_to_rgba(uint16_t c, uint8_t *r, uint8_t *g, uint8_t *b)
{
    int r5 = (c >> 11) & 0x1F, g6 = (c >> 5) & 0x3F, b5 = c & 0x1F;
    *r = (uint8_t)((r5 * 255 + 15) / 31);
    *g = (uint8_t)((g6 * 255 + 31) / 63);
    *b = (uint8_t)((b5 * 255 + 15) / 31);
}

/* Decodifica um bloco BC1 (8 bytes) em 4x4 pixels RGBA. */
static void decode_bc1(const uint8_t *blk, uint8_t *out, int stride)
{
    uint16_t c0 = r16(blk), c1 = r16(blk + 2);
    uint32_t idx = (uint32_t)blk[4] | ((uint32_t)blk[5] << 8) |
                   ((uint32_t)blk[6] << 16) | ((uint32_t)blk[7] << 24);
    uint8_t col[4][4];
    int i, x, y;

    rgb565_to_rgba(c0, &col[0][0], &col[0][1], &col[0][2]); col[0][3] = 255;
    rgb565_to_rgba(c1, &col[1][0], &col[1][1], &col[1][2]); col[1][3] = 255;

    if (c0 > c1) {
        for (i = 0; i < 3; i++) {
            col[2][i] = (uint8_t)((2 * col[0][i] + col[1][i]) / 3);
            col[3][i] = (uint8_t)((col[0][i] + 2 * col[1][i]) / 3);
        }
        col[2][3] = col[3][3] = 255;
    } else {
        for (i = 0; i < 3; i++) {
            col[2][i] = (uint8_t)((col[0][i] + col[1][i]) / 2);
            col[3][i] = 0;
        }
        col[2][3] = 255;
        col[3][3] = 0; /* cor transparente */
    }

    for (y = 0; y < 4; y++) {
        for (x = 0; x < 4; x++) {
            int sel = (int)((idx >> (2 * (y * 4 + x))) & 3u);
            uint8_t *d = out + (size_t)y * stride + (size_t)x * 4;
            d[0] = col[sel][0]; d[1] = col[sel][1];
            d[2] = col[sel][2]; d[3] = col[sel][3];
        }
    }
}

/* ---- BC5/DXT5/BC3 ------------------------------------------------------- */

/* BC5: dois sub-blocos de 8 bytes (v0, v1, indices de 3 bits) -> R e G. */
static void decode_bc5(const uint8_t *blk, uint8_t *chan, int base)
{
    uint8_t v0 = blk[base], v1 = blk[base + 1];
    uint64_t bits = 0;
    uint8_t t[8];
    int i;

    for (i = 0; i < 6; i++) bits |= (uint64_t)blk[base + 2 + i] << (8 * i);

    t[0] = v0;
    t[1] = v1;
    if (v0 > v1) {
        for (i = 1; i <= 6; i++) t[1 + i] = (uint8_t)(((7 - i) * v0 + i * v1) / 7);
    } else {
        for (i = 1; i <= 4; i++) t[1 + i] = (uint8_t)(((5 - i) * v0 + i * v1) / 5);
        t[6] = 0;
        t[7] = 255;
    }
    for (i = 0; i < 16; i++) chan[i] = t[(bits >> (3 * i)) & 7u];
}

static void decode_bc3(const uint8_t *blk, uint8_t *out, int stride)
{
    uint8_t a0 = blk[0], a1 = blk[1];
    uint64_t abits = 0;
    uint8_t alpha[8];
    int i, x, y;

    for (i = 0; i < 6; i++) abits |= (uint64_t)blk[2 + i] << (8 * i);

    alpha[0] = a0;
    alpha[1] = a1;
    if (a0 > a1) {
        for (i = 1; i <= 6; i++)
            alpha[1 + i] = (uint8_t)(((7 - i) * a0 + i * a1) / 7);
    } else {
        for (i = 1; i <= 4; i++)
            alpha[1 + i] = (uint8_t)(((5 - i) * a0 + i * a1) / 5);
        alpha[6] = 0;
        alpha[7] = 255;
    }

    decode_bc1(blk + 8, out, stride);

    for (y = 0; y < 4; y++) {
        for (x = 0; x < 4; x++) {
            int p = y * 4 + x;
            int sel = (int)((abits >> (3 * p)) & 7u);
            out[(size_t)y * stride + (size_t)x * 4 + 3] = alpha[sel];
        }
    }
}

int t4_iwi_decode_rgba(const uint8_t *data, size_t len, uint8_t **rgba, int *w, int *h)
{
    T4IwiInfo info;
    uint8_t *dst;
    int bw, bh, bx, by, i;
    int block_bytes = 0;

    *rgba = NULL;
    if (t4_iwi_parse(data, len, &info) < 0) return -1;

    dst = (uint8_t *)malloc((size_t)info.width * info.height * 4);
    if (!dst) return fail("sem memoria");

    switch (info.format) {
    case T4_IWI_FORMAT_BGRA8: {           /* B,G,R,A -> R,G,B,A */
        size_t n = (size_t)info.width * info.height;
        if (info.payload_len < n * 4) { free(dst); return fail("payload curto (BGRA8)"); }
        for (i = 0; i < (int)n; i++) {
            const uint8_t *s = info.payload + (size_t)i * 4;
            uint8_t *d = dst + (size_t)i * 4;
            d[0] = s[2]; d[1] = s[1]; d[2] = s[0]; d[3] = s[3];
        }
        break;
    }
    case T4_IWI_FORMAT_A8L8: {            /* [0]=alpha, [1]=luminancia */
        size_t n = (size_t)info.width * info.height;
        if (info.payload_len < n * 2) { free(dst); return fail("payload curto (A8L8)"); }
        for (i = 0; i < (int)n; i++) {
            uint8_t a = info.payload[(size_t)i * 2];
            uint8_t l = info.payload[(size_t)i * 2 + 1];
            uint8_t *d = dst + (size_t)i * 4;
            d[0] = l; d[1] = l; d[2] = l; d[3] = a;
        }
        break;
    }
    case T4_IWI_FORMAT_L8: {              /* 1 byte/pixel, alpha cheio */
        size_t n = (size_t)info.width * info.height;
        if (info.payload_len < n) { free(dst); return fail("payload curto (L8)"); }
        for (i = 0; i < (int)n; i++) {
            uint8_t v = info.payload[i];
            uint8_t *d = dst + (size_t)i * 4;
            d[0] = v; d[1] = v; d[2] = v; d[3] = 255;
        }
        break;
    }
    case T4_IWI_FORMAT_DXT1:
    case T4_IWI_FORMAT_DXT5:
    case T4_IWI_FORMAT_DXN: {
        uint8_t tmp[16 * 4];
        block_bytes = (info.format == T4_IWI_FORMAT_DXT1) ? 8 : 16;
        bw = (info.width + 3) / 4;
        bh = (info.height + 3) / 4;
        if ((size_t)bw * bh * block_bytes > info.payload_len) {
            snprintf(g_err, sizeof(g_err), "payload curto: %zu bytes para %dx%d (fmt %d)",
                     info.payload_len, info.width, info.height, info.format);
            free(dst);
            return -1;
        }
        for (by = 0; by < bh; by++) {
            for (bx = 0; bx < bw; bx++) {
                int px = bx * 4, py = by * 4;
                int x, y;
                const uint8_t *blk = info.payload + ((size_t)by * bw + bx) * block_bytes;

                if (info.format == T4_IWI_FORMAT_DXT1) {
                    decode_bc1(blk, tmp, 4 * 4);
                } else if (info.format == T4_IWI_FORMAT_DXT5) {
                    decode_bc3(blk, tmp, 4 * 4);
                } else {
                    /* DXN/BC5: dois canais de 3 bits; reconstruct em B=255 */
                    uint8_t ch_r[16], ch_g[16];
                    decode_bc5(blk, ch_r, 0);
                    decode_bc5(blk, ch_g, 8);
                    for (x = 0; x < 16; x++) {
                        tmp[x * 4 + 0] = ch_r[x];
                        tmp[x * 4 + 1] = ch_g[x];
                        tmp[x * 4 + 2] = 255;
                        tmp[x * 4 + 3] = 255;
                    }
                }

                for (y = 0; y < 4; y++) {
                    int yy = py + y;
                    if (yy >= info.height) break;
                    for (x = 0; x < 4; x++) {
                        int xx = px + x;
                        if (xx >= info.width) break;
                        memcpy(dst + ((size_t)yy * info.width + xx) * 4,
                               tmp + (y * 4 + x) * 4, 4);
                    }
                }
            }
        }
        break;
    }
    default:
        snprintf(g_err, sizeof(g_err),
                 "formato IWI %d nao identificado (suportados: 1,3,4,11,12,13 = 99,1%% das texturas)",
                 info.format);
        free(dst);
        return -1;
    }

    (void)block_bytes;
    *rgba = dst;
    *w = info.width;
    *h = info.height;
    return 0;
}
