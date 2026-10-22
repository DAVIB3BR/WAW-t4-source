/*
 * ff.c - descompressao e leitura de cabecalho dos fastfiles (IWffu100).
 */
#include "ff.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <zlib.h>

static char g_err[512];

const char *t4_ff_error(void) { return g_err; }

static int fail(const char *msg)
{
    snprintf(g_err, sizeof(g_err), "%s%s%s", msg ? msg : "",
             (msg && errno) ? " " : "", (errno) ? strerror(errno) : "");
    return -1;
}

int t4_ff_load(T4FF *ff, const char *path)
{
    FILE *fp;
    long fsize;
    uint8_t *raw = NULL, *out = NULL;
    z_stream strm;
    int rc;
    size_t out_cap;

    memset(ff, 0, sizeof(*ff));
    g_err[0] = '\0';

    fp = fopen(path, "rb");
    if (!fp) return fail("abrir fastfile");

    fseek(fp, 0, SEEK_END);
    fsize = ftell(fp);
    if (fsize < 16) { fclose(fp); return fail("fastfile curto demais"); }
    fseek(fp, 0, SEEK_SET);

    raw = (uint8_t *)malloc((size_t)fsize);
    if (!raw) { fclose(fp); return fail("sem memoria (raw)"); }
    if (fread(raw, 1, (size_t)fsize, fp) != (size_t)fsize) {
        free(raw); fclose(fp);
        return fail("leitura do fastfile");
    }
    fclose(fp);

    if (memcmp(raw, T4_FF_MAGIC, 8) != 0) {
        free(raw);
        snprintf(g_err, sizeof(g_err), "magic invalido (esperado \"%s\")", T4_FF_MAGIC);
        return -1;
    }

    ff->hdr0 = (uint32_t)raw[8] | ((uint32_t)raw[9] << 8) |
               ((uint32_t)raw[10] << 16) | ((uint32_t)raw[11] << 24);

    /* capacidade provisoria: fastfiles tem razao ~1.5x (43.8MB de 29MB).
     * o inflate realloca por blocos se precisar. */
    out_cap = (size_t)fsize * 2;
    out = (uint8_t *)malloc(out_cap);
    if (!out) { free(raw); return fail("sem memoria (out)"); }

    memset(&strm, 0, sizeof(strm));
    strm.next_in = raw + 12;
    strm.avail_in = (uInt)(fsize - 12);
    strm.next_out = out;
    strm.avail_out = (uInt)out_cap;

    if (inflateInit(&strm) != Z_OK) {
        free(raw); free(out);
        return fail("inflateInit");
    }

    for (;;) {
        rc = inflate(&strm, Z_NO_FLUSH);
        if (rc == Z_STREAM_END) break;
        if (rc == Z_OK || rc == Z_BUF_ERROR) {
            if (strm.avail_out == 0) {
                size_t have = strm.total_out;
                uint8_t *bigger = (uint8_t *)realloc(out, out_cap * 2);
                if (!bigger) { inflateEnd(&strm); free(raw); free(out); return fail("sem memoria (grow)"); }
                out = bigger;
                out_cap *= 2;
                strm.next_out = out + have;
                strm.avail_out = (uInt)(out_cap - have);
                continue;
            }
            if (rc == Z_BUF_ERROR && strm.avail_in == 0) {
                inflateEnd(&strm); free(raw); free(out);
                return fail("zlib truncado antes do fim");
            }
            continue;
        }
        inflateEnd(&strm);
        free(raw); free(out);
        snprintf(g_err, sizeof(g_err), "inflate falhou (rc=%d)", rc);
        return -1;
    }

    ff->len = strm.total_out;
    inflateEnd(&strm);
    free(raw);

    ff->data = out;
    if (ff->len >= 12) {
        ff->body0 = (uint32_t)out[0] | ((uint32_t)out[1] << 8) | ((uint32_t)out[2] << 16) | ((uint32_t)out[3] << 24);
        ff->body1 = (uint32_t)out[4] | ((uint32_t)out[5] << 8) | ((uint32_t)out[6] << 16) | ((uint32_t)out[7] << 24);
        ff->body2 = (uint32_t)out[8] | ((uint32_t)out[9] << 8) | ((uint32_t)out[10] << 16) | ((uint32_t)out[11] << 24);
    }
    return 0;
}

