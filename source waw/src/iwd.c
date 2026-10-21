/*
 * iwd.c - parser de ZIP para os .iwd do CoD: World at War.
 *
 * Estrutura lida:
 *   [file entries ...] [central directory] [ EOCD (0x06054b50) ]
 * O EOCD fica no fim do arquivo (com comentario opcional de ate 65535 bytes),
 * de la tiramos o offset e o tamanho do central directory, e de cada entrada
 * (0x02014b50) tiramos o offset do local header (0x04034b50) para a leitura.
 */
#include "iwd.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <zlib.h>

static char g_err[512];

const char *t4_iwd_error(void) { return g_err; }

static int fail(const char *fmt, ...)
{
    /* mantido simples: so mensagens literais + errno */
    if (fmt) {
        size_t n = strlen(fmt);
        if (n >= sizeof(g_err)) n = sizeof(g_err) - 1;
        memcpy(g_err, fmt, n);
        g_err[n] = '\0';
        if (errno) {
            size_t l = strlen(g_err);
            snprintf(g_err + l, sizeof(g_err) - l, " (%s)", strerror(errno));
        }
    }
    return -1;
}

static int rd(FILE *fp, void *buf, size_t n, long off)
{
    if (fseek(fp, off, SEEK_SET) != 0) return -1;
    if (fread(buf, 1, n, fp) != n) return -1;
    return 0;
}

static uint16_t r16(const uint8_t *p) { return (uint16_t)(p[0] | (p[1] << 8)); }
static uint32_t r32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

/* Normaliza '\' -> '/' e baixa para minusculas nao aplicavel: nomes do COD sao
 * consistentes em caixa, apenas o separador precisa padronizar. */
static void norm_name(char *s)
{
    for (; *s; ++s)
        if (*s == '\\') *s = '/';
}

int t4_iwd_open(T4Iwd *z, const char *path)
{
    uint8_t *tail = NULL;
    long fsize;
    size_t i, found = 0;
    long eocd = -1;
    uint32_t cd_off, cd_size;
    uint16_t n_entries;
    uint8_t *cd = NULL;

    memset(z, 0, sizeof(*z));
    g_err[0] = '\0';

    z->fp = fopen(path, "rb");
    if (!z->fp) return fail("nao foi possivel abrir");

    fseek(z->fp, 0, SEEK_END);
    fsize = ftell(z->fp);
    if (fsize < 22) { fclose(z->fp); z->fp = NULL; return fail("arquivo curto demais"); }

    snprintf(z->path, sizeof(z->path), "%s", path);

    /* procura o EOCD varrendo os ultimos 64KB + 22 bytes */
    {
        size_t tail_len = (size_t)(fsize < (long)(65535 + 22) ? fsize : (65535 + 22));
        long tail_off = fsize - (long)tail_len;
        tail = (uint8_t *)malloc(tail_len);
        if (!tail || rd(z->fp, tail, tail_len, tail_off) != 0) {
            free(tail);
            fclose(z->fp); z->fp = NULL;
            return fail("falha lendo o fim do arquivo");
        }
        for (i = tail_len - 22; i < tail_len; i--) {
            if (r32(tail + i) == 0x06054b50u) { eocd = tail_off + (long)i; break; }
            if (i == 0) break;
        }
        if (eocd < 0) {
            /* algunos .iwd de servidor sao zip puro so com EOCD "limpo"; tenta os 22 bytes finais */
            if (tail_len >= 22 && r32(tail + tail_len - 22) == 0x06054b50u)
                eocd = fsize - 22;
        }
        free(tail);
    }
    if (eocd < 0) { fclose(z->fp); z->fp = NULL; return fail("EOCD (zip) nao encontrado"); }

    {
        uint8_t h[22];
        if (rd(z->fp, h, 22, eocd) != 0) { fclose(z->fp); z->fp = NULL; return fail("EOCD ilegivel"); }
        n_entries = r16(h + 10);
        cd_size   = r32(h + 12);
        cd_off    = r32(h + 16);
    }

    cd = (uint8_t *)malloc(cd_size ? cd_size : 1);
    if (!cd || rd(z->fp, cd, cd_size, (long)cd_off) != 0) {
        free(cd);
        fclose(z->fp); z->fp = NULL;
        return fail("central directory ilegivel");
    }

    z->entries = (T4ZipEntry *)calloc(n_entries ? n_entries : 1, sizeof(T4ZipEntry));
    if (!z->entries) { free(cd); fclose(z->fp); z->fp = NULL; return fail("sem memoria"); }

    /* itera as entradas do central directory */
    {
        size_t off = 0;
        while (off + 46 <= cd_size && found < n_entries) {
            uint8_t *e = cd + off;
            uint16_t nlen, elen, clen;
            if (r32(e) != 0x02014b50u) {
                g_err[0] = '\0';
                snprintf(g_err, sizeof(g_err), "assinatura de entrada invalida em %zu", off);
                break;
            }
            nlen = r16(e + 28);
            elen = r16(e + 30);
            clen = r16(e + 32);
            if (off + 46 + nlen > cd_size) break;

            T4ZipEntry *ent = &z->entries[found];
            size_t namelen = nlen < (T4_IWD_MAX_NAME - 1) ? nlen : (T4_IWD_MAX_NAME - 1);
            memcpy(ent->name, e + 46, namelen);
            ent->name[namelen] = '\0';
            norm_name(ent->name);
            ent->method       = r16(e + 10);
            ent->crc32        = r32(e + 16);
            ent->comp_size    = r32(e + 20);
            ent->uncomp_size  = r32(e + 24);
            ent->local_offset = r32(e + 42);

            if (ent->name[0] && ent->name[strlen(ent->name) - 1] != '/')
                z->total_uncomp += ent->uncomp_size;

            found++;
            off += 46 + nlen + elen + clen;
        }
    }

    free(cd);
    z->count = found;
    if (g_err[0] == '\0') snprintf(g_err, sizeof(g_err), "ok");
    return 0;
}

