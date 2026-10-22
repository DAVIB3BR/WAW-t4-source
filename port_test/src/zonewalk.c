/*
 * zonewalk.c - walker estruturado de fastfile T4 em C (porte fiel de
 * re/fftool.py; ver zonewalk.h para o contrato e re/FF_FORMAT.md para a
 * semantica de stream).
 *
 * O walker consome o payload decompresso sequencialmente a partir de
 * data_start (XAssetList + string pool ja consumidos pelo t4_ff_parse),
 * asset a asset, exatamente como o walker Python validado em 129/129.
 */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "zonewalk.h"

#include "zonetables.inc"

/* ------------------------------------------------------------ tabelas */

static int cmp_struct(const void *a, const void *b)
{
    return strcmp(((const T4ZStruct *)a)->name, ((const T4ZStruct *)b)->name);
}

const T4ZStruct *t4z_struct(const char *name)
{
    T4ZStruct key;
    if (!name) return NULL;
    memset(&key, 0, sizeof(key));
    key.name = name;
    return (const T4ZStruct *)bsearch(&key, T4Z_STRUCTS, T4Z_STRUCT_N,
                                      sizeof(T4ZStruct), cmp_struct);
}

int t4z_field_off(const char *sname, const char *fname)
{
    const T4ZStruct *st = t4z_struct(sname);
    int i;
    if (!st) return -1;
    for (i = 0; i < st->nfields; i++)
        if (!strcmp(st->fields[i].name, fname))
            return st->fields[i].off;
    return -1;
}

int t4z_field_size(const char *sname, const char *fname)
{
    const T4ZStruct *st = t4z_struct(sname);
    int i;
    if (!st) return -1;
    for (i = 0; i < st->nfields; i++)
        if (!strcmp(st->fields[i].name, fname))
            return st->fields[i].size;
    return -1;
}

const char *t4z_typename(int asset_type)
{
    char key[16];
    size_t i;
    snprintf(key, sizeof(key), "%d", asset_type);
    for (i = 0; i < T4Z_TYPESTRUCT_N; i++)
        if (!strcmp(T4Z_TYPESTRUCT[i].a, key))
            return T4Z_TYPESTRUCT[i].b;
    return NULL;
}

static int cmp_ann(const void *a, const void *b)
{
    const T4ZAnnEnt *x = (const T4ZAnnEnt *)a;
    const T4ZAnnEnt *y = (const T4ZAnnEnt *)b;
    int c = strcmp(x->sname, y->sname);
    return c ? c : strcmp(x->fname, y->fname);
}

static const T4ZSpec *ann_lookup(const char *sname, const char *fname)
{
    T4ZAnnEnt key;
    const T4ZAnnEnt *e;
    key.sname = sname;
    key.fname = fname;
    key.spec = NULL;
    e = (const T4ZAnnEnt *)bsearch(&key, T4Z_ANN, T4Z_ANN_N,
                                   sizeof(T4ZAnnEnt), cmp_ann);
    return e ? e->spec : NULL;
}

static int cmp_pair(const void *a, const void *b)
{
    const T4ZPairEnt *x = (const T4ZPairEnt *)a;
    const T4ZPairEnt *y = (const T4ZPairEnt *)b;
    int c = strcmp(x->a, y->a);
    return c ? c : strcmp(x->b, y->b);
}

static int is_runtime(const char *sname, const char *fname)
{
    T4ZPairEnt key;
    key.a = sname;
    key.b = fname;
    return bsearch(&key, T4Z_RUNTIME, T4Z_RUNTIME_N,
                   sizeof(T4ZPairEnt), cmp_pair) != NULL;
}

static int cmp_order(const void *a, const void *b)
{
    return strcmp(((const T4ZOrderEnt *)a)->sname,
                  ((const T4ZOrderEnt *)b)->sname);
}

static const T4ZOrderEnt *reord_lookup(const char *sname)
{
    T4ZOrderEnt key;
    key.sname = sname;
    key.anchor = 0;
    key.n = 0;
    key.names = NULL;
    return (const T4ZOrderEnt *)bsearch(&key, T4Z_REORD, T4Z_REORD_N,
                                        sizeof(T4ZOrderEnt), cmp_order);
}

static const T4ZOrderEnt *order_lookup(const char *sname)
{
    T4ZOrderEnt key;
    key.sname = sname;
    key.anchor = 0;
    key.n = 0;
    key.names = NULL;
    return (const T4ZOrderEnt *)bsearch(&key, T4Z_ORDER, T4Z_ORDER_N,
                                        sizeof(T4ZOrderEnt), cmp_order);
}

static int is_union_type(const char *ty)
{
    char buf[128];
    if (!ty) return 0;
    snprintf(buf, sizeof(buf), "union:%s", ty);
    return t4z_struct(buf) != NULL;
}

static const T4ZStruct *union_struct(const char *ty)
{
    char buf[128];
    if (!ty) return NULL;
    snprintf(buf, sizeof(buf), "union:%s", ty);
    return t4z_struct(buf);
}

static int has_pointers(const char *ty, int depth)
{
    const T4ZStruct *st;
    int i;
    if (!ty || depth > 4) return 0;
    st = t4z_struct(ty);
    if (!st) {
        char buf[128];
        snprintf(buf, sizeof(buf), "union:%s", ty);
        st = t4z_struct(buf);
    }
    if (!st) return 0;
    for (i = 0; i < st->nfields; i++) {
        if (st->fields[i].ptr) return 1;
        if (has_pointers(st->fields[i].type, depth + 1)) return 1;
    }
    return 0;
}

/* ------------------------------------------------------- erros e leitura */

static int failf(T4ZW *w, const char *fmt, ...)
{
    va_list ap;
    if (w->err[0]) return -1;
    va_start(ap, fmt);
    vsnprintf(w->err, sizeof(w->err), fmt, ap);
    va_end(ap);
    return -1;
}