void t4_ff_free(T4FF *ff)
{
    if (!ff) return;
    free(ff->data);
    memset(ff, 0, sizeof(*ff));
}

/* ==========================================================================
 * Parser do XAssetList + caminhada exata dos assets
 *
 * Espelha re/fftool.py (validado em 129/129 .ff do WaW, ver re/FF_FORMAT.md).
 * Layouts exatos: localize (23) e rawfile (32). Demais tipos sao pulados por
 * ancoragem no proximo run parseavel (mesma heurica do parser Python).
 * ========================================================================== */

static int fail_msg(const char *msg)
{
    snprintf(g_err, sizeof(g_err), "%s", msg ? msg : "erro");
    return -1;
}

static uint32_t rd32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

/* Mesmo conjunto de imprimiveis do _PRINTABLE do Python (latin-1). */
static int is_printable(unsigned c)
{
    return (c >= 32 && c <= 126) || c == 9 || c == 10 || c == 13 ||
           (c >= 160 && c <= 255);
}

/* C-string dentro do stream; devolve ponteiro para dentro de r (NUL no buffer)
 * e a posicao seguinte ao terminador. */
static int cstr_at(const uint8_t *r, size_t n, size_t pos,
                   const char **out, size_t *end)
{
    size_t i, k;
    if (pos >= n) return 0;
    for (i = pos; i < n; i++)
        if (r[i] == 0) break;
    if (i >= n) return 0; /* sem terminador */
    for (k = pos; k < i; k++)
        if (!is_printable(r[k])) return 0;
    *out = (const char *)(r + pos);
    *end = i + 1;
    return 1;
}

/* Filtro de plausibilidade do _valid_name (evita falsos positivos). */
static int valid_name(const char *s, size_t len)
{
    size_t i;
    int alnum = 0;
    if (!s || len < 2 || len > 255) return 0;
    for (i = 0; i < len; i++) {
        unsigned c = (unsigned char)s[i];
        if (c == 0xFF || !is_printable(c)) return 0;
        if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z'))
            alnum = 1;
    }
    return alnum;
}

static int parseable_type(uint32_t t)
{
    return t == T4_ASSET_LOCALIZE_ENTRY || t == T4_ASSET_RAWFILE;
}

/* LocalizeEntry {value*, name*}: name FOLLOWING + strings inline.
 * `value` e deduplicado (pode morar fora do stream). */
static int parse_localize_at(const uint8_t *r, size_t n, size_t pos,
                             size_t *fim, const char **value, const char **name)
{
    uint32_t pv, pn;
    size_t p;
    const char *v = NULL, *nm;
    if (pos + 8 > n) return 0;
    pv = rd32(r + pos);
    pn = rd32(r + pos + 4);
    if (pn != T4_FF_PTR_FOLLOWING) return 0;
    p = pos + 8;
    if (pv == T4_FF_PTR_FOLLOWING) {
        if (!cstr_at(r, n, p, &v, &p)) return 0;
    }
    if (!cstr_at(r, n, p, &nm, &p)) return 0;
    if (!valid_name(nm, strlen(nm))) return 0;
    *fim = p;
    *value = v;
    *name = nm;
    return 1;
}

/* RawFile {name*, len, buffer*}: nome e buffer FOLLOWING.
 * Variante rara "vazia" (len==0, buffer*==0) tem UM byte residual (0x2c)
 * entre o struct e o nome; o terminador do buffer nao conta em `len`. */
static int parse_rawfile_at(const uint8_t *r, size_t n, size_t pos,
                            size_t *fim, const char **name,
                            const uint8_t **buf, size_t *blen)
{
    uint32_t pn, length, pb;
    int inline_buf;
    size_t p, end;
    const char *nm;
    if (pos + 12 > n) return 0;
    pn = rd32(r + pos);
    length = rd32(r + pos + 4);
    pb = rd32(r + pos + 8);
    if (pn != T4_FF_PTR_FOLLOWING) return 0;
    if ((size_t)length > n) return 0;
    inline_buf = (pb == T4_FF_PTR_FOLLOWING);
    p = pos + 12;
    if (!inline_buf) {
        if (length != 0 || pb != 0) return 0;
        if (p < n) p += 1; /* byte residual (0x2c) */
    }
    if (!cstr_at(r, n, p, &nm, &p)) return 0;
    if (!valid_name(nm, strlen(nm))) return 0;
    if (!inline_buf) {
        *fim = p;
        *name = nm;
        *buf = NULL;
        *blen = 0;
        return 1;
    }
    if (p + (size_t)length > n) return 0;
    end = p + (size_t)length;
    if (end < n && r[end] == 0) end += 1;
    *fim = end;
    *name = nm;
    *buf = r + p;
    *blen = (size_t)length;
    return 1;
}