void t4_iwd_close(T4Iwd *z)
{
    if (!z) return;
    if (z->fp) fclose(z->fp);
    free(z->entries);
    memset(z, 0, sizeof(*z));
}

size_t t4_iwd_count(const T4Iwd *z) { return z->count; }

const T4ZipEntry *t4_iwd_at(const T4Iwd *z, size_t index)
{
    if (index >= z->count) return NULL;
    return &z->entries[index];
}

const T4ZipEntry *t4_iwd_find(const T4Iwd *z, const char *name)
{
    size_t i;
    char norm[T4_IWD_MAX_NAME];
    size_t n = strlen(name);
    if (n >= sizeof(norm)) return NULL;
    memcpy(norm, name, n + 1);
    norm_name(norm);
    for (i = 0; i < z->count; i++)
        if (strcmp(z->entries[i].name, norm) == 0) return &z->entries[i];
    return NULL;
}

int t4_iwd_read(const T4Iwd *z, const T4ZipEntry *e, uint8_t **out, size_t *out_len)
{
    uint8_t lh[30];
    uint16_t nlen, elen;
    long data_off;
    uint8_t *comp = NULL, *dst = NULL;

    *out = NULL;
    *out_len = 0;
    if (!z->fp || !e) return fail("entrada invalida");

    if (rd(z->fp, lh, 30, (long)e->local_offset) != 0) return fail("local header ilegivel");
    if (r32(lh) != 0x04034b50u) return fail("assinatura local invalida");
    nlen = r16(lh + 26);
    elen = r16(lh + 28);
    data_off = (long)e->local_offset + 30 + nlen + elen;

    comp = (uint8_t *)malloc(e->comp_size ? e->comp_size : 1);
    if (!comp) return fail("sem memoria");
    if (rd(z->fp, comp, e->comp_size, data_off) != 0) { free(comp); return fail("falha lendo dados"); }

    dst = (uint8_t *)malloc(e->uncomp_size ? e->uncomp_size : 1);
    if (!dst) { free(comp); return fail("sem memoria"); }

    if (e->method == 0) {
        memcpy(dst, comp, e->uncomp_size);
    } else if (e->method == 8) {
        z_stream strm;
        int rc;
        memset(&strm, 0, sizeof(strm));
        strm.next_in = comp;
        strm.avail_in = e->comp_size;
        strm.next_out = dst;
        strm.avail_out = e->uncomp_size;
        if (inflateInit2(&strm, -MAX_WBITS) != Z_OK) { free(comp); free(dst); return fail("inflateInit"); }
        rc = inflate(&strm, Z_FINISH);
        inflateEnd(&strm);
        free(comp);
        if (rc != Z_STREAM_END && !(rc == Z_OK && strm.avail_out == 0)) {
            free(dst);
            return fail("inflate truncado");
        }
        *out_len = e->uncomp_size - strm.avail_out;
        *out = dst;
        return 0;
    } else {
        free(comp);
        free(dst);
        return fail("metodo de compressao nao suportado");
    }

    free(comp);
    *out = dst;
    *out_len = e->uncomp_size;
    return 0;
}
