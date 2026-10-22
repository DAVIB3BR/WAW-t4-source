/*
 * sdl2_dyn.c - resolucao dos simbolos da SDL2 em tempo de execucao.
 */
#include "sdl2_dyn.h"

#if defined(_WIN32)
#include <windows.h>
/* Adaptador no estilo dlopen() para carregar SDL2.dll no Windows. */
static void *t4_dlopen(const char *name) { return (void *)LoadLibraryA(name); }
static void *t4_dlsym(void *lib, const char *name)
{
    return (void *)GetProcAddress((HMODULE)lib, name);
}
static void t4_dlclose(void *lib) { if (lib) FreeLibrary((HMODULE)lib); }
static const char *t4_dlerror(void) { return "SDL2.dll nao encontrada"; }
#else
#include <dlfcn.h>
#define t4_dlopen(n)       dlopen((n), RTLD_NOW | RTLD_LOCAL)
#define t4_dlsym(l, s)     dlsym((l), (s))
#define t4_dlclose(l)      dlclose(l)
#define t4_dlerror()       dlerror()
#endif
#include <stdio.h>
#include <string.h>

/*
 * Modo linkado: quando os headers da SDL2 estao disponiveis (Flatpak, brew,
 * libsdl2-dev) ligamos direto e nao usamos dlopen. Os adaptadores preservam as
 * assinaturas minimas declaradas em sdl2_dyn.h.
 */
#ifdef T4PORT_SDL2_LINKED
#include <SDL2/SDL.h>
#endif

int   (*t4_SDL_Init)(uint32_t) = NULL;
void  (*t4_SDL_Quit)(void) = NULL;
const char *(*t4_SDL_GetError)(void) = NULL;
int   (*t4_SDL_GL_SetAttribute)(int, int) = NULL;

T4_SDL_Window *(*t4_SDL_CreateWindow)(const char *, int, int, int, int, uint32_t) = NULL;
void (*t4_SDL_DestroyWindow)(T4_SDL_Window *) = NULL;
void (*t4_SDL_SetWindowTitle)(T4_SDL_Window *, const char *) = NULL;

T4_SDL_Renderer *(*t4_SDL_CreateRenderer)(T4_SDL_Window *, int, uint32_t) = NULL;
void (*t4_SDL_DestroyRenderer)(T4_SDL_Renderer *) = NULL;
int  (*t4_SDL_RenderClear)(T4_SDL_Renderer *) = NULL;
int  (*t4_SDL_RenderCopy)(T4_SDL_Renderer *, T4_SDL_Texture *, const T4_SDL_Rect *, const T4_SDL_Rect *) = NULL;
void (*t4_SDL_RenderPresent)(T4_SDL_Renderer *) = NULL;
int  (*t4_SDL_SetRenderDrawColor)(T4_SDL_Renderer *, uint8_t, uint8_t, uint8_t, uint8_t) = NULL;

T4_SDL_Texture *(*t4_SDL_CreateTexture)(T4_SDL_Renderer *, uint32_t, int, int, int) = NULL;
void (*t4_SDL_DestroyTexture)(T4_SDL_Texture *) = NULL;
int  (*t4_SDL_UpdateTexture)(T4_SDL_Texture *, const T4_SDL_Rect *, const void *, int) = NULL;

int      (*t4_SDL_PollEvent)(T4_SDLEvent *) = NULL;
void     (*t4_SDL_Delay)(uint32_t) = NULL;
uint32_t (*t4_SDL_GetTicks)(void) = NULL;

static void *g_lib = NULL;
static char g_err[512];

const char *t4_sdl_error(void) { return g_err; }
int t4_sdl_available(void) { return g_lib != NULL; }

#ifndef T4PORT_SDL2_LINKED
static void *sym(void *lib, const char *name)
{
    void *p = t4_dlsym(lib, name);
    if (!p) {
        size_t n = strlen(g_err);
        snprintf(g_err + n, sizeof(g_err) - n, "%s%s", n ? ", " : "", name);
    }
    return p;
}
#endif

