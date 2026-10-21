/* menu.c - menu de opcoes de video do t4port.
 *
 * Desenha tudo em CPU para uma unica textura RGBA (binding SDL2 ja existente)
 * e copia para a janela - nao precisa de OpenGL, funciona ate em maquina sem
 * driver grafico. Fonte bitmap 5x7 embutida (ASCII, sem acentos).
 *
 * Itens:
 *   API GRAFICA   -> r_api no t4port.cfg (vale para gfx/world/check: e a
 *                    escolha que troca de backend de verdade)
 *   RESOLUCAO     -> r_mode      (usada pela janela do menu em tela cheia)
 *   TELA CHEIA    -> r_fullscreen (aplicado na proxima abertura)
 *   VSYNC         -> r_vsync     (aplicado no renderer do menu na hora)
 *
 * A tabela inferior mostra o que o sistema tem (gapi.c): so APIs detectadas
 * E com backend no porte entram na lista de opcoes; o resto aparece apenas
 * como informacao com [ ].
 */
#include "menu.h"
#include "gapi.h"
#include "gfx.h"          /* t4_gfx_config_get / t4_gfx_api_name / path */
#include "sdl2_dyn.h"

#include <stdlib.h>        /* qsort (enumeracao de resolucoes) */
#include <sys/stat.h>
#include <sys/types.h>

#if defined(_WIN32)
#include <direct.h>
#define t4_mkdir(p) _mkdir(p)
#else
#define t4_mkdir(p) mkdir((p), 0755)
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MENU_W 780
#define MENU_H 384

#define WIN_FULLSCREEN 0x00000001u   /* SDL_WINDOW_FULLSCREEN */
#define RDR_VSYNC      0x00000004u   /* SDL_RENDERER_PRESENTVSYNC */
#define RDR_SOFTWARE   0x00000001u   /* SDL_RENDERER_SOFTWARE */

/* cores: uint32 no layout RGBA dos bytes (little-endian) */
#define RGB(r, g, b) (0xFF000000u | ((uint32_t)(b) << 16) | \
                      ((uint32_t)(g) << 8) | (uint32_t)(r))
#define C_BG      RGB(18, 22, 32)
#define C_PANEL   RGB(30, 38, 56)
#define C_SELECT  RGB(56, 84, 132)
#define C_TEXT    RGB(228, 232, 240)
#define C_DIM     RGB(132, 142, 160)
#define C_ACCENT  RGB(255, 204, 68)
#define C_OK      RGB(120, 220, 140)
#define C_OFF     RGB(220, 120, 120)

/* ------------------------------------------------------------------ */
/* fonte bitmap 5x7 (ASCII 32..127); bit4 = coluna da esquerda         */
/* ------------------------------------------------------------------ */

