/*
 * gfx.h - camada de API grafica do porte (T4 / IW 3.0).
 *
 * O usuario escolhe a API grafica em opcoes de video; por padrao "auto"
 * prova na ordem OpenGL -> Vulkan -> OpenGL ES. Backends:
 *   gfx_gl.c  : OpenGL desktop 2.1..4.5 e OpenGL ES 3.x (via SDL2)
 *   gfx_vk.c  : Vulkan 1.x (loader + driver ICD, superficie via SDL2)
 * Todas as bibliotecas sao carregadas em tempo de execucao (dlopen), entao o
 * mesmo binario roda em qualquer distribuicao sem depender de headers.
 */
#ifndef T4PORT_GFX_H
#define T4PORT_GFX_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    T4_GFX_AUTO = 0,
    T4_GFX_OPENGL,
    T4_GFX_VULKAN,
    T4_GFX_ES
} T4GfxApi;

/* nomes amigaveis: "auto", "opengl", "vulkan", "opengles" */
const char *t4_gfx_api_name(T4GfxApi a);
int         t4_gfx_api_from_name(const char *s, T4GfxApi *out); /* 0 ok */

/* configuracao persistente (arquivo t4port.cfg) */
T4GfxApi    t4_gfx_config_get(void);
int         t4_gfx_config_set(T4GfxApi a);            /* 0 ok */
const char *t4_gfx_config_path(char *buf, size_t cap);
int         t4_gfx_config_str(const char *key, char *out, size_t cap);

/* tamanho da janela do motor: r_mode do cfg (qualquer WxH); default 720p */
void        t4_gfx_window_size(int *w, int *h);

/* diagnostico das APIs disponiveis no sistema; retorna n. de linhas */
int  t4_gfx_detect(char lines[][128], int max);

/*
 * Abre janela com a API pedida e renderiza a animacao de teste ate o usuario
 * fechar (ESC/quit) ou ate `seconds` (>0 = saida automatica, p/ testes).
 * Retorna 0 em sucesso; mensagem em t4_gfx_error().
 */
int         t4_gfx_run(T4GfxApi api, int w, int h, int seconds);
const char *t4_gfx_error(void);

/* backends (usados por gfx.c; nao chamar direto da CLI) */
int t4_gfx_gl_run(int w, int h, int seconds, int es_mode,
                  char *info, size_t info_cap);
int t4_gfx_gl_lib_check(char *info, size_t cap);   /* 0 = lib GL encontrada */
/* Renderiza um GfxWorld (vertices + indices) com orbita de camera;
 * 0 = ok. shot_png (opcional) grava screenshot da primeira imagem. */
int t4_gfx_gl_world_run(int w, int h, int seconds, int es_mode,
                        const float *pos, const float *nrm, size_t nverts,
                        const uint16_t *idx, size_t nidx,
                        const float mins[3], const float maxs[3],
                        const char *shot_png, int wire,
                        char *info, size_t info_cap);
int t4_gfx_vk_run(int w, int h, int seconds,
                  char *info, size_t info_cap);
int t4_gfx_vk_loader_check(char *info, size_t cap); /* 0 = loader+driver ok */

#endif /* T4PORT_GFX_H */
