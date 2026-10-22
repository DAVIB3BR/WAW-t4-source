/*
 * gfx.c - selecao/deteccao da API grafica + configuracao persistente.
 *
 * Configuracao em <config_dir>/t4port.cfg (chave r_api=auto|opengl|vulkan|
 * opengles). A pasta de configuracao segue o padrao de cada SO:
 *   Linux/FreeBSD : $XDG_CONFIG_HOME/t4port ou ~/.config/t4port
 *   macOS         : ~/Library/Application Support/t4port
 *   Windows       : %APPDATA%\t4port
 */
#include "gfx.h"
#include "sdl2_dyn.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#if defined(_WIN32)
#include <direct.h>
#define t4_mkdir(p) _mkdir(p)
#else
#define t4_mkdir(p) mkdir((p), 0755)
#endif

static char g_err[512];

const char *t4_gfx_error(void) { return g_err; }

const char *t4_gfx_api_name(T4GfxApi a)
{
    switch (a) {
    case T4_GFX_OPENGL: return "opengl";
    case T4_GFX_VULKAN: return "vulkan";
    case T4_GFX_ES:     return "opengles";
    default:            return "auto";
    }
}

int t4_gfx_api_from_name(const char *s, T4GfxApi *out)
{
    if (!s || !out) return -1;
    if (!strcmp(s, "auto"))     { *out = T4_GFX_AUTO;    return 0; }
    if (!strcmp(s, "opengl"))   { *out = T4_GFX_OPENGL;  return 0; }
    if (!strcmp(s, "vulkan"))   { *out = T4_GFX_VULKAN;  return 0; }
    if (!strcmp(s, "opengles") || !strcmp(s, "gles")) {
        *out = T4_GFX_ES; return 0;
    }
    return -1;
}

/* ----------------------------------------------------------- configuracao */

const char *t4_gfx_config_path(char *buf, size_t cap)
{
#if defined(_WIN32)
    const char *base = getenv("APPDATA");
    if (!base || !*base) base = ".";
    snprintf(buf, cap, "%s\\t4port\\t4port.cfg", base);
#elif defined(__APPLE__)
    const char *home = getenv("HOME");
    if (!home || !*home) home = ".";
    snprintf(buf, cap,
             "%s/Library/Application Support/t4port/t4port.cfg", home);
#else
    const char *base = getenv("XDG_CONFIG_HOME");
    char fallback[1024];
    if (!base || !*base) {
        const char *home = getenv("HOME");
        if (!home || !*home) home = ".";
        snprintf(fallback, sizeof(fallback), "%s/.config", home);
        base = fallback;
    }
    snprintf(buf, cap, "%s/t4port/t4port.cfg", base);
#endif
    return buf;
}

/* cria a pasta pai do arquivo de configuracao (mkdir -p de1 nivel) */
static void ensure_parent_dir(const char *file)
{
    char dir[1100];
    char *slash;
    size_t n = strlen(file);
    if (n >= sizeof(dir)) return;
    memcpy(dir, file, n + 1);
#ifdef _WIN32
    slash = strrchr(dir, '\\');
#else
    slash = strrchr(dir, '/');
#endif
    if (!slash) return;
    *slash = '\0';
    t4_mkdir(dir);          /* Linux/macOS: ignora erro se ja existe */
#ifdef __APPLE__
    /* macOS: ".../Library/Application Support/t4port" tambem e novo */
#endif
}

T4GfxApi t4_gfx_config_get(void)
{
    char path[1100], line[256];
    FILE *fp;
    T4GfxApi result = T4_GFX_AUTO;

    t4_gfx_config_path(path, sizeof(path));
    fp = fopen(path, "r");
    if (!fp) return result;
    while (fgets(line, sizeof(line), fp)) {
        char *eq, *key, *val, *nl;
        if (line[0] == '#' || line[0] == '\n') continue;
        nl = strchr(line, '\n');
        if (nl) *nl = '\0';
        eq = strchr(line, '=');
        if (!eq) continue;
        *eq = '\0';
        key = line;
        val = eq + 1;
        if (!strcmp(key, "r_api")) {
            T4GfxApi a;
            if (t4_gfx_api_from_name(val, &a) == 0) result = a;
        }
    }
    fclose(fp);
    return result;
}

/* le qualquer chave string do t4port.cfg (ex.: r_mode); 1 se achou */
int t4_gfx_config_str(const char *key, char *out, size_t cap)
{
    char path[1100], line[256];
    FILE *fp;
    int found = 0;

    out[0] = '\0';
    t4_gfx_config_path(path, sizeof(path));
    fp = fopen(path, "r");
    if (!fp) return 0;
    while (fgets(line, sizeof(line), fp)) {
        char *eq, *nl;
        if (line[0] == '#' || line[0] == '\n') continue;
        nl = strchr(line, '\n');
        if (nl) *nl = '\0';
        eq = strchr(line, '=');
        if (!eq) continue;
        *eq = '\0';
        if (!strcmp(line, key)) {
            snprintf(out, cap, "%s", eq + 1);
            found = 1;
            break;
        }
    }
    fclose(fp);
    return found;
}

/*
 * Janela do motor: r_mode do cfg aceita QUALQUER WxH (o menu enumera os
 * modos do monitor + presets; valor digitado a mao tambem vale).
 * Default 720p.
 */