/* Dispatch por tipo (apenas layouts exatos). */
static int try_parse_asset(const uint8_t *r, size_t n, uint32_t t, size_t pos,
                           size_t *fim, const char **name,
                           const uint8_t **buf, size_t *blen)
{
    if (t == T4_ASSET_LOCALIZE_ENTRY) {
        const char *value;
        if (!parse_localize_at(r, n, pos, fim, &value, name)) return 0;
        *buf = (const uint8_t *)value; /* valor (pode ser NULL) */
        *blen = 0;
        return 1;
    }
    if (t == T4_ASSET_RAWFILE)
        return parse_rawfile_at(r, n, pos, fim, name, buf, blen);
    return 0;
}

/* Procura pat a partir de `from` (equivalente a bytes.find do Python). */
static const uint8_t *find_bytes(const uint8_t *r, size_t n, size_t from,
                                 const uint8_t *pat, size_t plen)
{
    size_t i;
    if (from > n || plen > n) return NULL;
    for (i = from; i + plen <= n; i++)
        if (memcmp(r + i, pat, plen) == 0) return r + i;
    return NULL;
}

int t4_ff_parse(T4FF *ff)
{
    const uint8_t *r;
    size_t n, base, pos, need;
    uint32_t scount, sptr, acount, aptr, i;

    if (!ff || !ff->data) return fail_msg("t4_ff_parse sem dados");
    g_err[0] = '\0';
    r = ff->data;
    n = ff->len;
    if (n < 52) return fail_msg("stream curto (XAssetList)");

    ff->size = rd32(r);
    ff->external_size = rd32(r + 4);
    for (i = 0; i < 7; i++) ff->blocks[i] = rd32(r + 8 + 4 * i);

    scount = rd32(r + 36);
    sptr = rd32(r + 40);
    acount = rd32(r + 44);
    aptr = rd32(r + 48);
    (void)sptr;
    if (aptr != T4_FF_PTR_FOLLOWING) {
        snprintf(g_err, sizeof(g_err), "assets* nao-FOLLOWING: 0x%08x", aptr);
        return -1;
    }

    base = 52;
    if ((size_t)scount > (n - base) / 4 || (size_t)acount > (n - base) / 8)
        return fail_msg("contagens de string/asset fora dos limites");

    /* Script strings: slots FOLLOWING seguidos das strings inline. */
    pos = base + 4 * (size_t)scount;
    for (i = 0; i < scount; i++) {
        uint32_t v = rd32(r + base + 4 * i);
        if (v == T4_FF_PTR_FOLLOWING) {
            const char *s;
            if (!cstr_at(r, n, pos, &s, &pos))
                return fail_msg("script string invalida");
        } else if (v != 0) {
            const char *s;
            if (v - 1 >= n || !cstr_at(r, n, base + (v - 1), &s, &need))
                return fail_msg("script string (offset) invalida");
        }
    }
    ff->string_count = scount;

    /* Array de XAsset {type, header*} em pares de 8 bytes. */
    need = 8 * (size_t)acount;
    if (pos > n || need > n - pos) return fail_msg("array de assets fora dos limites");
    ff->asset_types = (uint32_t *)(void *)(r + pos);
    ff->asset_count = acount;
    ff->data_start = (uint32_t)(pos + need);
    return 0;
}