static const unsigned char F[96][7] = {
    /* 32 ' ' */ {0,0,0,0,0,0,0},
    /* 33 '!' */ {0x04,0x04,0x04,0x04,0x04,0x00,0x04},
    /* 34 '"' */ {0,0,0,0,0,0,0},
    /* 35 '#' */ {0x0A,0x0A,0x1F,0x0A,0x1F,0x0A,0x0A},
    /* 36 '$' */ {0,0,0,0,0,0,0},
    /* 37 '%' */ {0x19,0x1A,0x02,0x04,0x08,0x0B,0x13},
    /* 38 '&' */ {0,0,0,0,0,0,0},
    /* 39 ''' */ {0,0,0,0,0,0,0},
    /* 40 '(' */ {0x02,0x04,0x08,0x08,0x08,0x04,0x02},
    /* 41 ')' */ {0x08,0x04,0x02,0x02,0x02,0x04,0x08},
    /* 42 '*' */ {0x00,0x0A,0x04,0x1F,0x04,0x0A,0x00},
    /* 43 '+' */ {0x00,0x04,0x04,0x1F,0x04,0x04,0x00},
    /* 44 ',' */ {0x00,0x00,0x00,0x00,0x00,0x04,0x08},
    /* 45 '-' */ {0x00,0x00,0x00,0x1F,0x00,0x00,0x00},
    /* 46 '.' */ {0x00,0x00,0x00,0x00,0x00,0x00,0x04},
    /* 47 '/' */ {0x01,0x02,0x02,0x04,0x08,0x08,0x10},
    /* 48 '0' */ {0x0E,0x11,0x13,0x15,0x19,0x11,0x0E},
    /* 49 '1' */ {0x04,0x0C,0x04,0x04,0x04,0x04,0x0E},
    /* 50 '2' */ {0x0E,0x11,0x01,0x02,0x04,0x08,0x1F},
    /* 51 '3' */ {0x1F,0x02,0x04,0x02,0x01,0x11,0x0E},
    /* 52 '4' */ {0x02,0x06,0x0A,0x12,0x1F,0x02,0x02},
    /* 53 '5' */ {0x1F,0x10,0x1E,0x01,0x01,0x11,0x0E},
    /* 54 '6' */ {0x06,0x08,0x10,0x1E,0x11,0x11,0x0E},
    /* 55 '7' */ {0x1F,0x01,0x02,0x04,0x08,0x08,0x08},
    /* 56 '8' */ {0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E},
    /* 57 '9' */ {0x0E,0x11,0x11,0x0F,0x01,0x02,0x0C},
    /* 58 ':' */ {0x00,0x00,0x04,0x00,0x04,0x00,0x00},
    /* 59 ';' */ {0,0,0,0,0,0,0},
    /* 60 '<' */ {0x02,0x04,0x08,0x10,0x08,0x04,0x02},
    /* 61 '=' */ {0x00,0x00,0x1F,0x00,0x1F,0x00,0x00},
    /* 62 '>' */ {0x08,0x04,0x02,0x01,0x02,0x04,0x08},
    /* 63 '?' */ {0x0E,0x11,0x01,0x02,0x04,0x00,0x04},
    /* 64 '@' */ {0,0,0,0,0,0,0},
    /* 65 'A' */ {0x0E,0x11,0x11,0x11,0x1F,0x11,0x11},
    /* 66 'B' */ {0x1E,0x11,0x11,0x1E,0x11,0x11,0x1E},
    /* 67 'C' */ {0x0E,0x11,0x10,0x10,0x10,0x11,0x0E},
    /* 68 'D' */ {0x1C,0x12,0x11,0x11,0x11,0x12,0x1C},
    /* 69 'E' */ {0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F},
    /* 70 'F' */ {0x1F,0x10,0x10,0x1E,0x10,0x10,0x10},
    /* 71 'G' */ {0x0E,0x11,0x10,0x17,0x11,0x11,0x0F},
    /* 72 'H' */ {0x11,0x11,0x11,0x1F,0x11,0x11,0x11},
    /* 73 'I' */ {0x0E,0x04,0x04,0x04,0x04,0x04,0x0E},
    /* 74 'J' */ {0x07,0x02,0x02,0x02,0x02,0x12,0x0C},
    /* 75 'K' */ {0x11,0x12,0x14,0x18,0x14,0x12,0x11},
    /* 76 'L' */ {0x10,0x10,0x10,0x10,0x10,0x10,0x1F},
    /* 77 'M' */ {0x11,0x1B,0x15,0x15,0x11,0x11,0x11},
    /* 78 'N' */ {0x11,0x19,0x15,0x13,0x11,0x11,0x11},
    /* 79 'O' */ {0x0E,0x11,0x11,0x11,0x11,0x11,0x0E},
    /* 80 'P' */ {0x1E,0x11,0x11,0x1E,0x10,0x10,0x10},
    /* 81 'Q' */ {0x0E,0x11,0x11,0x11,0x15,0x12,0x0D},
    /* 82 'R' */ {0x1E,0x11,0x11,0x1E,0x14,0x12,0x11},
    /* 83 'S' */ {0x0F,0x10,0x10,0x0E,0x01,0x01,0x1E},
    /* 84 'T' */ {0x1F,0x04,0x04,0x04,0x04,0x04,0x04},
    /* 85 'U' */ {0x11,0x11,0x11,0x11,0x11,0x11,0x0E},
    /* 86 'V' */ {0x11,0x11,0x11,0x11,0x11,0x0A,0x04},
    /* 87 'W' */ {0x11,0x11,0x11,0x15,0x15,0x15,0x0A},
    /* 88 'X' */ {0x11,0x11,0x0A,0x04,0x0A,0x11,0x11},
    /* 89 'Y' */ {0x11,0x11,0x0A,0x04,0x04,0x04,0x04},
    /* 90 'Z' */ {0x1F,0x01,0x02,0x04,0x08,0x10,0x1F},
    /* 91 '[' */ {0x0E,0x08,0x08,0x08,0x08,0x08,0x0E},
    /* 92 '\' */ {0x10,0x08,0x08,0x04,0x02,0x02,0x01},
    /* 93 ']' */ {0x0E,0x02,0x02,0x02,0x02,0x02,0x0E},
    /* 94 '^' */ {0x04,0x0A,0x11,0x00,0x00,0x00,0x00},
    /* 95 '_' */ {0x00,0x00,0x00,0x00,0x00,0x00,0x1F},
    /* 96 '`' */ {0x08,0x04,0x00,0x00,0x00,0x00,0x00},
    /* 97 'a' */ {0x00,0x00,0x0E,0x01,0x0F,0x11,0x0F},
    /* 98 'b' */ {0x10,0x10,0x1E,0x11,0x11,0x11,0x1E},
    /* 99 'c' */ {0x00,0x00,0x0E,0x11,0x10,0x11,0x0E},
    /* 100 'd' */ {0x01,0x01,0x0F,0x11,0x11,0x11,0x0F},
    /* 101 'e' */ {0x00,0x00,0x0E,0x11,0x1F,0x10,0x0E},
    /* 102 'f' */ {0x06,0x09,0x08,0x1C,0x08,0x08,0x08},
    /* 103 'g' */ {0x00,0x00,0x0F,0x11,0x0F,0x01,0x0E},
    /* 104 'h' */ {0x10,0x10,0x1E,0x11,0x11,0x11,0x11},
    /* 105 'i' */ {0x04,0x00,0x0C,0x04,0x04,0x04,0x0E},
    /* 106 'j' */ {0x02,0x00,0x06,0x02,0x02,0x12,0x0C},
    /* 107 'k' */ {0x10,0x10,0x12,0x14,0x18,0x14,0x12},
    /* 108 'l' */ {0x0C,0x04,0x04,0x04,0x04,0x04,0x0E},
    /* 109 'm' */ {0x00,0x00,0x1A,0x15,0x15,0x15,0x15},
    /* 110 'n' */ {0x00,0x00,0x1E,0x11,0x11,0x11,0x11},
    /* 111 'o' */ {0x00,0x00,0x0E,0x11,0x11,0x11,0x0E},
    /* 112 'p' */ {0x00,0x00,0x1E,0x11,0x1E,0x10,0x10},
    /* 113 'q' */ {0x00,0x00,0x0F,0x11,0x0F,0x01,0x01},
    /* 114 'r' */ {0x00,0x00,0x16,0x19,0x10,0x10,0x10},
    /* 115 's' */ {0x00,0x00,0x0F,0x10,0x0E,0x01,0x1E},
    /* 116 't' */ {0x08,0x08,0x1C,0x08,0x08,0x09,0x06},
    /* 117 'u' */ {0x00,0x00,0x11,0x11,0x11,0x13,0x0D},
    /* 118 'v' */ {0x00,0x00,0x11,0x11,0x11,0x0A,0x04},
    /* 119 'w' */ {0x00,0x00,0x11,0x11,0x15,0x15,0x0A},
    /* 120 'x' */ {0x00,0x00,0x11,0x0A,0x04,0x0A,0x11},
    /* 121 'y' */ {0x00,0x00,0x11,0x11,0x0F,0x01,0x0E},
    /* 122 'z' */ {0x00,0x00,0x1F,0x02,0x04,0x08,0x1F},
    /* 123 '{' */ {0x02,0x04,0x04,0x08,0x04,0x04,0x02},
    /* 124 '|' */ {0x04,0x04,0x04,0x04,0x04,0x04,0x04},
    /* 125 '}' */ {0x08,0x04,0x04,0x02,0x04,0x04,0x08},
    /* 126 '~' */ {0x00,0x00,0x09,0x15,0x12,0x00,0x00},
    /* 127     */ {0,0,0,0,0,0,0},
};

