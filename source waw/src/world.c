/*
 * world.c - extrai o GfxWorld de um fastfile T4 com o walker estruturado e
 * monta os arrays de desenho (vertices + indices) para o renderer nativo.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ff.h"
#include "zonewalk.h"
#include "world.h"

static uint32_t rd32c(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static float rdcf(const uint8_t *p)
{
    uint32_t u = rd32c(p);
    float f;
    memcpy(&f, &u, 4);
    return f;
}

static void unpack_nrm(uint32_t packed, float *out)
{
    out[0] = (float)((packed & 0xFF)) / 127.5f - 1.0f;
    out[1] = (float)((packed >> 8) & 0xFF) / 127.5f - 1.0f;
    out[2] = (float)((packed >> 16) & 0xFF) / 127.5f - 1.0f;
}

int t4_world_load(T4World *wv, const T4FF *ff, char *err, size_t errcap)
{
    T4ZW w;
    uint32_t i;
    int have_world = 0;
    int off_xyz, off_nrm, off_vc, off_mins, off_maxs, off_surfs;
    int vsz;
    uint32_t nverts, nidx;
    size_t v;
    long bad_pos = 0, bad_idx = 0;

    memset(wv, 0, sizeof(*wv));
    if (!ff || !ff->data || !ff->asset_types) {
        snprintf(err, errcap, "world: ff nao parseado");
        return -1;
    }
    t4z_init(&w, ff->data, ff->len);
    /* O stream de conteudo comeca em data_start (arena VIRTUAL, ver
     * re/FF_FORMAT.md §3): caminhar do byte0 lia cabecalho+strings como
     * se fossem assets (pos=0 no trace T4_ZW_DEBUG) e so parava por sorte. */
    w.pos = (size_t)ff->data_start;

    for (i = 0; i < ff->asset_count; i++) {
        uint32_t t = rd32c((const uint8_t *)ff->asset_types + 8 * i);
        const char *sname = t4z_typename((int)t);
        w.nvstack = 0;
        w.atype = (int)t;
        if (getenv("T4_WORLD_DEBUG"))
            fprintf(stderr, "[world] asset %u tipo %u (%s) pos=%zu\n", i, t,
                    sname ? sname : "?", w.pos);
        if (t == 17 && getenv("T4_ZW_DUMP")) {
            const uint8_t *b = ff->data + w.pos;
            uint32_t namep = rd32c(b), idxp = rd32c(b + 20);
            uint32_t vdp = rd32c(b + 56), w64 = rd32c(b + 64);
            /* mapeamento arena VIRTUAL -> payload: candidatos empiricos */
            const uint32_t cand[3] = { ff->data_start, 52u, 0 };
            int ci;
            fprintf(stderr,
                "[dump] GfxWorld @%zu name=%08X idx=%08X vd=%08X +64=%08X "
                "blocks T=%u R=%u LR=%u PR=%u V=%u L=%u P=%u "
                "data_start=%u len=%zu\n",
                w.pos, namep, idxp, vdp, w64,
                ff->blocks[0], ff->blocks[1], ff->blocks[2], ff->blocks[3],
                ff->blocks[4], ff->blocks[5], ff->blocks[6],
                ff->data_start, ff->len);
            for (ci = 0; ci < 3; ci++) {
                uint32_t off = (namep & 0x1FFFFFFFu) - 1u;
                size_t tgt = (size_t)cand[ci] + off;
                if ((namep >> 29) < 7 && tgt + 80 < ff->len) {
                    char s[81];
                    memcpy(s, ff->data + tgt, 80);
                    s[80] = 0;
                    fprintf(stderr, "[dump] base=%u blk=%u off=%u -> \"%s\"\n",
                            cand[ci], namep >> 29, off, s);
                }
            }
        }
        if (!sname) {
            snprintf(err, errcap, "world: asset %u: tipo %u sem modelo", i, t);
            return -1;
        }
        if (t4z_load_alloc(&w, sname) != 0) {
            if (w.g_vtx && w.g_idx) {
                /* GfxWorld ja extraido; erro em asset posterior = aviso */
                snprintf(wv->walk_err, sizeof(wv->walk_err),
                         "asset %u (%s): %s", i, sname, t4z_error(&w));
                have_world = 1;
                break;
            }
            snprintf(err, errcap, "world: asset %u (%s): %s", i, sname,
                     t4z_error(&w));
            return -1;
        }
        if (w.g_vtx && w.g_idx)
            have_world = 1;
    }
    if (!have_world) {
        snprintf(err, errcap, "world: nenhum GfxWorld neste .ff");
        return -1;
    }
    wv->walk_exact = (w.pos == w.len) ? 1 : 0;

    /* offsets dos campos (layout MSVC 32 bits via zonetables) */
    off_xyz = t4z_field_off("GfxWorldVertex", "xyz");
    off_nrm = t4z_field_off("GfxWorldVertex", "normal");
    off_vc = t4z_field_off("GfxWorld", "vertexCount");
    off_mins = t4z_field_off("GfxWorld", "mins");
    off_maxs = t4z_field_off("GfxWorld", "maxs");
    off_surfs = t4z_field_off("GfxWorld", "surfaceCount");
    {
        const T4ZStruct *vs = t4z_struct("GfxWorldVertex");
        vsz = vs ? vs->size : 44;
    }
    if (off_xyz < 0 || off_vc < 0 || off_mins < 0 || off_maxs < 0) {
        snprintf(err, errcap, "world: layout GfxWorld inesperado");
        return -1;
    }

    nverts = (uint32_t)w.g_nvtx;
    nidx = (uint32_t)w.g_idxn;
    if (nverts == 0 || nverts > 8000000u || nidx == 0 || nidx > 24000000u) {
        snprintf(err, errcap, "world: contagens impossiveis (verts=%u idx=%u)",
                 nverts, nidx);
        return -1;
    }

    /* limites do mundo: lidos do blob do GfxWorld (mins/maxs float[3]) */
    if (w.g_blob) {
        for (i = 0; i < 3; i++) {
            wv->mins[i] = rdcf(w.g_blob + off_mins + 4 * i);
            wv->maxs[i] = rdcf(w.g_blob + off_maxs + 4 * i);
        }
        if (off_surfs >= 0)
            wv->surfaces = (int)rd32c(w.g_blob + off_surfs);
    }
    wv->nverts = nverts;
    wv->nidx = nidx;

    wv->pos = (float *)malloc((size_t)nverts * 3 * sizeof(float));
    wv->nrm = (float *)malloc((size_t)nverts * 3 * sizeof(float));
    wv->idx = (uint16_t *)malloc((size_t)nidx * sizeof(uint16_t));
    if (!wv->pos || !wv->nrm || !wv->idx) {
        snprintf(err, errcap, "world: sem memoria (%u verts)", nverts);
        t4_world_free(wv);
        return -1;
    }

    /* vertices: posicao + normal empacotada */
    for (v = 0; v < nverts; v++) {
        const uint8_t *vp = w.g_vtx + (size_t)vsz * v;
        wv->pos[3 * v + 0] = rdcf(vp + off_xyz + 0);
        wv->pos[3 * v + 1] = rdcf(vp + off_xyz + 4);
        wv->pos[3 * v + 2] = rdcf(vp + off_xyz + 8);
        if (off_nrm >= 0)
            unpack_nrm(rd32c(vp + off_nrm), &wv->nrm[3 * v]);
        else {
            wv->nrm[3 * v + 0] = 0.0f;
            wv->nrm[3 * v + 1] = 0.0f;
            wv->nrm[3 * v + 2] = 1.0f;
        }
    }

    /* validacao: vertices dentro de [-1e9, 1e9] e (se disponivel) dentro
     * da envoltoria do blob com folga de 1 unidade */
    {
        int have_blob_bounds = (w.g_blob && wv->mins[0] < 1e29f) ? 1 : 0;
        for (v = 0; v < nverts; v++) {
            int k;
            for (k = 0; k < 3; k++) {
                float x = wv->pos[3 * v + k];
                if (!(x > -1e9f && x < 1e9f)) { bad_pos++; continue; }
                if (have_blob_bounds &&
                    (x < wv->mins[k] - 1.0f || x > wv->maxs[k] + 1.0f))
                    bad_pos++;
            }
        }
        if (!have_blob_bounds) {
            for (i = 0; i < 3; i++) { wv->mins[i] = 1e30f; wv->maxs[i] = -1e30f; }
            for (v = 0; v < nverts; v++) {
                int k;
                for (k = 0; k < 3; k++) {
                    float x = wv->pos[3 * v + k];
                    if (x < wv->mins[k]) wv->mins[k] = x;
                    if (x > wv->maxs[k]) wv->maxs[k] = x;
                }
            }
        }
    }
    memcpy(wv->idx, w.g_idx, (size_t)nidx * sizeof(uint16_t));
    for (i = 0; i < nidx; i++)
        if (wv->idx[i] >= nverts) bad_idx++;

    if (bad_pos > (long)(nverts / 100) || bad_idx > (long)(nidx / 100)) {
        snprintf(err, errcap,
                 "world: dados invalidos (pos=%ld idx=%ld) - walker fora do GfxWorld",
                 bad_pos, bad_idx);
        t4_world_free(wv);
        return -1;
    }

    if (w.g_name[0]) {
        snprintf(wv->name, sizeof(wv->name), "%s", w.g_name);
    } else {
        snprintf(wv->name, sizeof(wv->name), "(sem nome)");
    }
    return 0;
}

void t4_world_free(T4World *wv)
{
    if (!wv) return;
    free(wv->pos);
    free(wv->nrm);
    free(wv->idx);
    memset(wv, 0, sizeof(*wv));
}