#ifdef T4PORT_SDL2_LINKED
/* adaptadores: assinaturas minimas de sdl2_dyn.h -> API real da SDL2 */
static int a_SDL_Init(uint32_t f) { return SDL_Init(f); }
static void a_SDL_Quit(void) { SDL_Quit(); }
static const char *a_SDL_GetError(void) { return SDL_GetError(); }
static int a_SDL_GL_SetAttribute(int a, int v) { return SDL_GL_SetAttribute(a, v); }
static T4_SDL_Window *a_SDL_CreateWindow(const char *t, int x, int y, int w, int h, uint32_t f)
{ return (T4_SDL_Window *)SDL_CreateWindow(t, x, y, w, h, f); }
static void a_SDL_DestroyWindow(T4_SDL_Window *w) { SDL_DestroyWindow((SDL_Window *)w); }
static void a_SDL_SetWindowTitle(T4_SDL_Window *w, const char *t) { SDL_SetWindowTitle((SDL_Window *)w, t); }
static T4_SDL_Renderer *a_SDL_CreateRenderer(T4_SDL_Window *w, int i, uint32_t f)
{ return (T4_SDL_Renderer *)SDL_CreateRenderer((SDL_Window *)w, i, f); }
static void a_SDL_DestroyRenderer(T4_SDL_Renderer *r) { SDL_DestroyRenderer((SDL_Renderer *)r); }
static int a_SDL_RenderClear(T4_SDL_Renderer *r) { return SDL_RenderClear((SDL_Renderer *)r); }
static int a_SDL_RenderCopy(T4_SDL_Renderer *r, T4_SDL_Texture *t, const T4_SDL_Rect *src, const T4_SDL_Rect *dst)
{ return SDL_RenderCopy((SDL_Renderer *)r, (SDL_Texture *)t, (const SDL_Rect *)src, (const SDL_Rect *)dst); }
static void a_SDL_RenderPresent(T4_SDL_Renderer *r) { SDL_RenderPresent((SDL_Renderer *)r); }
static int a_SDL_SetRenderDrawColor(T4_SDL_Renderer *r, uint8_t g0, uint8_t g1, uint8_t g2, uint8_t g3)
{ return SDL_SetRenderDrawColor((SDL_Renderer *)r, g0, g1, g2, g3); }
static T4_SDL_Texture *a_SDL_CreateTexture(T4_SDL_Renderer *r, uint32_t fmt, int acc, int w, int h)
{ return (T4_SDL_Texture *)SDL_CreateTexture((SDL_Renderer *)r, fmt, acc, w, h); }
static void a_SDL_DestroyTexture(T4_SDL_Texture *t) { SDL_DestroyTexture((SDL_Texture *)t); }
static int a_SDL_UpdateTexture(T4_SDL_Texture *t, const T4_SDL_Rect *rc, const void *px, int pitch)
{ return SDL_UpdateTexture((SDL_Texture *)t, (const SDL_Rect *)rc, px, pitch); }
static int a_SDL_PollEvent(T4_SDLEvent *e) { return SDL_PollEvent((SDL_Event *)e); }
static void a_SDL_Delay(uint32_t ms) { SDL_Delay(ms); }
static uint32_t a_SDL_GetTicks(void) { return SDL_GetTicks(); }
#endif