/* ------------------------------------------------------------------ */
/* framebuffer + desenho                                               */
/* ------------------------------------------------------------------ */

static uint32_t *fb;   /* MENU_W * MENU_H pixels RGBA */

static void rect(int x, int y, int w, int h, uint32_t c)
{
    int i, j;
    if (w <= 0 || h <= 0) return;
    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > MENU_W) w = MENU_W - x;
    if (y + h > MENU_H) h = MENU_H - y;
    if (w <= 0 || h <= 0) return;
    for (j = 0; j < h; j++) {
        uint32_t *row = fb + (size_t)(y + j) * MENU_W + x;
        for (i = 0; i < w; i++) row[i] = c;
    }
}

static void text_w_calc(const char *s, int sc, int *w)  /* apenas largura */
{
    *w = (int)strlen(s) * 6 * sc;
}

static void text(int x, int y, const char *s, uint32_t col, int sc)
{
    int cx;
    for (cx = 0; s[cx]; cx++) {
        unsigned char ch = (unsigned char)s[cx];
        const unsigned char *glyph;
        int row, b;
        if (ch < 32 || ch > 127) ch = '?';
        glyph = F[ch - 32];
        for (row = 0; row < 7; row++) {
            unsigned char bits = glyph[row];
            if (!bits) continue;
            for (b = 0; b < 5; b++)
                if (bits & (0x10u >> b))
                    rect(x + (cx * 6 + b) * sc, y + row * sc, sc, sc, col);
        }
    }
}

static void text_trunc(int x, int y, const char *s, uint32_t col, int sc,
                       int max_px)
{
    char buf[128];
    int max_chars = max_px / (6 * sc);
    size_t n = strlen(s);
    if (max_chars < 1) return;
    if ((int)n <= max_chars) { text(x, y, s, col, sc); return; }
    if (max_chars > 3) max_chars -= 3;
    if (n > (size_t)max_chars) n = (size_t)max_chars;
    memcpy(buf, s, n);
    if (max_chars > 3) memcpy(buf + n, "...", 3), n += 3;
    buf[n] = '\0';
    text(x, y, buf, col, sc);
}

/* ------------------------------------------------------------------ */
/* configuracao t4port.cfg (leitura/escrita preservando as chaves)      */
/* ------------------------------------------------------------------ */

static void ensure_dir(const char *file)
{
    char dir[1100];
    char *slash;
    size_t n = strlen(file);
    if (n >= sizeof(dir)) return;
    memcpy(dir, file, n + 1);
    slash = strrchr(dir, '/');
    if (!slash) return;
    *slash = '\0';
    t4_mkdir(dir);
}