const char *t4z_error(const T4ZW *w)
{
    return w->err[0] ? w->err : NULL;
}

static uint32_t rd32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static uint32_t rd16(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8);
}

static int is_following(uint32_t v)
{
    return v == 0xFFFFFFFFu || v == 0xFFFFFFFEu;
}

/* --------------------------------------------------------- sequencia */

struct SeqEnt { short embed; int f; int pf; };

static struct { const T4ZStruct *st; struct SeqEnt *seq; int n; }
    g_seqcache[768];
static int g_nseq;

static int fld_idx(const T4ZStruct *st, const char *name)
{
    int i;
    for (i = 0; i < st->nfields; i++)
        if (!strcmp(st->fields[i].name, name))
            return i;
    return -1;
}

static int in_names(const T4ZOrderEnt *o, const char *name)
{
    int i;
    for (i = 0; i < o->n; i++)
        if (!strcmp(o->names[i], name))
            return 1;
    return 0;
}

static int seq_build(T4ZW *w, const T4ZStruct *st, struct SeqEnt **out, int *outn)
{
    const T4ZOrderEnt *reord = reord_lookup(st->name);
    const T4ZOrderEnt *order = order_lookup(st->name);
    struct SeqEnt *seq = NULL;
    int n = 0, i, j, seen;
    (void)w;
    seq = (struct SeqEnt *)calloc((size_t)st->nfields + 8, sizeof(*seq));
    if (!seq) return failf(w, "out of memory (seq)");
    if (reord && reord->anchor && reord->n > 0) {
        const char *anchor = reord->names[0];
        for (i = 0; i < st->nfields; i++) {
            const char *nm = st->fields[i].name;
            if (in_names(reord, nm) && strcmp(nm, anchor)) continue;
            seq[n].embed = 0;
            seq[n].f = i;
            n++;
            if (!strcmp(nm, anchor)) break;
        }
        for (j = 1; j < reord->n; j++) {
            int fi = fld_idx(st, reord->names[j]);
            if (fi < 0) { free(seq); return failf(w, "reorder: missing field %s.%s", st->name, reord->names[j]); }
            seq[n].embed = 0;
            seq[n].f = fi;
            n++;
        }
        seen = 0;
        for (i = 0; i < st->nfields; i++) {
            const char *nm = st->fields[i].name;
            if (!strcmp(nm, anchor)) { seen = 1; continue; }
            if (!seen || in_names(reord, nm)) continue;
            seq[n].embed = 0;
            seq[n].f = i;
            n++;
        }
    } else if (reord) {
        for (j = 0; j < reord->n; j++) {
            int fi = fld_idx(st, reord->names[j]);
            if (fi < 0) { free(seq); return failf(w, "reorder: missing field %s.%s", st->name, reord->names[j]); }
            seq[n].embed = 0;
            seq[n].f = fi;
            n++;
        }
        for (i = 0; i < st->nfields; i++)
            if (!in_names(reord, st->fields[i].name)) {
                seq[n].embed = 0;
                seq[n].f = i;
                n++;
            }
    } else if (order) {
        for (j = 0; j < order->n; j++) {
            const char *nm = order->names[j];
            int fi = fld_idx(st, nm);
            if (fi >= 0) {
                seq[n].embed = 0;
                seq[n].f = fi;
                n++;
            } else {
                const char *sep = strstr(nm, "::");
                char parent[96];
                size_t pl;
                int pi;
                if (!sep) continue;
                pl = (size_t)(sep - nm);
                if (pl >= sizeof(parent)) continue;
                memcpy(parent, nm, pl);
                parent[pl] = 0;
                pi = fld_idx(st, parent);
                if (pi < 0) continue;
                seq[n].embed = 1;
                seq[n].f = -1;
                seq[n].pf = pi;
                n++;
            }
        }
    } else {
        for (i = 0; i < st->nfields; i++) {
            seq[n].embed = 0;
            seq[n].f = i;
            n++;
        }
    }
    if (g_nseq >= (int)(sizeof(g_seqcache) / sizeof(g_seqcache[0]))) {
        free(seq);
        return failf(w, "cache de seq cheio");
    }
    g_seqcache[g_nseq].st = st;
    g_seqcache[g_nseq].seq = seq;
    g_seqcache[g_nseq].n = n;
    g_nseq++;
    *out = seq;
    *outn = n;
    return 0;
}

static const struct SeqEnt *seq_get(T4ZW *w, const T4ZStruct *st, int *outn)
{
    int i;
    struct SeqEnt *seq = NULL;
    for (i = 0; i < g_nseq; i++)
        if (g_seqcache[i].st == st) {
            *outn = g_seqcache[i].n;
            return g_seqcache[i].seq;
        }
    if (seq_build(w, st, &seq, outn) != 0)
        return NULL;
    return seq;
}

/* ------------------------------------------------------------- valores */

static const T4ZField *fld_find(const T4ZStruct *st, const char *name)
{
    int i;
    for (i = 0; i < st->nfields; i++)
        if (!strcmp(st->fields[i].name, name))
            return &st->fields[i];
    return NULL;
}

/* valor escalar (espelha Walker._vals / _subfield); found=1 se o campo
 * existe, *ok=1 se produz numerico utilizavel */
static int val_scalar(const T4ZStruct *st, const uint8_t *base,
                      const char *name, long *out)
{
    const T4ZField *f = fld_find(st, name);
    const uint8_t *o;
    if (!f) return 0;
    o = base + f->off;
    if (f->ptr && (f->countval == -1 || f->countval == 1)) {
        *out = (long)rd32(o);
        return 1;
    }
    if (f->ptr) return 0;
    if (f->countval > 1 && f->size % f->countval == 0) {
        int es = f->size / f->countval;
        if ((es == 1 || es == 2 || es == 4) && (!f->type || strcmp(f->type, "float"))) {
            *out = (es == 1) ? (long)o[0] : (es == 2 ? (long)rd16(o) : (long)rd32(o));
            return 1;
        }
        return 0;
    }
    if ((f->size == 1 || f->size == 2 || f->size == 4) &&
        (!f->type || strcmp(f->type, "float"))) {
        *out = (f->size == 1) ? (long)o[0] : (f->size == 2 ? (long)rd16(o) : (long)rd32(o));
        return 1;
    }
    return 0;
}