int t4_sdl_load(void)
{
#ifdef T4PORT_SDL2_LINKED
    if (g_lib) return 0;
    t4_SDL_Init            = a_SDL_Init;
    t4_SDL_Quit            = a_SDL_Quit;
    t4_SDL_GetError        = a_SDL_GetError;
    t4_SDL_GL_SetAttribute = a_SDL_GL_SetAttribute;
    t4_SDL_CreateWindow    = a_SDL_CreateWindow;
    t4_SDL_DestroyWindow   = a_SDL_DestroyWindow;
    t4_SDL_SetWindowTitle  = a_SDL_SetWindowTitle;
    t4_SDL_CreateRenderer  = a_SDL_CreateRenderer;
    t4_SDL_DestroyRenderer = a_SDL_DestroyRenderer;
    t4_SDL_RenderClear      = a_SDL_RenderClear;
    t4_SDL_RenderCopy       = a_SDL_RenderCopy;
    t4_SDL_RenderPresent    = a_SDL_RenderPresent;
    t4_SDL_SetRenderDrawColor = a_SDL_SetRenderDrawColor;
    t4_SDL_CreateTexture    = a_SDL_CreateTexture;
    t4_SDL_DestroyTexture   = a_SDL_DestroyTexture;
    t4_SDL_UpdateTexture    = a_SDL_UpdateTexture;
    t4_SDL_PollEvent        = a_SDL_PollEvent;
    t4_SDL_Delay            = a_SDL_Delay;
    t4_SDL_GetTicks         = a_SDL_GetTicks;
    g_lib = (void *)1; /* marcador: linkado */
    g_err[0] = '\0';
    return 0;
#else
    static const char *candidates[] = {
#if defined(__APPLE__)
        "libSDL2-2.0.dylib", "libSDL2.dylib",
#elif defined(_WIN32)
        "SDL2.dll",
#else
        "libSDL2-2.0.so.0", "libSDL2.so", "libSDL2-2.0.so",
#endif
        NULL
    };
    int i;
    const char *tried = "";

    g_err[0] = '\0';
    if (g_lib) return 0;

    for (i = 0; candidates[i]; i++) {
        g_lib = t4_dlopen(candidates[i]);
        if (g_lib) break;
        tried = candidates[i];
    }
    if (!g_lib) {
        snprintf(g_err, sizeof(g_err), "SDL2 nao encontrada (ultima tentativa %s): %s",
                 tried, t4_dlerror());
        return -1;
    }

    t4_SDL_Init = (int (*)(uint32_t))sym(g_lib, "SDL_Init");
    t4_SDL_Quit = (void (*)(void))sym(g_lib, "SDL_Quit");
    t4_SDL_GetError = (const char *(*)(void))sym(g_lib, "SDL_GetError");
    t4_SDL_GL_SetAttribute = (int (*)(int, int))sym(g_lib, "SDL_GL_SetAttribute");

    t4_SDL_CreateWindow = (T4_SDL_Window *(*)(const char *, int, int, int, int, uint32_t))sym(g_lib, "SDL_CreateWindow");
    t4_SDL_DestroyWindow = (void (*)(T4_SDL_Window *))sym(g_lib, "SDL_DestroyWindow");
    t4_SDL_SetWindowTitle = (void (*)(T4_SDL_Window *, const char *))sym(g_lib, "SDL_SetWindowTitle");

    t4_SDL_CreateRenderer = (T4_SDL_Renderer *(*)(T4_SDL_Window *, int, uint32_t))sym(g_lib, "SDL_CreateRenderer");
    t4_SDL_DestroyRenderer = (void (*)(T4_SDL_Renderer *))sym(g_lib, "SDL_DestroyRenderer");
    t4_SDL_RenderClear = (int (*)(T4_SDL_Renderer *))sym(g_lib, "SDL_RenderClear");
    t4_SDL_RenderCopy = (int (*)(T4_SDL_Renderer *, T4_SDL_Texture *, const T4_SDL_Rect *, const T4_SDL_Rect *))sym(g_lib, "SDL_RenderCopy");
    t4_SDL_RenderPresent = (void (*)(T4_SDL_Renderer *))sym(g_lib, "SDL_RenderPresent");
    t4_SDL_SetRenderDrawColor = (int (*)(T4_SDL_Renderer *, uint8_t, uint8_t, uint8_t, uint8_t))sym(g_lib, "SDL_SetRenderDrawColor");

    t4_SDL_CreateTexture = (T4_SDL_Texture *(*)(T4_SDL_Renderer *, uint32_t, int, int, int))sym(g_lib, "SDL_CreateTexture");
    t4_SDL_DestroyTexture = (void (*)(T4_SDL_Texture *))sym(g_lib, "SDL_DestroyTexture");
    t4_SDL_UpdateTexture = (int (*)(T4_SDL_Texture *, const T4_SDL_Rect *, const void *, int))sym(g_lib, "SDL_UpdateTexture");

    t4_SDL_PollEvent = (int (*)(T4_SDLEvent *))sym(g_lib, "SDL_PollEvent");
    t4_SDL_Delay = (void (*)(uint32_t))sym(g_lib, "SDL_Delay");
    t4_SDL_GetTicks = (uint32_t (*)(void))sym(g_lib, "SDL_GetTicks");

    if (g_err[0]) {
        snprintf(g_err, sizeof(g_err), "SDL2 carregada mas simbolos ausentes: %s", g_err);
        t4_dlclose(g_lib);
        g_lib = NULL;
        return -1;
    }
    return 0;
#endif
}

void t4_sdl_unload(void)
{
#ifdef T4PORT_SDL2_LINKED
    g_lib = NULL;
#else
    if (g_lib && g_lib != (void *)1) t4_dlclose(g_lib);
    g_lib = NULL;
#endif
}