static int cfg_read(const char *key, char *val, size_t cap)
{
    char path[1100], line[256];
    FILE *fp;
    size_t klen = strlen(key);
    int found = 0;

    t4_gfx_config_path(path, sizeof(path));
    fp = fopen(path, "r");
    if (!fp) return 0;
    while (fgets(line, sizeof(line), fp)) {
        char *nl = strchr(line, '\n');
        char *eq;
        if (nl) *nl = '\0';
        if (line[0] == '#') continue;
        eq = strchr(line, '=');
        if (!eq) continue;
        if ((size_t)(eq - line) != klen) continue;
        if (strncmp(line, key, klen) != 0) continue;
        snprintf(val, cap, "%s", eq + 1);
        found = 1;
        break;
    }
    fclose(fp);
    return found;
}

/* reescreve o arquivo inteiro com a chave atualizada (upsert) */
static int cfg_write(const char *key, const char *val)
{
    char path[1100], line[256], buf[16384];
    FILE *fp;
    size_t n = 0;
    int replaced = 0;
    size_t klen = strlen(key);

    t4_gfx_config_path(path, sizeof(path));
    fp = fopen(path, "r");
    if (fp) {
        while (fgets(line, sizeof(line), fp)) {
            size_t len = strlen(line);
            if (!replaced && line[0] != '#') {
                char *nl = strchr(line, '\n');
                char *eq;
                if (nl) *nl = '\0';
                eq = strchr(line, '=');
                if (eq && (size_t)(eq - line) == klen &&
                    strncmp(line, key, klen) == 0) {
                    int w = snprintf(buf + n, sizeof(buf) - n,
                                     "%s=%s\n", key, val);
                    if (w > 0 && (size_t)w < sizeof(buf) - n) n += (size_t)w;
                    replaced = 1;
                    continue;
                }
                len = strlen(line);
                if (len + 1 < sizeof(buf) - n) {
                    memcpy(buf + n, line, len);
                    n += len;
                    buf[n++] = '\n';
                    buf[n] = '\0';
                }
                continue;
            }
            if (len < sizeof(buf) - n) {
                memcpy(buf + n, line, len);
                n += len;
                buf[n] = '\0';
            }
        }
        fclose(fp);
    }
    if (!replaced) {
        int w = snprintf(buf + n, sizeof(buf) - n, "%s=%s\n", key, val);
        if (w > 0 && (size_t)w < sizeof(buf) - n) n += (size_t)w;
    }

    ensure_dir(path);
    fp = fopen(path, "w");
    if (!fp) return -1;
    fwrite(buf, 1, n, fp);
    fclose(fp);
    return 0;
}

/* ------------------------------------------------------------------ */
/* tabela: linhas do menu                                              */
/* ------------------------------------------------------------------ */

enum { R_API = 0, R_MODE, R_FS, R_VS, R_COUNT };

/* presets: sempre disponiveis (janela e monitores fora da enumeracao) */
static const char *mode_presets[] = {
    "640x480", "800x600", "1024x768", "1152x864", "1280x720",
    "1280x768", "1280x800", "1280x960", "1280x1024", "1360x768",
    "1366x768", "1440x900", "1600x900", "1600x1200", "1680x1050",
    "1920x1080", "1920x1200", "2048x1152", "2560x1080", "2560x1440",
    "2560x1600", "2880x1620", "3200x1800", "3440x1440", "3840x1600",
    "3840x2160", "3840x2400", "5120x1440", "5120x2160", "5120x2880",
    "7680x4320", NULL
};

typedef struct {
    T4GApi list[8];
    int    n;
    char   api_ids[8][16];  /* opcoes selecionaveis: auto + suportadas */
    int    n_api;
    int    i_api, i_mode, i_fs, i_vs;
    char   modes[192][16];  /* WxH: modos do monitor + presets + cfg */
    int    n_modes;
    char   status[160];
} MenuState;

/* adiciona WxH a lista (dedupe); limites sensiveis a lixo de driver */
static int mode_add(MenuState *m, int w, int h)
{
    char tmp[16];
    int i;
    if (w < 320 || h < 200 || w > 16384 || h > 16384) return 0;
    snprintf(tmp, sizeof(tmp), "%dx%d", w, h);
    for (i = 0; i < m->n_modes; i++)
        if (strcmp(m->modes[i], tmp) == 0) return 0;
    if (m->n_modes >= (int)(sizeof(m->modes) / sizeof(m->modes[0])))
        return 0;
    snprintf(m->modes[m->n_modes], sizeof(m->modes[0]), "%s", tmp);
    m->n_modes++;
    return 1;
}

static int mode_cmp(const void *pa, const void *pb)
{
    const char *a = (const char *)pa, *b = (const char *)pb;
    int aw = 0, ah = 0, bw = 0, bh = 0;
    sscanf(a, "%dx%d", &aw, &ah);
    sscanf(b, "%dx%d", &bw, &bh);
    if ((long)aw * ah != (long)bw * bh)
        return (long)aw * ah < (long)bw * bh ? -1 : 1;
    return aw - bw;
}

