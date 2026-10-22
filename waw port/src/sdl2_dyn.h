/*
 * sdl2_dyn.h - binding dinamico minimo para SDL2 via dlopen().
 *
 * O ambiente de build alvo pode nao ter os headers/libs de desenvolvimento da
 * SDL2 (so a .so runtime). Em vez de exigir libsdl2-dev, carregamos a
 * biblioteca em tempo de execucao com dlopen e declaramos apenas os simbolos
 * que usamos. Isso mantem o binario portavel (Linux/macOS/Windows) sem
 * dependencia de headers de terceiros.
 */
#ifndef T4_SDL2_DYN_H
#define T4_SDL2_DYN_H

#include <stdint.h>

typedef struct T4_SDL_Window    T4_SDL_Window;
typedef struct T4_SDL_Renderer  T4_SDL_Renderer;
typedef struct T4_SDL_Texture   T4_SDL_Texture;

typedef struct { int x, y, w, h; } T4_SDL_Rect;

/* SDL_Event e uma union de 56 bytes no SDL2; so lemos o campo .type. */
typedef union { uint32_t type; unsigned char raw[56]; } T4_SDLEvent;

#define T4_SDL_INIT_VIDEO            0x00000020u
#define T4_SDL_QUIT                  0x100u
#define T4_SDL_KEYDOWN               0x300u
#define T4_SDL_WINDOWPOS_CENTERED    0x2FFF0000u
#define T4_SDL_PIXELFORMAT_ARGB8888  0x16362004u
#define T4_SDL_TEXTUREACCESS_STATIC  0
#define T4_SDL_TEXTUREACCESS_STREAMING 1
#define T4_SDLK_ESCAPE               27

/* flags de janela e atributos de contexto OpenGL */
#define T4_SDL_WINDOW_OPENGL         0x00000002u
#define T4_SDL_WINDOW_VULKAN         0x10000000u
#define T4_SDL_GL_DOUBLEBUFFER               5
#define T4_SDL_GL_CONTEXT_MAJOR_VERSION      17
#define T4_SDL_GL_CONTEXT_MINOR_VERSION      18
#define T4_SDL_GL_CONTEXT_FLAGS              20
#define T4_SDL_GL_CONTEXT_PROFILE_MASK       21
#define T4_SDL_GL_CONTEXT_PROFILE_CORE       0x0001
#define T4_SDL_GL_CONTEXT_PROFILE_COMPATIBILITY 0x0002
#define T4_SDL_GL_CONTEXT_PROFILE_ES         0x0004

/* ponteiros de funcao resolvidos */
extern int   (*t4_SDL_Init)(uint32_t flags);
extern void  (*t4_SDL_Quit)(void);
extern const char *(*t4_SDL_GetError)(void);
extern int   (*t4_SDL_GL_SetAttribute)(int attr, int value);

extern T4_SDL_Window *(*t4_SDL_CreateWindow)(const char *title, int x, int y, int w, int h, uint32_t flags);
extern void (*t4_SDL_DestroyWindow)(T4_SDL_Window *);
extern void (*t4_SDL_SetWindowTitle)(T4_SDL_Window *, const char *);

extern T4_SDL_Renderer *(*t4_SDL_CreateRenderer)(T4_SDL_Window *, int index, uint32_t flags);
extern void (*t4_SDL_DestroyRenderer)(T4_SDL_Renderer *);
extern int  (*t4_SDL_RenderClear)(T4_SDL_Renderer *);
extern int  (*t4_SDL_RenderCopy)(T4_SDL_Renderer *, T4_SDL_Texture *, const T4_SDL_Rect *, const T4_SDL_Rect *);
extern void (*t4_SDL_RenderPresent)(T4_SDL_Renderer *);
extern int  (*t4_SDL_SetRenderDrawColor)(T4_SDL_Renderer *, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

extern T4_SDL_Texture *(*t4_SDL_CreateTexture)(T4_SDL_Renderer *, uint32_t format, int access, int w, int h);
extern void (*t4_SDL_DestroyTexture)(T4_SDL_Texture *);
extern int  (*t4_SDL_UpdateTexture)(T4_SDL_Texture *, const T4_SDL_Rect *, const void *pixels, int pitch);

extern int  (*t4_SDL_PollEvent)(T4_SDLEvent *);
extern void (*t4_SDL_Delay)(uint32_t ms);
extern uint32_t (*t4_SDL_GetTicks)(void);

/* contexto OpenGL (obrigatorios: fazem parte da SDL2 desde sempre) */
typedef struct T4_SDL_GLContext_T *T4_SDL_GLContext;
extern T4_SDL_GLContext (*t4_SDL_GL_CreateContext)(T4_SDL_Window *);
extern void  (*t4_SDL_GL_DeleteContext)(T4_SDL_GLContext);
extern int   (*t4_SDL_GL_MakeCurrent)(T4_SDL_Window *, T4_SDL_GLContext);
extern void *(*t4_SDL_GL_GetProcAddress)(const char *);
extern int   (*t4_SDL_GL_SetSwapInterval)(int);
extern void  (*t4_SDL_GL_SwapWindow)(T4_SDL_Window *);
extern void  (*t4_SDL_GL_GetDrawableSize)(T4_SDL_Window *, int *, int *);

/* Vulkan via SDL (opcional: desde SDL 2.0.6; ponteiro fica NULL se ausente) */
extern int (*t4_SDL_Vulkan_LoadLibrary)(const char *);
extern int (*t4_SDL_Vulkan_GetInstanceExtensions)(T4_SDL_Window *, unsigned *, const char **);
extern int (*t4_SDL_Vulkan_CreateSurface)(T4_SDL_Window *, void *, uint64_t *);
extern void (*t4_SDL_Vulkan_GetDrawableSize)(T4_SDL_Window *, int *, int *);

/* modos de video (SDL2 >= 2.0.1; NULL se a SDL nao exportar): TODAS as
 * resolucoes que o monitor/GPU anuncia; qualquer WxH vale no t4port.cfg */
typedef struct {
    uint32_t format;       /* SDL_PixelFormat do modo */
    int      w, h;         /* resolucao em pixels */
    int      refresh_rate; /* Hz (informativo; r_mode guarda so WxH) */
    void    *driverdata;
} T4_SDL_DisplayMode;

extern int (*t4_SDL_GetNumVideoDisplays)(void);
extern int (*t4_SDL_GetNumDisplayModes)(int displayIndex);
extern int (*t4_SDL_GetDisplayMode)(int displayIndex, int modeIndex,
                                    T4_SDL_DisplayMode *mode);
extern int (*t4_SDL_GetCurrentDisplayMode)(int displayIndex,
                                           T4_SDL_DisplayMode *mode);

/*
 * Carrega libSDL2 com dlopen. Retorna 0 em sucesso; em caso de falha preenche
 * a mensagem interna (t4_sdl_error()) com a causa exata.
 */
int         t4_sdl_load(void);
void        t4_sdl_unload(void);
int         t4_sdl_available(void);
const char *t4_sdl_error(void);

#endif /* T4_SDL2_DYN_H */