static int val_arr(const T4ZStruct *st, const uint8_t *base, const char *name,
                   const uint8_t **pp, int *n, int *esz)
{
    const T4ZField *f = fld_find(st, name);
    if (!f) return 0;
    if (f->ptr && f->countval > 1) {
        *pp = base + f->off;
        *n = f->countval;
        *esz = 4;
        return 1;
    }
    if (!f->ptr && f->countval > 1 && f->size % f->countval == 0) {
        int es = f->size / f->countval;
        if ((es == 1 || es == 2 || es == 4) &&
            (!f->type || strcmp(f->type, "float"))) {
            *pp = base + f->off;
            *n = f->countval;
            *esz = es;
            return 1;
        }
    }
    return 0;
}

/* ------------------------------------------------------------- ecount */

typedef struct { const char *s; int err; } EV;

/* identificador simples -> valor (espelha _mvals + _subfield: vence o
 * frame interno; fallback le o blob de sub-structs embutidas) */
static int ident_lookup(T4ZW *w, const char *name, int hasidx, int idx,
                        int64_t *out)
{
    int i;
    for (i = w->nvstack - 1; i >= 0; i--) {
        const T4ZStruct *st = w->vstack[i].st;
        const uint8_t *base = w->vstack[i].base;
        const T4ZField *f = fld_find(st, name);
        const uint8_t *pp;
        int n, es;
        long sv;
        if (!f) continue;
        if (hasidx) {
            if (val_arr(st, base, name, &pp, &n, &es)) {
                if (idx < 0 || idx >= n) return 0;
                *out = (es == 1) ? (int64_t)pp[idx]
                      : (es == 2) ? (int64_t)rd16(pp + 2 * idx)
                                  : (int64_t)(int32_t)rd32(pp + 4 * idx);
                return 1;
            }
            return 0;
        }
        if (val_scalar(st, base, name, &sv)) {
            *out = sv;
            return 1;
        }
    }
    return 0;
}


static int64_t ev_add(T4ZW *w, EV *e);

static int64_t ev_primary(T4ZW *w, EV *e)
{
    const char *s = e->s;
    while (*s == ' ') s++;
    if (*s == '(') {
        int64_t v;
        s++;
        e->s = s;
        v = ev_add(w, e);
        s = e->s;
        while (*s == ' ') s++;
        if (*s == ')') s++;
        e->s = s;
        return v;
    }
    if (*s == '-') {
        s++;
        e->s = s;
        return -ev_primary(w, e);
    }
    if (*s >= '0' && *s <= '9') {
        int64_t v = 0;
        while (*s >= '0' && *s <= '9') {
            v = v * 10 + (*s - '0');
            s++;
        }
        e->s = s;
        return v;
    }
    {
        char name[96], sub[96];
        int nl = 0, sl;
        int64_t v = 0;
        int hasidx = 0, idx = 0;
        while ((*s >= 'a' && *s <= 'z') || (*s >= 'A' && *s <= 'Z') ||
               (*s >= '0' && *s <= '9') || *s == '_') {
            if (nl < 95) name[nl++] = *s;
            s++;
        }
        name[nl] = 0;
        if (!nl) { e->err = 1; return 0; }
        if (!strcmp(name, "i")) {
            e->s = s;
            return w->cur_i;
        }
        sub[0] = 0;
        if (s[0] == ':' && s[1] == ':') {
            s += 2;
            sl = 0;
            while ((*s >= 'a' && *s <= 'z') || (*s >= 'A' && *s <= 'Z') ||
                   (*s >= '0' && *s <= '9') || *s == '_') {
                if (sl < 95) sub[sl++] = *s;
                s++;
            }
            sub[sl] = 0;
        }
        while (*s == ' ') s++;
        if (*s == '[') {
            EV ie;
            s++;
            ie.s = s;
            ie.err = 0;
            v = ev_add(w, &ie);
            s = ie.s;
            if (ie.err) { e->err = 1; return 0; }
            while (*s == ' ') s++;
            if (*s == ']') s++;
            hasidx = 1;
            idx = (int)v;
        }
        e->s = s;
        /* resolve */
        {
            int i, j;
            if (!sub[0]) {
                for (i = w->nvstack - 1; i >= 0; i--) {
                    const T4ZStruct *st = w->vstack[i].st;
                    const uint8_t *base = w->vstack[i].base;
                    const T4ZField *f = fld_find(st, name);
                    const uint8_t *pp;
                    int n, es;
                    long sv;
                    if (!f) continue;
                    if (hasidx) {
                        if (val_arr(st, base, name, &pp, &n, &es)) {
                            if (idx < 0 || idx >= n) { e->err = 1; return 0; }
                            if (es == 1) return pp[idx];
                            if (es == 2) return (int64_t)rd16(pp + 2 * idx);
                            return (int64_t)(int32_t)rd32(pp + 4 * idx);
                        }
                        e->err = 1;
                        return 0;
                    }
                    if (val_scalar(st, base, name, &sv))
                        return sv;
                    (void)j;
                }
                e->err = 1;
                return 0;
            }
            /* qualificado X::campo: name=owner (struct), sub=campo
             * (fftool._resolve_qual: '_q_<owner>_<campo>') */
            for (i = w->nvstack - 1; i >= 0; i--) {
                const T4ZStruct *st = w->vstack[i].st;
                const uint8_t *base = w->vstack[i].base;
                long sv;
                const uint8_t *pp;
                int n, es;
                if (strcmp(st->name, name)) continue;
                if (hasidx) {
                    if (val_arr(st, base, sub, &pp, &n, &es)) {
                        if (idx < 0 || idx >= n) { e->err = 1; return 0; }
                        if (es == 1) return pp[idx];
                        if (es == 2) return (int64_t)rd16(pp + 2 * idx);
                        return (int64_t)(int32_t)rd32(pp + 4 * idx);
                    }
                    e->err = 1;
                    return 0;
                }
                if (val_scalar(st, base, sub, &sv))
                    return sv;
            }
            /* fallback: owner e o nome de um campo embutido (sub-struct de
             * tipo desconhecido da walk); le o campo `sub` do blob */
            for (i = w->nvstack - 1; i >= 0; i--) {
                const T4ZStruct *st = w->vstack[i].st;
                const uint8_t *base = w->vstack[i].base;
                int k;
                for (k = 0; k < st->nfields; k++) {
                    const T4ZField *f = &st->fields[k];
                    long sv;
                    const uint8_t *pp;
                    int n, es;
                    if (f->ptr || !f->type || strcmp(f->name, name)) continue;
                    if (is_union_type(f->type)) continue;
                    if (!t4z_struct(f->type)) continue;
                    if (hasidx) {
                        if (val_arr(t4z_struct(f->type), base + f->off, sub,
                                    &pp, &n, &es)) {
                            if (idx < 0 || idx >= n) { e->err = 1; return 0; }
                            if (es == 1) return pp[idx];
                            if (es == 2) return (int64_t)rd16(pp + 2 * idx);
                            return (int64_t)(int32_t)rd32(pp + 4 * idx);
                        }
                        continue;
                    }
                    if (val_scalar(t4z_struct(f->type), base + f->off, sub, &sv))
                        return sv;
                }
            }
            e->err = 1;
            return 0;
        }
    }
}