/*
 * TODAS as resolucoes: modos reais de cada monitor/saida (SDL) unidos aos
 * presets e ao r_mode do cfg (mesmo digitado a mao, ex. 3440x1440).
 * Chamar com SDL de video inicializado; ordena por area crescente.
 */
static void menu_load_modes(MenuState *m)
{
    int d, k, ndisp, w, h;
    char cur[64];

    m->n_modes = 0;
    if (t4_SDL_GetNumVideoDisplays && t4_SDL_GetNumDisplayModes &&
        t4_SDL_GetDisplayMode) {
        ndisp = t4_SDL_GetNumVideoDisplays();
        for (d = 0; d < ndisp; d++) {
            int nm = t4_SDL_GetNumDisplayModes(d);
            for (k = 0; k < nm; k++) {
                T4_SDL_DisplayMode dm;
                memset(&dm, 0, sizeof(dm));
                if (t4_SDL_GetDisplayMode(d, k, &dm) == 0)
                    mode_add(m, dm.w, dm.h);
            }
        }
    }
    for (k = 0; mode_presets[k]; k++)
        if (sscanf(mode_presets[k], "%dx%d", &w, &h) == 2)
            mode_add(m, w, h);
    if (cfg_read("r_mode", cur, sizeof(cur)) &&
        sscanf(cur, "%dx%d", &w, &h) == 2)
        mode_add(m, w, h);
    if (m->n_modes > 1)
        qsort(m->modes, (size_t)m->n_modes, sizeof(m->modes[0]), mode_cmp);

    /* indice do modo atual (cfg ou 1280x720) */
    m->i_mode = 0;
    if (!cfg_read("r_mode", cur, sizeof(cur)))
        snprintf(cur, sizeof(cur), "1280x720");
    for (k = 0; k < m->n_modes; k++)
        if (strcmp(m->modes[k], cur) == 0) { m->i_mode = k; break; }
}

static const char *api_display(const MenuState *m, const char *id)
{
    const T4GApi *g;
    if (strcmp(id, "auto") == 0) return "auto (automatic detection)";
    g = t4_gapi_find(m->list, m->n, id);
    if (g) return g->name;
    return id;
}

static void menu_init(MenuState *m)
{
    char cur[64];
    int i, found = -1;

    m->n = t4_gapi_probe(m->list, 8);

    /* opcoes: "auto" + APIs detectadas COM backend no porte */
    m->n_api = 0;
    snprintf(m->api_ids[m->n_api++], sizeof(m->api_ids[0]), "auto");
    for (i = 0; i < m->n && m->n_api < 8; i++)
        if (m->list[i].detected && m->list[i].support)
            snprintf(m->api_ids[m->n_api++], sizeof(m->api_ids[0]),
                     "%s", m->list[i].id);

    /* indice atual a partir do cfg */
    cur[0] = '\0';
    snprintf(cur, sizeof(cur), "%s", t4_gfx_api_name(t4_gfx_config_get()));
    for (i = 0; i < m->n_api; i++)
        if (strcmp(m->api_ids[i], cur) == 0) found = i;
    if (found < 0 && m->n_api < 8) {           /* cfg com API indisponivel:
                                                  mostra mesmo assim */
        snprintf(m->api_ids[m->n_api], sizeof(m->api_ids[0]), "%s", cur);
        found = m->n_api++;
    }
    m->i_api = found >= 0 ? found : 0;

    if (!cfg_read("r_mode", cur, sizeof(cur)))
        snprintf(cur, sizeof(cur), "1280x720");
    m->i_mode = 0;   /* indice real definido em menu_load_modes() (pos-SDL) */
    (void)cur;

    if (!cfg_read("r_fullscreen", cur, sizeof(cur)))
        snprintf(cur, sizeof(cur), "0");
    m->i_fs = cur[0] == '1';

    if (!cfg_read("r_vsync", cur, sizeof(cur)))
        snprintf(cur, sizeof(cur), "0");
    m->i_vs = cur[0] == '1';

    snprintf(m->status, sizeof(m->status),
             "%d graphics APIs detected", m->n);
}

static const char *row_label(int row)
{
    switch (row) {
    case R_API:  return "VIDEO API";
    case R_MODE: return "RESOLUTION";
    case R_FS:   return "FULLSCREEN";
    case R_VS:   return "VSYNC";
    }
    return "";
}

static void row_value(const MenuState *m, int row, char *out, size_t cap)
{
    switch (row) {
    case R_API:
        snprintf(out, cap, "%s", api_display(m, m->api_ids[m->i_api]));
        break;
    case R_MODE:
        snprintf(out, cap, "%s", m->n_modes ? m->modes[m->i_mode] : "?");
        break;
    case R_FS:
        snprintf(out, cap, "%s", m->i_fs ? "ON" : "OFF");
        break;
    case R_VS:
        snprintf(out, cap, "%s", m->i_vs ? "ON" : "OFF");
        break;
    default:
        out[0] = '\0';
    }
}

