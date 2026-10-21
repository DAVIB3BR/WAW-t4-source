/*
 * world.h - carregamento nativo do GfxWorld (mundo 3D) de um fastfile T4.
 *
 * Usa o walker estruturado (zonewalk.c) para consumir o asset GfxWorld e
 * extrair vertices (GfxWorldVertex) e indices (u16) para desenho direto.
 */
#ifndef T4_WORLD_H
#define T4_WORLD_H

#include <stddef.h>
#include <stdint.h>

#include "ff.h"

typedef struct {
    char      name[64];     /* nome do mundo (ex.: "mp_dome") */
    uint32_t  nverts, nidx;
    float    *pos;          /* nverts * 3 (xyz) */
    float    *nrm;          /* nverts * 3 (normal descompactada) */
    uint16_t *idx;          /* nidx indices */
    float     mins[3], maxs[3];
    int       surfaces;     /* dpvs.surfaceCount */
    int       walk_exact;   /*1 = walker terminou exatamente no fim do payload */
    char      walk_err[256];/* erro do walker depois do GfxWorld (aviso) */
} T4World;

/* Carrega o GfxWorld do .ff ja parseado (t4_ff_load + t4_ff_parse).
 * Retorna 0 ok, <0 erro (mensagem em err). */
int  t4_world_load(T4World *wv, const T4FF *ff, char *err, size_t errcap);
void t4_world_free(T4World *wv);

#endif