static int64_t ev_unary(T4ZW *w, EV *e)
{
    while (e->s[0] == ' ') e->s++;
    if (e->s[0] == '-') {
        e->s++;
        return -ev_unary(w, e);
    }
    return ev_primary(w, e);
}

static int64_t ev_mul(T4ZW *w, EV *e)
{
    int64_t v = ev_unary(w, e);
    for (;;) {
        while (e->s[0] == ' ') e->s++;
        if (e->s[0] == '*' && !e->err) {
            e->s++;
            v = v * ev_unary(w, e);
        } else if (e->s[0] == '/' && !e->err) {
            int64_t d;
            e->s++;
            if (e->s[0] == '/') e->s++;
            d = ev_unary(w, e);
            if (d == 0) { e->err = 1; return 0; }
            v = v / d;
        } else if (e->s[0] == '%' && !e->err) {
            int64_t d;
            e->s++;
            d = ev_unary(w, e);
            if (d == 0) { e->err = 1; return 0; }
            v = v % d;
        } else break;
    }
    return v;
}

static int64_t ev_add(T4ZW *w, EV *e)
{
    int64_t v = ev_mul(w, e);
    for (;;) {
        while (e->s[0] == ' ') e->s++;
        if (e->s[0] == '+' && !e->err) {
            e->s++;
            v = v + ev_mul(w, e);
        } else if (e->s[0] == '-' && !e->err) {
            e->s++;
            v = v - ev_mul(w, e);
        } else break;
    }
    return v;
}

static long ecount(T4ZW *w, const char *expr)
{
    EV e;
    int64_t v;
    if (!expr) return 0;
    e.s = expr;
    e.err = 0;
    v = ev_add(w, &e);
    if (e.err) {
        failf(w, "invalid count '%s'", expr);
        return -1;
    }
    return (long)v;
}

/* contagem de um spec: expr string ou inteiro literal */
static long spec_cnt(T4ZW *w, const T4ZSpec *sp)
{
    if (sp->e2) return ecount(w, sp->e2);
    return sp->a;
}

static int cmp_eval(int op, int64_t v, int b)
{
    switch (op) {
    case C_LT: return v < b;
    case C_GT: return v > b;
    case C_GE: return v >= b;
    default:   return v == b;
    }
}

static const T4ZSpec *choose_spec(T4ZW *w, const T4ZSpec *sp)
{
    int64_t v = ecount(w, sp->e);
    int op = sp->a;
    if (v < 0 && !w->err[0]) { failf(w, "invalid choose '%s'", sp->e); return NULL; }
    /* ops *_S: a anotacao py compara com _s32 (bit31 = signed); ecount le
     * unsigned, entao refaz o cast aqui antes do comparador. */
    if (op >= C_LT_S && op <= C_GE_S) {
        v = (int32_t)v;
        op -= C_LT_S - C_LT;
    }
    return cmp_eval(op, v, sp->b) ? sp->sub : sp->sub2;
}

static const T4ZSpec *choisk_spec(T4ZW *w, const T4ZSpec *sp)
{
    int64_t v = ecount(w, sp->e);
    int i;
    /* re/gen_tables.py grava a contagem de ents em .a (o .nents nunca e
     * inicializado).  Sem o fallback em .a o loop nunca rodava e todo
     * choose caia no sub (K_SKIP), pulando pointees como BYTES(16). */
    int n = sp->nents ? sp->nents : sp->a;
    if (v < 0 && !w->err[0]) { failf(w, "invalid choose '%s'", sp->e); return NULL; }
    for (i = 0; i < n; i++)
        if ((int64_t)sp->ents[i].key == v)
            return sp->ents[i].spec;
    return sp->sub;
}

/* ------------------------------------------------- proc/campo/pointee */

static int proc(T4ZW *w, const T4ZStruct *st, const uint8_t *base);