/* aplica a mudanca da linha `row` em +/-1 e grava no cfg */
static void row_change(MenuState *m, int row, int delta)
{
    char val[64];
    const char *key = NULL;

    switch (row) {
    case R_API: {
        int n = m->n_api;
        m->i_api = (m->i_api + n + (delta % n)) % n;
        snprintf(val, sizeof(val), "%s", m->api_ids[m->i_api]);
        key = "r_api";
        break;
    }
    case R_MODE: {
        int n = m->n_modes > 0 ? m->n_modes : 1;
        m->i_mode = (m->i_mode + n + (delta % n)) % n;
        snprintf(val, sizeof(val), "%s", m->modes[m->i_mode]);
        key = "r_mode";
        break;
    }
    case R_FS:
        m->i_fs = !m->i_fs;
        snprintf(val, sizeof(val), "%d", m->i_fs);
        key = "r_fullscreen";
        break;
    case R_VS:
        m->i_vs = !m->i_vs;
        snprintf(val, sizeof(val), "%d", m->i_vs);
        key = "r_vsync";
        break;
    default:
        return;
    }

    if (cfg_write(key, val) == 0)
        snprintf(m->status, sizeof(m->status), "saved: %s=%s", key, val);
    else
        snprintf(m->status, sizeof(m->status),
                 "error saving %s (check %s)", key, "t4port.cfg");
}

/* ------------------------------------------------------------------ */
/* desenho do quadro                                                   */
/* ------------------------------------------------------------------ */

static void draw(const MenuState *m, int sel)
{
    int y, i, w;
    char val[128];

    rect(0, 0, MENU_W, MENU_H, C_BG);
    rect(0, 0, MENU_W, 4, C_SELECT);

    text_w_calc("OPTIONS - T4PORT", 2, &w);
    text((MENU_W - w) / 2, 14, "OPTIONS - T4PORT", C_ACCENT, 2);
    rect(24, 44, MENU_W - 48, 1, C_PANEL);

    /* secao GRAPHICS: as opcoes de video vivem DENTRO do jogo, em ingles */
    text(32, 47, "GRAPHICS", C_ACCENT, 1);

    /* linhas de opcoes */
    y = 60;
    for (i = 0; i < R_COUNT; i++) {
        int on = (i == sel);
        if (on) {
            rect(32, y - 5, MENU_W - 64, 24, C_SELECT);
            text(38, y + 2, ">", C_ACCENT, 2);
        }
        text(56, y + 2, row_label(i), on ? C_TEXT : C_DIM, 2);
        row_value(m, i, val, sizeof(val));
        text(340, y + 2, val, on ? C_TEXT : C_DIM, 2);
        y += 26;
    }

    rect(24, 164, MENU_W - 48, 1, C_PANEL);
    text(32, 172, "GRAPHICS APIs - DETECTED ON THIS SYSTEM:", C_ACCENT, 1);
    text(36, 188, "  NAME           LOADER                 DETAIL",
         C_DIM, 1);

    /* tabela do sistema: tudo que o probe achou */
    y = 202;
    for (i = 0; i < m->n; i++) {
        const T4GApi *g = &m->list[i];
        const T4GApi *cur =
            t4_gapi_find(m->list, m->n, m->api_ids[m->i_api]);
        int is_cur = (m->i_api > 0 && cur == g);
        uint32_t c = is_cur ? C_ACCENT : (g->detected ? C_OK : C_OFF);
        text(36, y, g->detected ? "[x]" : "[ ]", c, 1);
        text_trunc(64, y, g->name, is_cur ? C_TEXT : C_DIM, 1, 14 * 6);
        text_trunc(154, y, g->loader, C_DIM, 1, 24 * 6);
        text_trunc(306, y, g->detail,
                   g->support ? C_TEXT : C_DIM, 1, MENU_W - 306 - 16);
        y += 14;
    }

    y += 4;
    text(36, y, "[x] detected   [ ] missing   backend: yes = the port "
                "can switch to it", C_DIM, 1);
    y += 16;
    rect(24, y, MENU_W - 48, 1, C_PANEL);
    y += 8;
    text(32, y, "ARROWS/LEFT-RIGHT: navigate and change   ENTER/ESC: save and quit",
         C_TEXT, 1);
    y += 14;
    text(32, y, m->status, C_ACCENT, 1);
    y += 14;
    text(32, y, "API applies to game commands; fullscreen on next open; "
                "vsync immediately.", C_DIM, 1);
}

/* ------------------------------------------------------------------ */
/* eventos                                                             */
/* ------------------------------------------------------------------ */

/* chaves via SDL_Keycode (offset 20 do evento) com fallback em Scancode */
#define K_NONE   0
#define K_UP     1
#define K_DOWN   2
#define K_LEFT   3
#define K_RIGHT  4
#define K_ENTER  5
#define K_ESC    6