int t4_ff_walk(const T4FF *ff, int collect_rawfiles,
               T4FFAsset **out, size_t *count,
               size_t *n_exact, size_t *n_skipped, int *anchor_failed)
{
    const uint8_t *r, *types;
    size_t n, pos, i = 0, produced = 0, pulados = 0, exatos = 0;
    uint32_t nn;
    T4FFAsset *arr;
    static const uint8_t pat_ff[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
    int af = 0;

    if (out) *out = NULL;
    if (count) *count = 0;
    if (n_exact) *n_exact = 0;
    if (n_skipped) *n_skipped = 0;
    if (anchor_failed) *anchor_failed = 0;
    if (!ff || !ff->data || !ff->asset_types)
        return fail_msg("t4_ff_walk sem t4_ff_parse");

    r = ff->data;
    n = ff->len;
    nn = ff->asset_count;
    types = (const uint8_t *)ff->asset_types;
    pos = ff->data_start;

    arr = (T4FFAsset *)calloc(nn ? (size_t)nn : 1, sizeof(T4FFAsset));
    if (!arr) return fail_msg("sem memoria (walk)");

    while (i < nn) {
        uint32_t t = rd32(types + 8 * i);
        if (parseable_type(t)) {
            size_t fim;
            const char *nm = NULL;
            const uint8_t *buf = NULL;
            size_t blen = 0;
            if (try_parse_asset(r, n, t, pos, &fim, &nm, &buf, &blen)) {
                arr[produced].index = (uint32_t)i;
                arr[produced].type = t;
                arr[produced].name = nm;
                arr[produced].name_len = nm ? strlen(nm) : 0;
                if (t == T4_ASSET_RAWFILE && collect_rawfiles) {
                    arr[produced].buf = buf;
                    arr[produced].buf_len = blen;
                }
                produced++;
                exatos++;
                pos = fim;
                i++;
                continue;
            }
            /* Layout nao casou: marca como pulado e segue (mesmo codigo). */
            pulados++;
            arr[produced].index = (uint32_t)i;
            arr[produced].type = t;
            arr[produced].name = NULL;
            arr[produced].buf = NULL;
            produced++;
            i++;
            continue;
        }

        /* Run de tipos nao-parseaveis: ancora no proximo run parseavel. */
        {
            size_t j = i, k2;
            uint32_t anchor_type;
            size_t runlen = 0, probe, found = (size_t)-1;
            while (j < nn && !parseable_type(rd32(types + 8 * j))) j++;
            if (j >= nn) break; /* sem ancora; fim de dados esperado */
            anchor_type = rd32(types + 8 * j);
            while (j + runlen < nn && runlen < 8 &&
                   parseable_type(rd32(types + 8 * (j + runlen))))
                runlen++;

            probe = pos;
            for (;;) {
                const uint8_t *f = find_bytes(r, n, probe, pat_ff, 4);
                size_t k, p2;
                int ok = 0;
                size_t jj = j;
                if (!f) break;
                k = (size_t)(f - r);
                if (k >= n) break;
                {
                    size_t fim;
                    const char *nm;
                    const uint8_t *b;
                    size_t bl;
                    if (try_parse_asset(r, n, anchor_type, k, &fim, &nm, &b, &bl)) {
                        p2 = k;
                        while (jj < nn && ok < (int)runlen &&
                               parseable_type(rd32(types + 8 * jj))) {
                            size_t fim2;
                            const char *nm2;
                            const uint8_t *b2;
                            size_t bl2;
                            if (!try_parse_asset(r, n, rd32(types + 8 * jj), p2,
                                                 &fim2, &nm2, &b2, &bl2))
                                break;
                            p2 = fim2;
                            ok++;
                            jj++;
                        }
                        if (ok == (int)runlen) { found = k; break; }
                    }
                }
                probe = k + 1;
            }

            if (found == (size_t)-1) {
                /* Resto nao parseado; devolve o lido ate aqui (resiliente). */
                pulados += nn - i;
                for (k2 = i; k2 < nn; k2++) {
                    arr[produced].index = (uint32_t)k2;
                    arr[produced].type = rd32(types + 8 * k2);
                    arr[produced].name = NULL;
                    arr[produced].buf = NULL;
                    produced++;
                }
                af = 1;
                break;
            }
            pulados += j - i;
            for (k2 = i; k2 < j; k2++) {
                arr[produced].index = (uint32_t)k2;
                arr[produced].type = rd32(types + 8 * k2);
                arr[produced].name = NULL;
                arr[produced].buf = NULL;
                produced++;
            }
            pos = found;
            i = j;
        }
    }

    if (out) *out = arr; else free(arr);
    if (count) *count = produced;
    if (n_exact) *n_exact = exatos;
    if (n_skipped) *n_skipped = pulados;
    if (anchor_failed) *anchor_failed = af;
    return 0;
}