static int read_string(T4ZW *w, const char *sname, const T4ZField *f)
{
    size_t p = w->pos;
    while (p < w->len && w->d[p]) p++;
    if (p >= w->len)
        return failf(w, "string sem terminador em %zu", w->pos);
    if (getenv("T4_ZW_DEBUG"))
        fprintf(stderr, "[zw] str pos=%zu len=%zu %s.%s\n", w->pos, p - w->pos,
                sname ? sname : "?", f ? f->name : "?");
    if (w->atype == 17 && sname && f &&
        !strcmp(sname, "GfxWorld") && !strcmp(f->name, "name")) {
        size_t n = p - w->pos;
        if (n > sizeof(w->g_name) - 1) n = sizeof(w->g_name) - 1;
        memcpy(w->g_name, w->d + w->pos, n);
        w->g_name[n] = 0;
    }
    w->pos = p + 1;
    return 0;
}

static int pointee(T4ZW *w, const char *sname, const T4ZField *f,
                   const T4ZSpec *sp, long poff, uint32_t uv, int inline_data);

/* unionmember: acha o membro `member` descendo unions aninhados */
static int um_member(const T4ZStruct *st, const char *member,
                     const T4ZField **out)
{
    const T4ZStruct *cur = st;
    int depth;
    for (depth = 0; depth < 8; depth++) {
        const T4ZField *mf = fld_find(cur, member);
        int i;
        if (mf) { *out = mf; return 0; }
        for (i = 0; i < cur->nfields; i++) {
            const char *ty = cur->fields[i].type;
            char buf[128];
            const T4ZStruct *nt;
            if (!ty) continue;
            snprintf(buf, sizeof(buf), "union:%s", ty);
            nt = t4z_struct(buf);
            if (nt && fld_find(nt, member)) { cur = nt; goto next; }
        }
        return -1;
    next: ;
    }
    return -1;
}

static int load_alloc(T4ZW *w, const char *sname);