static int decode_key(const T4_SDLEvent *ev)
{
    int32_t sym = 0;
    uint8_t sc;

    if (ev->type != T4_SDL_KEYDOWN) return K_NONE;
    memcpy(&sym, ev->raw + 20, 4);
    sc = ev->raw[16];

    switch (sym) {
    case 0x40000052: return K_UP;      /* SDLK_UP */
    case 0x40000051: return K_DOWN;    /* SDLK_DOWN */
    case 0x40000050: return K_LEFT;    /* SDLK_LEFT */
    case 0x4000004F: return K_RIGHT;   /* SDLK_RIGHT */
    case 13:         return K_ENTER;   /* SDLK_RETURN */
    case 27:         return K_ESC;     /* SDLK_ESCAPE */
    default: break;
    }
    /* fallback: SDL_Scancode */
    switch (sc) {
    case 82: return K_UP;
    case 81: return K_DOWN;
    case 80: return K_LEFT;
    case 79: return K_RIGHT;
    case 40: return K_ENTER;
    case 41: return K_ESC;
    default: return K_NONE;
    }
}

/* ------------------------------------------------------------------ */
/* --list (sem display; util para teste automatizado)                  */
/* ------------------------------------------------------------------ */

static int cmd_list(void)
{
    T4GApi list[8];
    int n = t4_gapi_probe(list, 8);
    int i;

    printf("System graphics APIs (%s):\n", t4_gapi_os_name());
    for (i = 0; i < n; i++) {
        const T4GApi *g = &list[i];
        printf("  [%c] %-13s backend:%-4s %-24s %s\n",
               g->detected ? 'x' : ' ', g->name,
               g->support ? "yes" : "no",
               g->loader, g->detail);
    }
    printf("current selection: r_api=%s (t4port.cfg)\n",
           t4_gfx_api_name(t4_gfx_config_get()));
    printf("usable in menu:");
    printf(" auto");
    for (i = 0; i < n; i++)
        if (list[i].detected && list[i].support)
            printf(" %s", list[i].id);
    printf("\n");
    {
        T4Gpu gp;
        t4_gapi_gpu(&gp);
        printf("GPUs: %d NVIDIA, %d AMD, %d Intel", gp.nvidia, gp.amd,
               gp.intel);
        if (gp.nouveau) printf(" (nouveau)");
        if (gp.nv_module[0] || gp.nv_gl[0]) {
            printf("  | NVIDIA: module %s GL %s Vulkan %s ICD %s",
                   gp.nv_module[0] ? gp.nv_module : "?",
                   gp.nv_gl[0] ? gp.nv_gl : "missing",
                   gp.nv_vk > 0 ? "yes" : (gp.nv_vk == 0 ? "no" : "?"),
                   gp.nv_icd ? "active" : "MISSING (gpu --fix)");
        }
        printf("\n");
    }

    /* resolucoes: todos os modos do monitor + presets (qualquer WxH no cfg) */
    if (t4_sdl_load() == 0 && t4_SDL_Init(T4_SDL_INIT_VIDEO) == 0) {
        MenuState ms;
        int col = 0, k;
        memset(&ms, 0, sizeof(ms));
        menu_load_modes(&ms);
        printf("resolutions: %d modes (monitors + presets + r_mode in cfg)\n  ",
               ms.n_modes);
        for (k = 0; k < ms.n_modes; k++) {
            int add = (int)strlen(ms.modes[k]) + (col ? 3 : 0);
            if (col && col + add > 72) { printf("\n  "); col = 0; }
            printf("%s%s", col ? " | " : "", ms.modes[k]);
            col += add;
        }
        printf("\n");
        t4_SDL_Quit();
    } else {
        printf("resolutions: enumeration skipped (SDL2/display unavailable)\n");
    }
    return 0;
}

/* ------------------------------------------------------------------ */
/* janela do menu                                                      */
/* ------------------------------------------------------------------ */

static T4_SDL_Renderer *make_renderer(T4_SDL_Window *win, int vsync,
                                      T4_SDL_Texture **tex)
{
    uint32_t flags = vsync ? RDR_VSYNC : 0;
    T4_SDL_Renderer *r = t4_SDL_CreateRenderer(win, -1, flags);
    if (!r) r = t4_SDL_CreateRenderer(win, -1, 0);
    if (!r) r = t4_SDL_CreateRenderer(win, -1, RDR_SOFTWARE);
    *tex = NULL;
    if (!r) return NULL;
    *tex = t4_SDL_CreateTexture(r, 0x16762004 /* RGBA32 */, 1 /* streaming */,
                                MENU_W, MENU_H);
    if (!*tex) {
        t4_SDL_DestroyRenderer(r);
        return NULL;
    }
    return r;
}