void t4_gfx_window_size(int *w, int *h)
{
    char mode[64];
    int ww = 0, hh = 0;

    *w = 1280;
    *h = 720;
    if (t4_gfx_config_str("r_mode", mode, sizeof(mode)) &&
        sscanf(mode, "%dx%d", &ww, &hh) == 2 &&
        ww >= 320 && hh >= 240 && ww <= 16384 && hh <= 16384) {
        *w = ww;
        *h = hh;
    }
}

int t4_gfx_config_set(T4GfxApi a)
{
    char path[1100], line[256], others[4096];
    FILE *fp;
    size_t on = 0;

    /* preserva as demais chaves do arquivo (r_mode, r_fullscreen,
     * r_vsync - gravadas pelo `t4port menu`); so r_api e reescrito. */
    others[0] = '\0';
    t4_gfx_config_path(path, sizeof(path));
    fp = fopen(path, "r");
    if (fp) {
        while (fgets(line, sizeof(line), fp)) {
            size_t len;
            if (strncmp(line, "r_api=", 6) == 0) continue;
            len = strlen(line);
            if (on + len < sizeof(others) - 1) {
                memcpy(others + on, line, len);
                on += len;
                others[on] = '\0';
            }
        }
        fclose(fp);
    }

    ensure_parent_dir(path);
    fp = fopen(path, "w");
    if (!fp) {
        snprintf(g_err, sizeof(g_err), "could not write %s", path);
        return -1;
    }
    fprintf(fp,
            "# t4port - configuracao grafica\n"
            "# r_api = auto | opengl | vulkan | opengles\n"
            "# r_mode = WxH | r_fullscreen = 0|1 | r_vsync = 0|1\n"
            "r_api=%s\n",
            t4_gfx_api_name(a));
    fputs(others, fp);
    fclose(fp);
    return 0;
}

/* ------------------------------------------------------------- deteccao */

int t4_gfx_detect(char lines[][128], int max)
{
    int n = 0;
    char info[96];

    if (n < max) {
        if (t4_sdl_load() == 0) {
            snprintf(lines[n], 128, "SDL2            OK (window + surfaces)");
            t4_sdl_unload();
        } else {
            snprintf(lines[n], 128, "SDL2            MISSING: %s",
                     t4_sdl_error());
        }
        n++;
    }

    if (n < max) {
        if (t4_gfx_gl_lib_check(info, sizeof(info)) == 0)
            snprintf(lines[n], 128, "OpenGL          %s", info);
        else
            snprintf(lines[n], 128, "OpenGL          %s", info);
        n++;
    }

    if (n < max) {
        t4_gfx_vk_loader_check(info, sizeof(info));
        snprintf(lines[n], 128, "Vulkan          %s", info);
        n++;
    }

    if (n < max) {
        snprintf(lines[n], 128,
                 "current choice  %s (%s)", t4_gfx_api_name(t4_gfx_config_get()),
                 "t4port api <auto|opengl|vulkan|opengles> to change");
        n++;
    }
    return n;
}

/* --------------------------------------------------------------- execucao */

int t4_gfx_run(T4GfxApi api, int w, int h, int seconds)
{
    char info[256];
    int rc = -1;

    g_err[0] = '\0';

    if (t4_sdl_load() != 0) {
        snprintf(g_err, sizeof(g_err), "SDL2 unavailable: %s", t4_sdl_error());
        return -1;
    }
    if (t4_SDL_Init(T4_SDL_INIT_VIDEO) != 0) {
        snprintf(g_err, sizeof(g_err), "SDL_Init(VIDEO): %s", t4_SDL_GetError());
        t4_sdl_unload();
        return -1;
    }

    if (api == T4_GFX_OPENGL || api == T4_GFX_AUTO) {
        rc = t4_gfx_gl_run(w, h, seconds, 0, info, sizeof(info));
        if (rc == 0) printf("API grafica ativa: %s\n", info);
        else snprintf(g_err, sizeof(g_err), "%s", info);
    }
    if (rc != 0 && (api == T4_GFX_VULKAN || api == T4_GFX_AUTO)) {
        char prev[512];
        snprintf(prev, sizeof(prev), "%s", g_err);
        rc = t4_gfx_vk_run(w, h, seconds, info, sizeof(info));
        if (rc == 0) printf("API grafica ativa: %s\n", info);
        else if (prev[0])
            snprintf(g_err, sizeof(g_err), "OpenGL: %s | Vulkan: %s", prev, info);
        else
            snprintf(g_err, sizeof(g_err), "%s", info);
    }
    if (rc != 0 && (api == T4_GFX_ES || api == T4_GFX_AUTO)) {
        char prev[512];
        snprintf(prev, sizeof(prev), "%s", g_err);
        rc = t4_gfx_gl_run(w, h, seconds, 1, info, sizeof(info));
        if (rc == 0) printf("API grafica ativa: %s\n", info);
        else if (prev[0])
            snprintf(g_err, sizeof(g_err), "%s | OpenGL ES: %s", prev, info);
        else
            snprintf(g_err, sizeof(g_err), "%s", info);
    }

    t4_SDL_Quit();
    t4_sdl_unload();
    return rc;
}