static int pointee(T4ZW *w, const char *sname, const T4ZField *f,
                   const T4ZSpec *sp, long poff, uint32_t uv, int inline_data)
{
    const char *ty = f->type;
    char stripped[128];
    (void)uv;

    if (ty) {
        size_t n = strlen(ty);
        while (n && ty[n - 1] == '*') n--;
        if (n >= sizeof(stripped)) n = sizeof(stripped) - 1;
        memcpy(stripped, ty, n);
        stripped[n] = 0;
        ty = stripped;
    }

    if (!sp || sp->kind == K_STR) {
        if (sp) return read_string(w, sname, f);
        if (ty && !strcmp(ty, "char"))
            return read_string(w, sname, f);
        if (f->type && strstr(f->type, "**"))
            return failf(w, "missing annotation %s.%s (%s)", sname, f->name, f->type);
        if (ty && t4z_struct(ty) && !is_union_type(ty))
            return load_alloc(w, ty);
        return failf(w, "missing annotation %s.%s (%s)", sname, f->name,
                     f->type ? f->type : "?");
    }

    switch (sp->kind) {
    case K_SKIP:
        return 0;
    case K_BYTES: {
        long n = ecount(w, sp->e);
        if (n < 0) return -1;
        if (w->pos + (size_t)n > w->len)
            return failf(w, "bytes '%s' outside the payload (%zu+%ld > %zu)",
                         sp->e, w->pos, n, w->len);
        w->pos += (size_t)n;
        return 0;
    }
    case K_U16: {
        long n = ecount(w, sp->e);
        if (n < 0) return -1;
        if (w->pos + 2 * (size_t)n > w->len)
            return failf(w, "u16 '%s' outside the payload", sp->e);
        if (w->atype == 17 && sname && f &&
            !strcmp(sname, "GfxWorld") && !strcmp(f->name, "indices")) {
            w->g_idx = w->d + w->pos;
            w->g_idxn = (size_t)n;
        }
        w->pos += 2 * (size_t)n;
        return 0;
    }
    case K_STRUCT: {
        const T4ZStruct *es = t4z_struct(sp->e);
        long n = spec_cnt(w, sp);
        size_t start, i;
        if (!es) return failf(w, "unknown struct %s", sp->e);
        if (n < 0) return -1;
        if (inline_data) {
            start = w->pos;
            if (start + es->size * (size_t)n > w->len)
                return failf(w, "array %s[%ld] outside the payload", sp->e, n);
            w->pos = start + es->size * (size_t)n;
        } else {
            start = poff >= 0 ? (size_t)poff : w->pos;
        }
        if (!strcmp(sp->e, "GfxWorldVertex") && sname && f &&
            !strcmp(sname, "GfxWorldVertexData") && !strcmp(f->name, "vertices")) {
            w->g_vtx = w->d + start;
            w->g_nvtx = (size_t)n;
        }
        for (i = 0; i < (size_t)n; i++) {
            if (proc(w, es, w->d + start + es->size * i) != 0)
                return -1;
        }
        return 0;
    }
    case K_STRUCTU: {
        long n = spec_cnt(w, sp);
        size_t start, i;
        if (n < 0) return -1;
        start = inline_data ? w->pos : (poff >= 0 ? (size_t)poff : w->pos);
        if (start + 4 * (size_t)n > w->len)
            return failf(w, "slots de union[%ld] outside the payload", n);
        w->pos = start + 4 * (size_t)n;
        for (i = 0; i < (size_t)n; i++) {
            uint32_t p = rd32(w->d + start + 4 * i);
            if (is_following(p)) {
                T4ZField dummy;
                const T4ZSpec *um = sp->sub;
                memset(&dummy, 0, sizeof(dummy));
                dummy.name = (um && um->kind == K_UM && um->name) ? um->name : "x";
                dummy.ptr = 1;
                dummy.countval = -1;
                if (pointee(w, sname, &dummy, um ? um->sub : sp->sub,
                            (long)(start + 4 * i), p, 1) != 0)
                    return -1;
            }
        }
        return 0;
    }
    case K_STRUCTEX: {
        const T4ZStruct *es = t4z_struct(sp->e);
        long n = spec_cnt(w, sp);
        long extra;
        long i;
        if (!es) return failf(w, "unknown struct %s", sp->e);
        if (n < 0) return -1;
        for (i = 0; i < n; i++)
            if (load_alloc(w, es->name) != 0) return -1;
        extra = ecount(w, sp->e2);
        if (extra < 0) return -1;
        if (w->pos + (size_t)extra > w->len)
            return failf(w, "struct_extra outside the payload");
        w->pos += (size_t)extra;
        return 0;
    }
    case K_EARR: {
        const T4ZStruct *es = t4z_struct(sp->e);
        T4ZSpec tmp;
        if (!es) return failf(w, "unknown struct %s", sp->e);
        memset(&tmp, 0, sizeof(tmp));
        tmp.kind = K_STRUCT;
        tmp.e = sp->e;
        tmp.a = sp->a;
        tmp.e2 = sp->e2;
        return pointee(w, sname, f, &tmp, poff, uv, inline_data);
    }
    case K_EARRP: {
        long cnt = ecount(w, sp->e2);
        long i;
        if (cnt < 0) return -1;
        if (w->pos + 4 * (size_t)cnt > w->len)
            return failf(w, "earrptr outside the payload");
        for (i = 0; i < cnt; i++) {
            uint32_t p = rd32(w->d + w->pos + 4 * i);
            if (is_following(p)) {
                if (!strcmp(sp->e, "str")) {
                    if (read_string(w, sname, f) != 0) return -1;
                } else {
                    if (load_alloc(w, sp->e) != 0) return -1;
                }
            }
        }
        w->pos += 4 * (size_t)cnt;
        return 0;
    }
    case K_PTRARR: {
        long cnt = ecount(w, sp->e2);
        long i;
        size_t abase;
        if (cnt < 0) return -1;
        if (w->pos + 4 * (size_t)cnt > w->len)
            return failf(w, "ptrarr outside the payload");
        /* o array inteiro de ponteiros e consumido ANTES de seguir qualquer
         * pointee (fftool.py 'ptrarr': pos += 4*cnt, entao load_alloc dos
         * elementos).  Seguir antes lia o blob por cima do proprio array. */
        abase = w->pos;
        w->pos += 4 * (size_t)cnt;
        for (i = 0; i < cnt; i++) {
            uint32_t p = rd32(w->d + abase + 4 * i);
            if (is_following(p)) {
                if (sp->e && !strcmp(sp->e, "str")) {
                    if (read_string(w, sname, f) != 0) return -1;
                } else if (sp->sub) {
                    if (pointee(w, sname, f, sp->sub, (long)(abase + 4 * i),
                                p, 1) != 0)
                        return -1;
                } else {
                    if (load_alloc(w, sp->e) != 0) return -1;
                }
            }
        }
        return 0;
    }
    case K_UM: {
        const T4ZStruct *ut = union_struct(f->type);
        const T4ZField *mf = NULL;
        int inline_data2;
        if (!ut) return failf(w, "unionmember in %s.%s is not a union", sname, f->name);
        if (um_member(ut, sp->name, &mf) != 0 || !mf)
            return failf(w, "union member %s not found in %s", sp->name, f->type);
        if (mf->ptr && !is_following(uv)) return 0;
        if (!mf->ptr && is_union_type(mf->type) && !is_following(uv)) return 0;
        inline_data2 = (mf->ptr || is_union_type(mf->type)) ? 1 : 0;
        return pointee(w, sname, mf, sp->sub, poff, uv, inline_data2);
    }
    case K_CHOOSE: {
        const T4ZSpec *c = choose_spec(w, sp);
        if (w->err[0]) return -1;
        if (!c) return 0;
        return pointee(w, sname, f, c, poff, uv, inline_data);
    }
    case K_CHOISK: {
        const T4ZSpec *c = choisk_spec(w, sp);
        if (w->err[0]) return -1;
        if (!c) return 0;
        return pointee(w, sname, f, c, poff, uv, inline_data);
    }
    case K_UARR: {
        long n = spec_cnt(w, sp);
        size_t pbase;
        long i;
        if (n < 0) return -1;
        pbase = w->pos;
        if (pbase + 4 * (size_t)n > w->len)
            return failf(w, "uarr outside the payload");
        w->pos = pbase + 4 * (size_t)n;
        for (i = 0; i < n; i++) {
            uint32_t p = rd32(w->d + pbase + 4 * i);
            if (is_following(p)) {
                T4ZField dummy;
                memset(&dummy, 0, sizeof(dummy));
                dummy.name = sp->name;
                dummy.ptr = 1;
                dummy.countval = -1;
                if (pointee(w, sname, &dummy, sp->sub, (long)(pbase + 4 * i),
                            p, 1) != 0)
                    return -1;
            }
        }
        return 0;
    }
    case K_PARR2: {
        const T4ZStruct *es = t4z_struct(sp->e);
        size_t pbase = poff >= 0 ? (size_t)poff : w->pos;
        int n = sp->a > 0 ? sp->a : 2;
        int i;
        if (!es) return failf(w, "unknown struct %s", sp->e);
        if (pbase + 4 * (size_t)n > w->len)
            return failf(w, "parr2 outside the payload");
        for (i = 0; i < n; i++) {
            uint32_t p = rd32(w->d + pbase + 4 * i);
            if (is_following(p))
                if (load_alloc(w, es->name) != 0) return -1;
        }
        return 0;
    }
    case K_INLINE: {
        const T4ZStruct *is = t4z_struct(sp->e);
        if (!is) return failf(w, "unknown struct %s", sp->e);
        return proc(w, is, w->d);
    }
    case K_BAD:
        return failf(w, "unsupported spec (%s.%s: %s)", sname, f->name,
                     sp->name ? sp->name : "?");
    default:
        return failf(w, "unexpected spec %d in %s.%s", sp->kind, sname, f->name);
    }
}