int t4_menu_cmd(int argc, char **argv)
{
    MenuState m;
    T4_SDL_Window *win = NULL;
    T4_SDL_Renderer *ren = NULL;
    T4_SDL_Texture *tex = NULL;
    int i, sel = 0, quit = 0;
    int win_w = MENU_W, win_h = MENU_H;
    char mode[32];

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--list") == 0) return cmd_list();
        if (strcmp(argv[i], "--help") == 0) {
            printf("t4port menu [--list]\n"
                   "  (no arguments) opens the video options window "
                   "(Options > Graphics)\n"
                   "  --list           prints the detected APIs and exits\n");
            return 0;
        }
    }

    menu_init(&m);

    /* resolucao do modo escolhido (janela fullscreen usa o tamanho real) */
    mode[0] = '\0';
    cfg_read("r_mode", mode, sizeof(mode));
    {
        int w = 0, h = 0;
        if (sscanf(mode, "%dx%d", &w, &h) == 2 &&
            w >= 320 && h >= 240 && w <= 7680 && h <= 4320) {
            win_w = w;
            win_h = h;
        }
    }

    if (t4_sdl_load() != 0) {
        /* t4_sdl_load devolve 0 no SUCESSO; !0 reportava falha com SDL ok */
        printf("t4port menu: SDL2 unavailable (install libSDL2, or use "
               "the Flatpak com.t4port.engine)\n");
        return 1;
    }
    if (t4_SDL_Init(T4_SDL_INIT_VIDEO) != 0) {
        printf("t4port menu: SDL_Init failed: %s\n", t4_SDL_GetError());
        return 1;
    }
    menu_load_modes(&m);   /* TODAS as resolucoes: monitor + presets + cfg */

    win = t4_SDL_CreateWindow("t4port - Options",
                              T4_SDL_WINDOWPOS_CENTERED,
                              T4_SDL_WINDOWPOS_CENTERED,
                              win_w, win_h,
                              m.i_fs ? WIN_FULLSCREEN : 0);
    if (!win && m.i_fs) {
        /* fullscreen falhou (ex.: sem display) - janela normal */
        m.i_fs = 0;
        snprintf(m.status, sizeof(m.status),
                 "fullscreen unavailable here; opening windowed");
        win = t4_SDL_CreateWindow("t4port - Options",
                                  T4_SDL_WINDOWPOS_CENTERED,
                                  T4_SDL_WINDOWPOS_CENTERED,
                                  MENU_W, MENU_H, 0);
    }
    if (!win) {
        printf("t4port menu: could not open window: %s\n", t4_SDL_GetError());
        t4_SDL_Quit();
        return 1;
    }

    ren = make_renderer(win, m.i_vs, &tex);
    if (!ren) {
        printf("t4port menu: SDL renderer failed: %s\n", t4_SDL_GetError());
        t4_SDL_DestroyWindow(win);
        t4_SDL_Quit();
        return 1;
    }

    fb = (uint32_t *)malloc((size_t)MENU_W * MENU_H * 4);
    if (!fb) {
        t4_SDL_DestroyTexture(tex);
        t4_SDL_DestroyRenderer(ren);
        t4_SDL_DestroyWindow(win);
        t4_SDL_Quit();
        return 1;
    }

    while (!quit) {
        T4_SDLEvent ev;
        while (t4_SDL_PollEvent(&ev)) {
            /* X da janela, ou SIGTERM/SIGINT (o SDL converte em QUIT):
             * sem isso a janela so fecha com ESC/ENTER e ignora o fecho */
            if (ev.type == T4_SDL_QUIT) {
                quit = 1;
                break;
            }
            int k = decode_key(&ev);
            if (k == K_ESC || k == K_ENTER) {
                quit = 1;
            } else if (k == K_UP) {
                sel = (sel + R_COUNT - 1) % R_COUNT;
            } else if (k == K_DOWN) {
                sel = (sel + 1) % R_COUNT;
            } else if (k == K_LEFT) {
                row_change(&m, sel, -1);
                if (sel == R_VS) {          /* vsync muda o renderer na hora */
                    T4_SDL_Texture *nt = NULL;
                    T4_SDL_Renderer *nr = make_renderer(win, m.i_vs, &nt);
                    if (nr) {
                        t4_SDL_DestroyTexture(tex);
                        t4_SDL_DestroyRenderer(ren);
                        ren = nr;
                        tex = nt;
                    }
                }
            } else if (k == K_RIGHT) {
                row_change(&m, sel, +1);
                if (sel == R_VS) {
                    T4_SDL_Texture *nt = NULL;
                    T4_SDL_Renderer *nr = make_renderer(win, m.i_vs, &nt);
                    if (nr) {
                        t4_SDL_DestroyTexture(tex);
                        t4_SDL_DestroyRenderer(ren);
                        ren = nr;
                        tex = nt;
                    }
                }
            }
        }
        if (quit) break;

        draw(&m, sel);
        if (t4_SDL_UpdateTexture(tex, NULL, fb, MENU_W * 4) != 0) {
            /* superficie ocupada: tenta no proximo quadro */
        }
        t4_SDL_RenderClear(ren);
        t4_SDL_RenderCopy(ren, tex, NULL, NULL);
        t4_SDL_RenderPresent(ren);
        t4_SDL_Delay(16);
    }

    free(fb);
    fb = NULL;
    t4_SDL_DestroyTexture(tex);
    t4_SDL_DestroyRenderer(ren);
    t4_SDL_DestroyWindow(win);
    t4_SDL_Quit();
    printf("t4port menu: saved (%s)\n", m.status);
    return 0;
}
