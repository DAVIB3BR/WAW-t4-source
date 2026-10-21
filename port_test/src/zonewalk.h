/*
 * zonewalk.h - walker estruturado de fastfile T4 (IWffu100 v387) em C.
 *
 * Porte fiel de re/fftool.py (validado em 129/129 arquivos, 150.211 assets).
 * As tabelas (structs/offsets, regras ANN, RUNTIME, reorders, mapa de tipos)
 * vem de zonetables.inc, gerado por re/gen_tables.py a partir das mesmas
 * tabelas do walker Python.
 *
 * Semantica reproduzida (ver re/FF_FORMAT.md):
 *   - blob do struct = layout MSVC 32 bits (offsets de structs.json);
 *   - ponteiro = 4 bytes no blob; pointee consumido SE (block<<29|off+1)
 *     tiver offset+1 != 0 (FOLLOWING/INSERT = inline no cursor de alocacao);
 *   - membros `set block ... RUNTIME` nao consomem bytes;
 *   - array de structs: blobs primeiro, depois os pointees de cada elemento;
 *   - strings NUL terminadas avancam o cursor;
 *   - reorders OAT SequenceReorder (ORDER/ORDER_V2) aplicados na ordem.
 */
#ifndef T4_ZONEWALK_H
#define T4_ZONEWALK_H

#include <stddef.h>
#include <stdint.h>

/* kinds de spec (espelham os construtores de re/fftool.py) */
enum {
    K_STR = 1, K_SKIP, K_BYTES, K_U16, K_STRUCT, K_STRUCTU, K_STRUCTEX,
    K_EARR, K_EARRP, K_PTRARR, K_UM, K_PCOUNT, K_PSTARR, K_INLINE, K_UARR,
    K_PARR2, K_CHOOSE, K_CHOISK, K_DYNIDX, K_DYNFR, K_INLAT, K_BAD
};
/* comparadores de choose/dynidx */
enum { C_LT = 1, C_GT, C_GE, C_EQ, C_LT_S, C_GT_S, C_GE_S };

typedef struct T4ZSpec T4ZSpec;
typedef struct { int key; const T4ZSpec *spec; } T4ZChooseEnt;

struct T4ZSpec {
    int  kind;
    int  a, b;
    const char *e, *e2, *name;
    const T4ZSpec *sub, *sub2;
    const T4ZChooseEnt *ents;
    int  nents;
};

typedef struct {
    const char *name;
    int off, size, ptr, countval;   /* countval -1 = sem contagem */
    const char *type;
} T4ZField;

typedef struct {
    const char *name;
    int size, nfields;
    const T4ZField *fields;
    int is_union;
} T4ZStruct;

typedef struct { const char *a, *b; } T4ZPairEnt;
typedef struct {
    const char *sname;
    int anchor, n;
    const char *const *names;
} T4ZOrderEnt;
typedef struct { const char *sname, *fname; const T4ZSpec *spec; } T4ZAnnEnt;

#define KENT(k, s) { k, s }

typedef struct T4ZW {
    const uint8_t *d;
    size_t len, pos;
    char err[512];
    /* pilha de structs ativas (para ecount e reorders) */
    struct { const T4ZStruct *st; const uint8_t *base; } vstack[96];
    int nvstack;
    int cur_i;                       /* indice de elemento (pcount/uarr) */
    /* registro do GfxWorld (asset tipo 17) durante o walk */
    int atype;
    const uint8_t *g_blob;         /* blob inline do GfxWorld (800 B) */
    const uint8_t *g_idx;
    size_t g_idxn;
    const uint8_t *g_vtx;
    size_t g_nvtx;
    char g_name[64];
} T4ZW;

void t4z_init(T4ZW *w, const uint8_t *d, size_t len);
const T4ZStruct *t4z_struct(const char *name);
int t4z_field_off(const char *sname, const char *fname);       /* -1 se ausente */
int t4z_field_size(const char *sname, const char *fname);      /* -1 se ausente */
const char *t4z_typename(int asset_type);                      /* NULL se desconhecido */
int t4z_load_alloc(T4ZW *w, const char *sname);                /* 0 ok */
const char *t4z_error(const T4ZW *w);

#endif