static int field(T4ZW *w, const T4ZStruct *st, const T4ZField *f,
                 const uint8_t *base);

static int proc(T4ZW *w, const T4ZStruct *st, const uint8_t *base)
{
    const struct SeqEnt *seq = NULL;
    int seqn = 0, k;
    if (w->nvstack >= (int)(sizeof(w->vstack) / sizeof(w->vstack[0])))
        return failf(w, "struct stack overflow at %s", st->name);
    seq = seq_get(w, st, &seqn);
    if (!seq) return -1;
    w->vstack[w->nvstack].st = st;
    w->vstack[w->nvstack].base = base;
    w->nvstack++;
    for (k = 0; k < seqn; k++) {
        int rc;
        if (seq[k].embed) {
            const T4ZField *pf = &st->fields[seq[k].pf];
            const T4ZStruct *ps = t4z_struct(pf->type);
            if (!ps) {
                w->nvstack--;
                return failf(w, "embed %s.%s: missing struct %s", st->name,
                             pf->name, pf->type ? pf->type : "?");
            }
            rc = proc(w, ps, base + pf->off);
        } else {
            rc = field(w, st, &st->fields[seq[k].f], base);
        }
        if (rc != 0) {
            w->nvstack--;
            return -1;
        }
    }
    w->nvstack--;
    return 0;
}

static int field(T4ZW *w, const T4ZStruct *st, const T4ZField *f,
                 const uint8_t *base)
{
    const T4ZSpec *spec = ann_lookup(st->name, f->name);

    if (getenv("T4_ZW_DEBUG")) {
        /* faixa opcional de posicao para o diff contra o trace do Python
         * (re/fftool.py) -- evita flood de dezenas de milhoes de linhas */
        static long fl_from = -1, fl_to = -1;
        static int fl_init = 0;
        if (!fl_init) {
            const char *a = getenv("T4_ZW_FLD_FROM");
            const char *b = getenv("T4_ZW_FLD_TO");
            fl_from = a ? atol(a) : -1;
            fl_to = b ? atol(b) : -1;
            fl_init = 1;
        }
        if ((fl_from < 0 || (long)w->pos >= fl_from) &&
            (fl_to < 0 || (long)w->pos < fl_to))
            fprintf(stderr, "[zw] fld pos=%zu %s.%s\n", w->pos, st->name,
                    f->name);
    }

    if (!f->ptr) {
        if (spec && (spec->kind == K_CHOOSE || spec->kind == K_CHOISK)) {
            spec = spec->kind == K_CHOISK ? choisk_spec(w, spec)
                                          : choose_spec(w, spec);
            if (w->err[0]) return -1;
        }
        if (spec && spec->kind == K_BYTES) {
            long n = ecount(w, spec->e);
            if (n < 0) return -1;
            if (w->pos + (size_t)n > w->len)
                return failf(w, "%s.%s bytes outside the payload", st->name, f->name);
            w->pos += (size_t)n;
            return 0;
        }
        if (spec && spec->kind == K_DYNIDX) {
            int64_t fv = 0;
            long n;
            int sel;
            if (!ident_lookup(w, spec->e, 0, 0, &fv))
                return failf(w, "%s.%s dynidx: missing field %s", st->name,
                             f->name, spec->e);
            sel = cmp_eval(spec->a, fv, spec->b);
            n = ecount(w, spec->e2);
            if (n < 0) return -1;
            w->pos = (size_t)(base + f->off + n * (sel ? 1 : 2));
            return 0;
        }
        if (spec && spec->kind == K_INLAT) {
            long n = ecount(w, spec->e);
            if (n < 0) return -1;
            w->pos = (size_t)(base + spec->a + n);
            return 0;
        }
        if (is_union_type(f->type)) {
            const T4ZStruct *ut = union_struct(f->type);
            uint32_t uv = 0;   /* nao inicializado virava UB em is_following */
            if (getenv("T4_ZW_DEBUG"))
                fprintf(stderr, "[zw] union %s.%s kind=%d base=%ld\n",
                        st->name, f->name, spec ? spec->kind : -1,
                        (long)(base - w->d));
            if (!spec) {
                if (has_pointers(f->type, 0))
                    return failf(w, "missing annotation for union %s.%s (%s)",
                                 st->name, f->name, f->type);
                return 0;
            }
            if (spec->kind == K_INLINE) {
                const T4ZStruct *is = t4z_struct(spec->e);
                if (!is) return failf(w, "unknown struct %s", spec->e);
                return proc(w, is, base + f->off);
            }
            if (spec->kind == K_DYNFR) {
                uv = rd32(base + f->off);
                if (is_following(uv)) {
                    int64_t fv = 0;
                    long n;
                    int sel;
                    if (!ident_lookup(w, spec->e, 0, 0, &fv))
                        return failf(w, "%s.%s dynframes: missing field %s",
                                     st->name, f->name, spec->e);
                    sel = cmp_eval(spec->a, fv, spec->b);
                    n = ecount(w, spec->e2);
                    if (n < 0) return -1;
                    w->pos += (size_t)(n * (sel ? 1 : 2));
                }
                return 0;
            }
            uv = rd32(base + f->off);
            if (spec->kind == K_CHOOSE || spec->kind == K_CHOISK) {
                /* union por valor com choose: o slot guarda o ponteiro
                 * ativo do membro escolhido (re/fftool.py: "the slot holds
                 * the active inner pointer").  Sem ler uv aqui o pointee
                 * FOLLOWING era pulado (uv indevido em is_following). */
                const T4ZSpec *cs = (spec->kind == K_CHOISK)
                    ? choisk_spec(w, spec) : choose_spec(w, spec);
                if (w->err[0]) return -1;
                uv = rd32(base + f->off);
                spec = cs;
                if (getenv("T4_ZW_DEBUG"))
                    fprintf(stderr,
                            "[zw] uchoose base=%ld uv=%08X ckind=%d ce=%s\n",
                            (long)(base - w->d), uv, spec ? spec->kind : -1,
                            (spec && spec->e) ? spec->e : "-");
                if (!spec) return 0;
            }
            if (spec->kind != K_UM) {
                if (is_following(uv))
                    return pointee(w, st->name, f, spec, (long)(base - w->d) + f->off, uv, 1);
                return 0;
            }
            {
                const T4ZField *mf = NULL;
                if (!ut) return failf(w, "missing union %s", f->type);
                if (um_member(ut, spec->name, &mf) != 0 || !mf)
                    return failf(w, "union member %s not found in %s.%s",
                                 spec->name, st->name, f->type);
                if (mf->ptr && !is_following(uv)) return 0;
                if (!mf->ptr && is_union_type(mf->type) && !is_following(uv))
                    return 0;
                return pointee(w, st->name, mf, spec->sub,
                               (long)(base - w->d) + f->off, uv,
                               (mf->ptr || is_union_type(mf->type)) ? 1 : 0);
            }
        }
        if (spec && spec->kind == K_EARR) {
            const T4ZStruct *es = t4z_struct(spec->e);
            long n = spec_cnt(w, spec);
            long i;
            if (!es) return failf(w, "unknown struct %s", spec->e);
            if (n < 0) return -1;
            w->pos += (size_t)(n * (long)es->size) - f->size;
            for (i = 0; i < n; i++)
                if (proc(w, es, base + f->off + es->size * (size_t)i) != 0)
                    return -1;
            return 0;
        }
        if (spec && spec->kind == K_EARRP)
            return 0;
        if (spec && spec->kind == K_INLINE) {
            const T4ZStruct *is = t4z_struct(spec->e);
            if (!is) return failf(w, "unknown struct %s", spec->e);
            return proc(w, is, base + f->off);
        }
        if (f->type && !is_union_type(f->type) &&
            (f->countval == -1 || f->countval == 1)) {
            const T4ZStruct *ts = t4z_struct(f->type);
            if (ts)
                return proc(w, ts, base + f->off);
        }
        return 0;
    }

    /* ponteiro */
    if (is_runtime(st->name, f->name))
        return 0;
    if (f->countval > 1) {
        const uint8_t *pp = base + f->off;
        int n = f->countval;
        int i;
        if (spec && spec->kind == K_EARRP) {
            for (i = 0; i < n; i++) {
                uint32_t p = rd32(pp + 4 * i);
                if (is_following(p)) {
                    if (!strcmp(spec->e, "str")) {
                        if (read_string(w, st->name, f) != 0) return -1;
                    } else {
                        if (load_alloc(w, spec->e) != 0) return -1;
                    }
                }
            }
            return 0;
        }
        if (spec && spec->kind == K_PSTARR) {
            const T4ZStruct *es = t4z_struct(spec->e);
            if (!es) return failf(w, "unknown struct %s", spec->e);
            for (i = 0; i < n; i++) {
                uint32_t p = rd32(pp + 4 * i);
                if (is_following(p)) {
                    long cnt;
                    size_t start;
                    long j;
                    w->cur_i = i;
                    cnt = ecount(w, spec->e2);
                    if (cnt < 0) return -1;
                    start = w->pos;
                    if (start + es->size * (size_t)cnt > w->len)
                        return failf(w, "%s.%s pstructarr outside the payload",
                                     st->name, f->name);
                    w->pos = start + es->size * (size_t)cnt;
                    for (j = 0; j < cnt; j++)
                        if (proc(w, es, w->d + start + es->size * (size_t)j) != 0)
                            return -1;
                }
            }
            return 0;
        }
        if (spec && spec->kind == K_PCOUNT) {
            for (i = 0; i < n; i++) {
                uint32_t p = rd32(pp + 4 * i);
                if (is_following(p)) {
                    long cnt;
                    w->cur_i = i;
                    cnt = ecount(w, spec->e);
                    if (cnt < 0) return -1;
                    if (w->pos + (size_t)(spec->a) * (size_t)cnt > w->len)
                        return failf(w, "%s.%s pcount outside the payload",
                                     st->name, f->name);
                    w->pos += (size_t)(spec->a) * (size_t)cnt;
                }
            }
            return 0;
        }
        for (i = 0; i < n; i++) {
            uint32_t p = rd32(pp + 4 * i);
            if (is_following(p))
                if (pointee(w, st->name, f, spec, (long)(pp + 4 * i - w->d),
                            p, 1) != 0)
                    return -1;
        }
        return 0;
    }
    {
        uint32_t v = rd32(base + f->off);
        if (is_following(v))
            return pointee(w, st->name, f, spec, (long)(base - w->d) + f->off,
                           v, 1);
    }
    return 0;
}

static int load_alloc(T4ZW *w, const char *sname)
{
    const T4ZStruct *st = t4z_struct(sname);
    const uint8_t *base;
    if (!st) return failf(w, "unknown struct %s", sname);
    if (w->pos + st->size > w->len)
        return failf(w, "blob %s outside the payload (%zu+%d > %zu)", sname,
                     w->pos, st->size, w->len);
    base = w->d + w->pos;
    if (getenv("T4_ZW_DEBUG"))
        fprintf(stderr, "[zw] atype=%-3d %-24s pos=%zu size=%d\n",
                w->atype, sname, w->pos, st->size);
    w->pos += (size_t)st->size;
    if (w->atype == 17 && !strcmp(sname, "GfxWorld") && !w->g_blob)
        w->g_blob = base;
    return proc(w, st, base);
}

/* ------------------------------------------------------------- publico */

void t4z_init(T4ZW *w, const uint8_t *d, size_t len)
{
    memset(w, 0, sizeof(*w));
    w->d = d;
    w->len = len;
}

int t4z_load_alloc(T4ZW *w, const char *sname)
{
    return load_alloc(w, sname);
}
