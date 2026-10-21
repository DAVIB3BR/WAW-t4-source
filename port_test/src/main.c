/*
 * main.c - t4port: nucleo nativo do porte (Call of Duty: World at War, motor IW 3.0).
 *
 * "T4" e o codigo interno da Treyarch para World at War (a engine IW 3.0, variante
 * Treyarch da Infinity Ward 3.0). Prefixo usado nos tipos publicos (T4Iwd, T4FF,
 * T4Iwi...). O Black Ops de 2010 e o T5.
 *
 * Subcomandos:
 *   info  <dir_do_jogo>              inventario dos .iwd/.ff
 *   zip   <arquivo.iwd> [filtro]     lista entradas de um .iwd
 *   ff    <arquivo.ff>               descompacta e mostra o cabecalho
 *   iwi   <arquivo.iwi> [-o x.png]   decodifica textura
 *   view  <arquivo.iwi>              abre janela com a textura (SDL2)
 *   view  --iwd <a.iwd> <entrada>    idem, lendo direto do .iwd
 *   extract <a.iwd> <entrada> -o f   extrai um arquivo do .iwd
 */
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <unistd.h>

#include "ff.h"
#include "iwd.h"
#include "gapi.h"
#include "gfx.h"
#include "gsc.h"
#include "iwi.h"
#include "menu.h"
#include "mods.h"
#include "net.h"
#include "platform.h"
#include "png.h"
#include "sdl2_dyn.h"
#include "world.h"

#include <zlib.h>

static int bundle_game_dir(char *buf, size_t cap);

static void usage(void)
{
    printf(
        "t4port - nucleo nativo do porte de Call of Duty: World at War (motor IW 3.0)\n"
        "\n"
        "uso:\n"
        "  t4port info  <dir_do_jogo>\n"
        "  t4port zip   <arquivo.iwd> [filtro]\n"
        "  t4port ff    <arquivo.ff> [-o dir] [filtro]\n"
        "  t4port mods  init|unpack|list|which ...\n"
        "  t4port gsc   <arquivo.gsc | pasta>   valida scripts extraidos\n"
        "  t4port iwi   <arquivo.iwi> [-o saida.png]\n"
        "  t4port view  <arquivo.iwi>\n"
        "  t4port view  --iwd <arquivo.iwd> <entrada>\n"
        "  t4port extract <arquivo.iwd> <entrada> -o <saida>\n"
        "  t4port video  <arquivo.bik> [-o saida.mp4] [--play]\n"
        "  t4port api    [auto|opengl|vulkan|opengles]   API grafica (video)\n"
        "  t4port play   [dir]                    start the game (Windows: native)\n"
        "  t4port menu   [--list]                 video options and system APIs\n"
        "  t4port gpu    [--fix|--unfix]          placas/drivers (NVIDIA, AMD, Intel)\n"
        "  t4port gfx    [--api A] [--segundos N] [--janela W H]  janela de teste\n"
        "  t4port world  <mapa.ff> [--api A] [--segundos N] [--shot x.png]  renderiza o mapa\n"
        "  t4port lan host      [--nome N] [--mapa M] [--modo MD] [--porta P]\n"
        "  t4port lan discover  [--espera MS] [--porta P] [--to IP]\n"
        "  t4port check | --check   diagnostica o ambiente\n");
}

static int has_suffix(const char *s, const char *suf)
{
    size_t n = strlen(s), m = strlen(suf);
    return n >= m && strcasecmp(s + n - m, suf) == 0;
}

/* ---------------------------------------------------------------- info -- */

static int count_ff_header(const char *path, uint32_t *hdr0)
{
    FILE *fp = fopen(path, "rb");
    uint8_t h[12];
    if (!fp) return -1;
    if (fread(h, 1, 12, fp) != 12) { fclose(fp); return -1; }
    fclose(fp);
    if (memcmp(h, T4_FF_MAGIC, 8) != 0) return -1;
    if (hdr0)
        *hdr0 = (uint32_t)h[8] | ((uint32_t)h[9] << 8) | ((uint32_t)h[10] << 16) | ((uint32_t)h[11] << 24);
    return 0;
}

static int cmd_info(const char *root)
{
    char path[1024];
    DIR *d;
    struct dirent *e;
    int n_iwd = 0, n_ff = 0;
    uint64_t iwd_bytes = 0, ff_bytes = 0, iwd_unpacked = 0;
    uint64_t entries = 0;

    snprintf(path, sizeof(path), "%s/main", root);
    d = opendir(path);
    if (!d) { fprintf(stderr, "t4port: nao encontrei '%s' (esperado <jogo>/main)\n", path); return 1; }

    printf("== %s ==\n", root);
    printf("%-28s %12s %10s %14s\n", "iwd", "tamanho", "entradas", "descompactado");
    while ((e = readdir(d))) {
        T4Iwd z;
        struct stat st;
        if (!has_suffix(e->d_name, ".iwd")) continue;
        snprintf(path, sizeof(path), "%s/main/%s", root, e->d_name);
        if (stat(path, &st) != 0) continue;
        if (t4_iwd_open(&z, path) == 0) {
            printf("%-28s %12.1f MB %10zu %14.1f MB\n", e->d_name,
                   st.st_size / 1048576.0, t4_iwd_count(&z),
                   z.total_uncomp / 1048576.0);
            entries += t4_iwd_count(&z);
            iwd_unpacked += z.total_uncomp;
            t4_iwd_close(&z);
        } else {
            printf("%-28s  ERRO: %s\n", e->d_name, t4_iwd_error());
        }
        n_iwd++;
        iwd_bytes += (uint64_t)st.st_size;
    }
    closedir(d);

    printf("\n%-.1f MB em %d .iwd, %llu entradas, %-.1f MB descompactados\n\n",
           iwd_bytes / 1048576.0, n_iwd, (unsigned long long)entries,
           iwd_unpacked / 1048576.0);

    snprintf(path, sizeof(path), "%s/zone", root);
    d = opendir(path);
    if (d) {
        struct dirent *de;
        printf("%-32s %12s\n", "fastfile", "tamanho");
        while ((de = readdir(d))) {
            char zp[1024];
            DIR *zd;
            struct dirent *ze;
            if (de->d_name[0] == '.') continue;
            if (strcmp(de->d_name, "english") != 0) continue;
            snprintf(zp, sizeof(zp), "%s/zone/%s", root, de->d_name);
            zd = opendir(zp);
            if (!zd) continue;
            while ((ze = readdir(zd))) {
                struct stat st;
                uint32_t h0 = 0;
                if (!has_suffix(ze->d_name, ".ff")) continue;
                snprintf(path, sizeof(path), "%s/%s", zp, ze->d_name);
                if (stat(path, &st) != 0) continue;
                if (count_ff_header(path, &h0) == 0) {
                    printf("%-32s %12.1f MB   (hdr=0x%08x)\n", ze->d_name,
                           st.st_size / 1048576.0, h0);
                    n_ff++;
                    ff_bytes += (uint64_t)st.st_size;
                }
            }
            closedir(zd);
        }
        closedir(d);
        printf("\n%-.1f MB em %d fastfiles\n", ff_bytes / 1048576.0, n_ff);
    }
    return 0;
}

/* ----------------------------------------------------------------- zip -- */

static int cmd_zip(const char *iwd, const char *filter)
{
    T4Iwd z;
    size_t i, shown = 0;
    if (t4_iwd_open(&z, iwd) != 0) {
        fprintf(stderr, "t4port: %s: %s\n", iwd, t4_iwd_error());
        return 1;
    }
    printf("%s: %zu entradas, %.1f MB descompactados\n", iwd, t4_iwd_count(&z),
           z.total_uncomp / 1048576.0);
    for (i = 0; i < t4_iwd_count(&z); i++) {
        const T4ZipEntry *e = t4_iwd_at(&z, i);
        if (filter && !strstr(e->name, filter)) continue;
        printf("  %10u  %10u  %s\n", e->uncomp_size, e->comp_size, e->name);
        if (++shown >= 200) {
            printf("  ... (mais %zu; use um filtro)\n", t4_iwd_count(&z) - i - 1);
            break;
        }
    }
    t4_iwd_close(&z);
    return 0;
}

static int cmd_extract(const char *iwd, const char *entry, const char *out_path)
{
    T4Iwd z;
    const T4ZipEntry *e;
    uint8_t *data = NULL;
    size_t len = 0;
    FILE *fp;

    if (t4_iwd_open(&z, iwd) != 0) {
        fprintf(stderr, "t4port: %s: %s\n", iwd, t4_iwd_error());
        return 1;
    }
    e = t4_iwd_find(&z, entry);
    if (!e) {
        fprintf(stderr, "t4port: entrada '%s' nao encontrada\n", entry);
        t4_iwd_close(&z);
        return 1;
    }
    if (t4_iwd_read(&z, e, &data, &len) != 0) {
        fprintf(stderr, "t4port: falha lendo '%s': %s\n", entry, t4_iwd_error());
        t4_iwd_close(&z);
        return 1;
    }
    fp = fopen(out_path, "wb");
    if (!fp) { free(data); t4_iwd_close(&z); return 1; }
    fwrite(data, 1, len, fp);
    fclose(fp);
    printf("extraiu %s (%zu bytes) -> %s\n", entry, len, out_path);
    free(data);
    t4_iwd_close(&z);
    return 0;
}

/* ------------------------------------------------------------------- lan -- */

static void lan_on_query(const char *ip, int sport, void *ud)
{
    const T4LanInfo *in = (const T4LanInfo *)ud;
    printf("  consulta de %s:%d  -> INFO enviado (mapa %s, modo %s)\n",
           ip, sport, in->map, in->mode);
    fflush(stdout);
}

static int cmd_lan(int argc, char **argv)
{
    const char *sub = argc > 2 ? argv[2] : NULL;

    if (!sub || (strcmp(sub, "host") != 0 && strcmp(sub, "discover") != 0)) {
        fprintf(stderr,
                "uso:\n"
                "  t4port lan host [--nome N] [--mapa M] [--modo MD]\n"
                "                  [--porta P] [--jogadores A] [--max B]\n"
                "  t4port lan discover [--espera MS] [--porta P] [--to IP]\n"
                "\n"
                "host      = anfitriao LAN (responde descoberta UDP T4LAN1)\n"
                "discover  = busca anfitrioes na rede (broadcast da Ethernet)\n");
        return 1;
    }

    if (strcmp(sub, "host") == 0) {
        T4LanInfo in;
        char ips[8][16];
        int port = T4LAN_PORT, i, n;

        memset(&in, 0, sizeof(in));
        snprintf(in.name, sizeof(in.name), "t4port");
        snprintf(in.map, sizeof(in.map), "mp_dome");
        snprintf(in.mode, sizeof(in.mode), "dm");
        in.players = 0;
        in.max_players = 8;

        for (i = 3; i < argc; i++) {
            const char *a = argv[i];
            if (!strcmp(a, "--nome") && i + 1 < argc) snprintf(in.name, sizeof(in.name), "%s", argv[++i]);
            else if (!strcmp(a, "--mapa") && i + 1 < argc) snprintf(in.map, sizeof(in.map), "%s", argv[++i]);
            else if (!strcmp(a, "--modo") && i + 1 < argc) snprintf(in.mode, sizeof(in.mode), "%s", argv[++i]);
            else if (!strcmp(a, "--porta") && i + 1 < argc) port = atoi(argv[++i]);
            else if (!strcmp(a, "--jogadores") && i + 1 < argc) in.players = atoi(argv[++i]);
            else if (!strcmp(a, "--max") && i + 1 < argc) in.max_players = atoi(argv[++i]);
            else { fprintf(stderr, "t4port: lan host: opcao desconhecida %s\n", a); return 1; }
        }

        printf("modo LAN ativo (protocolo T4LAN1/UDP, porta %d)\n", port);
        printf("  nome=%s  mapa=%s  modo=%s  jogadores=%d/%d\n",
               in.name, in.map, in.mode, in.players, in.max_players);
        n = t4_lan_local_ips(ips, 8);
        printf("  escutando em:");
        for (i = 0; i < n; i++) printf(" %s", ips[i]);
        printf("%s\n", n ? "" : " (nenhum IPv4 ativo)");
        printf("  Ctrl+C encerra.\n");
        fflush(stdout);

        if (t4_lan_host(&in, port, lan_on_query, &in) != 0) {
            fprintf(stderr, "t4port: %s\n", t4_lan_error());
            return 1;
        }
        printf("encerrado.\n");
        return 0;
    }

    /* discover */
    {
        T4LanHost *hosts = NULL;
        int count = 0, port = T4LAN_PORT, wait = 1500, i;
        const char *to = NULL;

        for (i = 3; i < argc; i++) {
            const char *a = argv[i];
            if (!strcmp(a, "--espera") && i + 1 < argc) wait = atoi(argv[++i]);
            else if (!strcmp(a, "--porta") && i + 1 < argc) port = atoi(argv[++i]);
            else if (!strcmp(a, "--to") && i + 1 < argc) to = argv[++i];
            else { fprintf(stderr, "t4port: lan discover: opcao desconhecida %s\n", a); return 1; }
        }

        if (t4_lan_discover(port, wait, to, &hosts, &count) != 0) {
            fprintf(stderr, "t4port: %s\n", t4_lan_error());
            return 1;
        }
        printf("descoberta em %d ms (porta %d): %d host(s)\n", wait, port, count);
        for (i = 0; i < count; i++)
            printf("  %-21s %-20s %-16s %-8s %-6s %s\n",
                   hosts[i].ip, hosts[i].name, hosts[i].map, hosts[i].mode,
                   hosts[i].players, hosts[i].proto);
        free(hosts);
        return 0;
    }
}

/* ------------------------------------------------------------------ ff -- */

static int cmd_ff(int argc, char **argv)
{
    const char *path = NULL, *filter = NULL, *out_dir = NULL;
    T4FF ff;
    T4FFAsset *assets = NULL;
    size_t count = 0, n_exact = 0, n_skipped = 0;
    int anchor_failed = 0, i;
    uint32_t hist[64];
    size_t k;
    static const char *type_names[] = {
        "xmodelpieces", "physpreset", "physconstraints", "destructibledef",
        "xanimparts", "xmodel", "material", "technique_set", "image", "sound",
        "?10", "clipmap", "clipmap_pvs", "comworld", "gameworld_sp",
        "gameworld_mp", "?16", "gfxworld", "light_def", "?19", "font",
        "menulist", "?22", "localize_entry", "character", "weapondef", "fx",
        "sndglobals", "?28", "?29", "?30", "?31", "rawfile", "stringtable"
    };

    for (i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) out_dir = argv[++i];
        else if (!path) path = argv[i];
        else filter = argv[i];
    }
    if (!path) {
        fprintf(stderr, "t4port: ff precisa de <arquivo.ff>\n");
        return 1;
    }

    if (t4_ff_load(&ff, path) != 0) {
        fprintf(stderr, "t4port: %s: %s\n", path, t4_ff_error());
        return 1;
    }
    if (t4_ff_parse(&ff) != 0) {
        fprintf(stderr, "t4port: %s: %s\n", path, t4_ff_error());
        t4_ff_free(&ff);
        return 1;
    }

    printf("fastfile: %s\n", path);
    printf("  magic              %s\n", T4_FF_MAGIC);
    printf("  versao             %u\n", ff.hdr0);
    printf("  descompactado      %.1f MB\n", ff.len / 1048576.0);
    printf("  size / external    %u / %u\n", ff.size, ff.external_size);
    printf("  blocos             TEMP=%u RUNTIME=%u LARGE_RUNTIME=%u "
           "PHYS_RUNTIME=%u VIRTUAL=%u LARGE=%u PHYSICAL=%u\n",
           ff.blocks[0], ff.blocks[1], ff.blocks[2], ff.blocks[3],
           ff.blocks[4], ff.blocks[5], ff.blocks[6]);
    printf("  script strings     %u\n", ff.string_count);
    printf("  assets             %u\n", ff.asset_count);

    if (t4_ff_walk(&ff, 1, &assets, &count, &n_exact, &n_skipped,
                   &anchor_failed) != 0) {
        fprintf(stderr, "t4port: %s: %s\n", path, t4_ff_error());
        t4_ff_free(&ff);
        return 1;
    }
    printf("  caminhada          %zu exatos / %zu pulados / ancora %s\n",
           n_exact, n_skipped, anchor_failed ? "FALHOU" : "ok");

    memset(hist, 0, sizeof(hist));
    for (k = 0; k < count; k++) {
        uint32_t t = assets[k].type;
        if (t < 64) hist[t]++;
    }
    if (!filter) {
        printf("  por tipo:\n");
        for (k = 0; k < 64; k++)
            if (hist[k])
                printf("    %-16s %zu\n",
                       (k < sizeof(type_names) / sizeof(type_names[0]))
                           ? type_names[k] : "?",
                       (size_t)hist[k]);
    } else {
        for (k = 0; k < count; k++) {
            if (!assets[k].name) continue;
            if (!strstr(assets[k].name, filter)) continue;
            printf("  %3u  %-16s %s", assets[k].index,
                   (assets[k].type < sizeof(type_names) / sizeof(type_names[0]))
                       ? type_names[assets[k].type] : "?",
                   assets[k].name);
            if (assets[k].type == T4_ASSET_RAWFILE)
                printf("  (%zu B)", assets[k].buf_len);
            printf("\n");
        }
    }

    if (out_dir) {
        int written = 0;
        if (t4_assets_extract_rawfiles(&ff, assets, count, out_dir, &written) != 0) {
            fprintf(stderr, "t4port: %s\n", t4_mods_error());
            free(assets);
            t4_ff_free(&ff);
            return 1;
        }
        printf("  extraidos          %d rawfiles -> %s\n", written, out_dir);
    }

    free(assets);
    t4_ff_free(&ff);
    return 0;
}

/* ----------------------------------------------------------------- mods -- */

static int resolve_game_root(char *buf, size_t cap)
{
    static const char *cands[] = {
        NULL,                             /* T4_GAME_DIR (preenchido abaixo) */
        "/app/share/t4port/game",         /* Flatpak */
        "~/Plutonium/Call of Duty World At War", /* copia classica completa (padrao) */
        "~/Plutonium/pluto_t4_full_game", /* fallback de dados */
        "~/.local/share/t4port/game",
        NULL
    };
    const char *env = getenv("T4_GAME_DIR");
    char bdir[1024];
    int i;

    if (env && *env) {
        snprintf(buf, cap, "%s", env);
        return 0;
    }
    if (bundle_game_dir(bdir, sizeof(bdir)) == 0) {
        snprintf(buf, cap, "%s", bdir);
        return 0;
    }
    for (i = 1; cands[i]; i++) {
        char tmp[1024];
        struct stat st;
        const char *c = cands[i];
        /* expande "~/" para o HOME do usuario (antes o "~" era so cortado,
         * gerando /Plutonium/... na raiz do SO e a raiz nunca resolvia) */
        if (c[0] == '~' && (c[1] == '/' || c[1] == '\0')) {
            const char *home = getenv("HOME");
#if defined(_WIN32)
            if (!home || !*home) home = getenv("USERPROFILE");
#endif
            snprintf(tmp, sizeof(tmp), "%s%s",
                     (home && *home) ? home : "", c + 1);
        } else {
            snprintf(tmp, sizeof(tmp), "%s", c);
        }
        if (stat(tmp, &st) == 0 && S_ISDIR(st.st_mode)) {
            snprintf(buf, cap, "%s", tmp);
            return 0;
        }
    }
    return -1;
}

static int cmd_mods(int argc, char **argv)
{
    const char *sub = argc > 2 ? argv[2] : NULL;
    const char *root = NULL, *name = NULL, *rel = NULL;
    char rootbuf[1024];
    int search = 0, i;

    for (i = 3; i < argc; i++) {
        if (strcmp(argv[i], "--root") == 0 && i + 1 < argc) root = argv[++i];
        else if (strcmp(argv[i], "--mod") == 0 && i + 1 < argc) name = argv[++i];
        else if (strcmp(argv[i], "--search") == 0) search = 1;
        else if (argv[i][0] == '-') {
            fprintf(stderr, "t4port: mods: opcao desconhecida %s\n", argv[i]);
            return 1;
        } else if (strcmp(sub, "which") == 0) {
            if (!rel) rel = argv[i];
        } else if (!name) {
            name = argv[i];
        }
    }

    if (!root) {
        if (resolve_game_root(rootbuf, sizeof(rootbuf)) != 0) {
            fprintf(stderr,
                    "t4port: mods: raiz do jogo nao encontrada "
                    "(exporte T4_GAME_DIR=... ou use --root <dir>)\n");
            return 1;
        }
        root = rootbuf;
    }

    if (!sub) {
        fprintf(stderr,
                "uso:\n"
                "  t4port mods init <nome>        cria mods/<nome>/\n"
                "  t4port mods unpack <nome>      extrai TODOS os scripts (.gsc)\n"
                "  t4port mods list               lista os mods\n"
                "  t4port mods which <caminho> [--mod <nome>] [--search]\n"
                "  opcoes: --root <raiz_do_jogo>  (ou T4_GAME_DIR)\n");
        return 1;
    }

    if (strcmp(sub, "init") == 0) {
        if (!name) { fprintf(stderr, "t4port: mods init exige <nome>\n"); return 1; }
        if (t4_mods_init(root, name) != 0) {
            fprintf(stderr, "t4port: %s\n", t4_mods_error());
            return 1;
        }
        printf("mod criado: %s/mods/%s/ (veja MOD.txt)\n", root, name);
        return 0;
    }
    if (strcmp(sub, "unpack") == 0) {
        if (!name) { fprintf(stderr, "t4port: mods unpack exige <nome>\n"); return 1; }
        if (t4_mods_unpack(root, name) != 0) {
            fprintf(stderr, "t4port: %s\n", t4_mods_error());
            return 1;
        }
        return 0;
    }
    if (strcmp(sub, "list") == 0) {
        if (t4_mods_list(root) != 0) {
            fprintf(stderr, "t4port: %s\n", t4_mods_error());
            return 1;
        }
        return 0;
    }
    if (strcmp(sub, "which") == 0) {
        int rc;
        if (!rel) { fprintf(stderr, "t4port: mods which exige <caminho>\n"); return 1; }
        rc = t4_mods_which(root, name, rel, search);
        return rc < 0 ? 1 : (rc > 0 ? 1 : 0);
    }

    fprintf(stderr, "t4port: subcomando mods desconhecido: %s\n", sub);
    return 1;
}

/* ----------------------------------------------------------------- gsc -- */

static char *dupstr(const char *s)
{
    size_t n = strlen(s);
    char *p = (char *)malloc(n + 1);
    if (p) memcpy(p, s, n + 1);
    return p;
}

static int gsc_has_script_suffix(const char *s)
{
    size_t n = strlen(s);
    return n > 4 && (!strcmp(s + n - 4, ".gsc") || !strcmp(s + n - 4, ".csc"));
}

/* Coleta recursivamente todos os .gsc/.csc sob <dir> (libera com free()). */
static int gsc_walk_dir(const char *dir, char ***out, int *n, int *cap)
{
    DIR *d = opendir(dir);
    struct dirent *e;

    if (!d) return -1;
    while ((e = readdir(d)) != NULL) {
        char path[1200];
        struct stat st;

        if (!strcmp(e->d_name, ".") || !strcmp(e->d_name, "..")) continue;
        if (snprintf(path, sizeof(path), "%s/%s", dir, e->d_name) >= (int)sizeof(path))
            continue;
        if (stat(path, &st) != 0) continue;
        if (S_ISDIR(st.st_mode)) {
            gsc_walk_dir(path, out, n, cap);
            continue;
        }
        if (!gsc_has_script_suffix(e->d_name)) continue;
        if (*n >= *cap) {
            *cap = *cap ? *cap * 2 : 512;
            *out = (char **)realloc(*out, (size_t)*cap * sizeof(char *));
            if (!*out) { closedir(d); return -1; }
        }
        (*out)[*n] = dupstr(path);
        if (!(*out)[*n]) { closedir(d); return -1; }
        (*n)++;
    }
    closedir(d);
    return 0;
}

/*
 * t4port gsc <arquivo.gsc>   -> OK/ERRO com linha/coluna + estatisticas
 * t4port gsc <pasta>         -> varre todos os .gsc/.csc (resumo + erros)
 */
static int cmd_gsc(int argc, char **argv)
{
    const char *target = argc > 2 ? argv[2] : NULL;
    struct stat st;

    if (!target) {
        fprintf(stderr, "uso: t4port gsc <arquivo.gsc | pasta>\n");
        return 1;
    }
    if (stat(target, &st) != 0) {
        fprintf(stderr, "t4port: %s: nao encontrado\n", target);
        return 1;
    }

    if (!S_ISDIR(st.st_mode)) {
        T4GSC g;
        if (t4_gsc_parse_file(&g, target) != 0) {
            printf("%s: ERRO linha %d col %d: %s\n",
                   target, g.err_line, g.err_col, g.err);
            return 1;
        }
        printf("%s: OK\n", target);
        printf("  linhas %zu | funcoes %zu | includes %zu | instrucoes %zu | "
               "chamadas %zu\n",
               g.lines, g.funcs, g.includes, g.stmts, g.calls);
        return 0;
    }

    /* diretorio: valida tudo */
    struct { char msg[192]; int n; } hist[64];
    int nh = 0;
    {
        char **files = NULL;
        int n = 0, cap = 0, i, ok = 0, bad = 0, shown = 0, j;

        if (gsc_walk_dir(target, &files, &n, &cap) != 0 || n == 0) {
            fprintf(stderr, "t4port: gsc: nenhum .gsc/.csc em %s\n", target);
            free(files);
            return 1;
        }
        printf("scripts em %s:\n", target);
        for (i = 0; i < n; i++) {
            T4GSC g;
            if (t4_gsc_parse_file(&g, files[i]) == 0) {
                ok++;
            } else {
                bad++;
                /* histograma de mensagens: mostra as classes de erro */
                for (j = 0; j < nh; j++)
                    if (!strcmp(hist[j].msg, g.err)) break;
                if (j < nh) {
                    hist[j].n++;
                } else if (nh < 64) {
                    snprintf(hist[nh].msg, sizeof(hist[nh].msg), "%s", g.err);
                    hist[nh].n = 1;
                    nh++;
                } else {
                    hist[63].n++;
                }
                if (shown < 10) {
                    printf("  ERRO %s:%d:%d: %s\n",
                           files[i], g.err_line, g.err_col, g.err);
                    shown++;
                }
            }
            free(files[i]);
        }
        free(files);
        if (nh > 0) {
            printf("  erros por tipo:\n");
            for (j = 0; j < nh && j < 12; j++)
                printf("    %-45s %d\n", hist[j].msg, hist[j].n);
        }
        printf("total %d | ok %d | erros %d%s\n",
               n, ok, bad, bad > shown ? " (mostrando os 10 primeiros)" : "");
        return bad ? 1 : 0;
    }
}

/* ------------------------------------------------------------------ iwi -- */

static int load_iwi_bytes(const char *path, uint8_t **out, size_t *len)
{
    FILE *fp = fopen(path, "rb");
    long sz;
    if (!fp) return -1;
    fseek(fp, 0, SEEK_END);
    sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (sz <= 0) { fclose(fp); return -1; }
    *out = (uint8_t *)malloc((size_t)sz);
    if (!*out) { fclose(fp); return -1; }
    if (fread(*out, 1, (size_t)sz, fp) != (size_t)sz) { free(*out); fclose(fp); return -1; }
    fclose(fp);
    *len = (size_t)sz;
    return 0;
}

static int load_iwi_from_iwd(const char *iwd, const char *entry, uint8_t **out, size_t *len)
{
    T4Iwd z;
    const T4ZipEntry *e;
    int rc;
    if (t4_iwd_open(&z, iwd) != 0) return -1;
    e = t4_iwd_find(&z, entry);
    if (!e) { t4_iwd_close(&z); return -1; }
    rc = t4_iwd_read(&z, e, out, len);
    t4_iwd_close(&z);
    return rc;
}

static int cmd_iwi(const char *path, const char *png_out)
{
    uint8_t *raw = NULL, *rgba = NULL;
    size_t len = 0;
    T4IwiInfo info;
    int w = 0, h = 0;

    if (load_iwi_bytes(path, &raw, &len) != 0) {
        fprintf(stderr, "t4port: nao consegui ler %s\n", path);
        return 1;
    }
    if (t4_iwi_parse(raw, len, &info) < 0) {
        fprintf(stderr, "t4port: %s: %s\n", path, t4_iwi_error());
        free(raw);
        return 1;
    }
    printf("%s\n  versao=%d formato=%d flags=0x%02x  %dx%d  campo+0x0A=%d\n"
           "  payload=%zu bytes (nivel 0: %s)\n",
           path, info.version, info.format, info.flags, info.width, info.height,
           info.field_0x0A, info.payload_len,
           info.format == T4_IWI_FORMAT_DXT1 ? "DXT1/BC1" :
           info.format == T4_IWI_FORMAT_DXT5 ? "DXT5/BC3" :
           info.format == T4_IWI_FORMAT_DXN  ? "DXN/BC5 (mapa normal)" :
           info.format == T4_IWI_FORMAT_BGRA8 ? "BGRA8" :
           info.format == T4_IWI_FORMAT_A8L8 ? "A8L8" :
           info.format == T4_IWI_FORMAT_L8   ? "L8" : "nao identificado");

    if (t4_iwi_decode_rgba(raw, len, &rgba, &w, &h) != 0) {
        fprintf(stderr, "t4port: decodificacao: %s\n", t4_iwi_error());
        free(raw);
        return 1;
    }
    if (png_out) {
        if (t4_write_png(png_out, rgba, w, h) == 0)
            printf("  -> %s (%dx%d RGBA)\n", png_out, w, h);
        else {
            fprintf(stderr, "t4port: falha escrevendo %s\n", png_out);
            free(rgba); free(raw);
            return 1;
        }
    }
    free(rgba);
    free(raw);
    return 0;
}

/* ---------------------------------------------------------------- view -- */

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define T4_PIXELFORMAT_RGBA32 0x16762004u /* SDL_PIXELFORMAT_ABGR8888 */
#else
#define T4_PIXELFORMAT_RGBA32 0x16442004u /* SDL_PIXELFORMAT_BGRA8888 */
#endif

static int run_viewer(uint8_t *raw, size_t len, const char *title)
{
    uint8_t *rgba = NULL;
    int w = 0, h = 0;
    T4_SDL_Window *win;
    T4_SDL_Renderer *ren;
    T4_SDL_Texture *tex;
    T4_SDLEvent ev;
    int running = 1;

    if (t4_iwi_decode_rgba(raw, len, &rgba, &w, &h) != 0) {
        fprintf(stderr, "t4port: %s\n", t4_iwi_error());
        return 1;
    }

    if (t4_sdl_load() != 0) {
        fprintf(stderr, "t4port: SDL2 indisponivel: %s\n", t4_sdl_error());
        free(rgba);
        return 1;
    }
    if (t4_SDL_Init(T4_SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "t4port: SDL_Init: %s\n", t4_SDL_GetError());
        free(rgba);
        return 1;
    }

    win = t4_SDL_CreateWindow(title ? title : "t4port", T4_SDL_WINDOWPOS_CENTERED,
                              T4_SDL_WINDOWPOS_CENTERED, 1280, 720, 0);
    if (!win) {
        fprintf(stderr, "t4port: SDL_CreateWindow: %s\n", t4_SDL_GetError());
        t4_SDL_Quit();
        free(rgba);
        return 1;
    }
    ren = t4_SDL_CreateRenderer(win, -1, 0);
    if (!ren) {
        fprintf(stderr, "t4port: SDL_CreateRenderer: %s\n", t4_SDL_GetError());
        t4_SDL_DestroyWindow(win);
        t4_SDL_Quit();
        free(rgba);
        return 1;
    }
    tex = t4_SDL_CreateTexture(ren, T4_PIXELFORMAT_RGBA32, T4_SDL_TEXTUREACCESS_STATIC, w, h);
    if (!tex) {
        fprintf(stderr, "t4port: SDL_CreateTexture: %s\n", t4_SDL_GetError());
        t4_SDL_DestroyRenderer(ren);
        t4_SDL_DestroyWindow(win);
        t4_SDL_Quit();
        free(rgba);
        return 1;
    }
    t4_SDL_UpdateTexture(tex, NULL, rgba, w * 4);
    printf("renderizando %dx%d - ESC ou fechar a janela para sair\n", w, h);

    while (running) {
        T4_SDL_Rect dst;
        while (t4_SDL_PollEvent(&ev)) {
            if (ev.type == T4_SDL_QUIT) running = 0;
            if (ev.type == T4_SDL_KEYDOWN && ev.raw[16] == T4_SDLK_ESCAPE) running = 0;
        }
        t4_SDL_SetRenderDrawColor(ren, 24, 24, 28, 255);
        t4_SDL_RenderClear(ren);
        /* mantem a proporcao da textura na janela */
        {
            float wa = (float)w / (float)h;
            float ww = 1280.0f, wh = 720.0f;
            if (ww / wh > wa) ww = wh * wa; else wh = ww / wa;
            dst.x = (int)((1280.0f - ww) / 2.0f);
            dst.y = (int)((720.0f - wh) / 2.0f);
            dst.w = (int)ww;
            dst.h = (int)wh;
        }
        t4_SDL_RenderCopy(ren, tex, NULL, &dst);
        t4_SDL_RenderPresent(ren);
        t4_SDL_Delay(16);
    }

    t4_SDL_DestroyTexture(tex);
    t4_SDL_DestroyRenderer(ren);
    t4_SDL_DestroyWindow(win);
    t4_SDL_Quit();
    t4_sdl_unload();
    free(rgba);
    return 0;
}

static int cmd_view_iwi(const char *path)
{
    uint8_t *raw = NULL;
    size_t len = 0;
    int rc;
    if (load_iwi_bytes(path, &raw, &len) != 0) {
        fprintf(stderr, "t4port: nao consegui ler %s\n", path);
        return 1;
    }
    rc = run_viewer(raw, len, path);
    free(raw);
    return rc;
}

static int cmd_view_iwd(const char *iwd, const char *entry)
{
    uint8_t *raw = NULL;
    size_t len = 0;
    int rc;
    if (load_iwi_from_iwd(iwd, entry, &raw, &len) != 0) {
        fprintf(stderr, "t4port: '%s' em %s nao encontrado/ilegivel\n", entry, iwd);
        return 1;
    }
    rc = run_viewer(raw, len, entry);
    free(raw);
    return rc;
}

/* --------------------------------------------------------------- video -- */
/*
 * Videos do jogo sao Bink (.bik). O binario e proprietario, mas o ffmpeg ja
 * traz os decoders binkvideo + binkaudio (dct/rdft), entao o porte usa o
 * ffmpeg como backend de video em vez de reimplementar o codec.
 */
#if defined(_WIN32)
/* Windows nao tem fork/exec; mingw traz _spawnvp(). */
#include <process.h>
#include <direct.h>

static int run_argv(char *const argv[])
{
    intptr_t r;
    if (!argv || !argv[0]) return -1;
    r = _spawnvp(_P_WAIT, argv[0], (const char *const *)argv);
    if (r < 0) return -1;
    return (int)r;
}

static int have_tool(const char *name)
{
    /* Sem fork para suprimir a saida: assume presente e deixa run_argv
     * falhar graciosamente se o ffmpeg nao estiver no PATH. */
    (void)name;
    return 1;
}
#else
#include <sys/wait.h>

static int run_argv(char *const argv[])
{
    pid_t pid;
    int st = 0;

    if (!argv || !argv[0]) return -1;
    pid = fork();
    if (pid < 0) return -1;
    if (pid == 0) {
        execvp(argv[0], argv);
        _exit(127);
    }
    if (waitpid(pid, &st, 0) < 0) return -1;
    if (WIFEXITED(st)) return WEXITSTATUS(st);
    return -1;
}

static int have_tool(const char *name)
{
    pid_t pid;
    int st = 0;

    pid = fork();
    if (pid == 0) {
        int devnull = open("/dev/null", O_WRONLY);
        if (devnull >= 0) { dup2(devnull, 1); dup2(devnull, 2); close(devnull); }
        execlp(name, name, "-version", (char *)NULL);
        _exit(127);
    }
    if (pid < 0) return 0;
    waitpid(pid, &st, 0);
    return WIFEXITED(st) && WEXITSTATUS(st) == 0;
}
#endif

/* --------------------------------------------------------------- play -- */
/*
 * Inicia o cliente retail classico (CoDWaW.exe/CoDWaWmp.exe) da copia local
 * do jogo. No Windows isso roda NATIVAMENTE (o retail e um PE32 do proprio
 * Windows; o duplo clique no t4port.exe abre o jogo). Em Linux/macOS o
 * binario nativo nao e o jogo completo - o caminho do jogo la e o launcher
 * ./t4port.sh play (ponte Wine) - entao aqui so apontamos pra ele.
 */
static int cmd_play(const char *gd)
{
    static const char *exes[] = { "CoDWaW.exe", "CoDWaWmp.exe", NULL };
    const char *dir = (gd && *gd) ? gd : NULL;
    char bdir[1024], path[1400];
    int i;
    FILE *f = NULL;

    if (!dir) {
        const char *env = getenv("T4_GAME_DIR");
        if (env && *env) dir = env;
        else if (bundle_game_dir(bdir, sizeof(bdir)) == 0) dir = bdir;
    }
    if (!dir) {
        fprintf(stderr,
                "t4port play: game folder not found (set T4_GAME_DIR=<folder>)\n");
        return 1;
    }
    path[0] = '\0';
    for (i = 0; exes[i]; i++) {
        snprintf(path, sizeof(path), "%s/%s", dir, exes[i]);
        f = fopen(path, "rb");
        if (f) { fclose(f); break; }
        path[0] = '\0';
    }
    if (!path[0]) {
        fprintf(stderr,
                "t4port play: no classic executable (CoDWaW.exe/CoDWaWmp.exe) "
                "in '%s'\n", dir);
        return 1;
    }
#if defined(_WIN32)
    /* o motor le os dados ao lado do exe: roda com cwd na pasta do jogo */
    if (_chdir(dir) != 0) {
        fprintf(stderr, "t4port play: could not enter '%s'\n", dir);
        return 1;
    }
    {
        char *play_argv[2];
        play_argv[0] = path;
        play_argv[1] = NULL;
        return run_argv(play_argv);
    }
#else
    fprintf(stderr,
        "t4port play: the classic client starts through the launcher here:\n"
        "  ./t4port.sh play   (Wine bridge on Linux/macOS; the native engine\n"
        "                      still has no game loop - see roadmap)\n"
        "  on Windows, t4port.exe / 't4port play' starts it natively.\n");
    return 1;
#endif
}

static int run_transcode(const char *path, const char *out, int h264)
{
    if (h264) {
        char *const argv[] = {
            (char *)"ffmpeg", (char *)"-y", (char *)"-v", (char *)"error",
            (char *)"-i", (char *)path,
            (char *)"-c:v", (char *)"libx264", (char *)"-pix_fmt", (char *)"yuv420p",
            (char *)"-crf", (char *)"20", (char *)"-preset", (char *)"medium",
            (char *)"-c:a", (char *)"aac", (char *)"-movflags", (char *)"+faststart",
            (char *)out, NULL
        };
        return run_argv(argv);
    }
    {
        /* fallback compativel com qualquer build do ffmpeg (sem libx264) */
        char *const argv[] = {
            (char *)"ffmpeg", (char *)"-y", (char *)"-v", (char *)"error",
            (char *)"-i", (char *)path,
            (char *)"-c:v", (char *)"mpeg4", (char *)"-q:v", (char *)"3",
            (char *)"-c:a", (char *)"aac", (char *)"-movflags", (char *)"+faststart",
            (char *)out, NULL
        };
        return run_argv(argv);
    }
}

static int cmd_video(const char *path, const char *out_mp4, int play)
{
    const char *need = out_mp4 ? "ffmpeg" : (play ? "ffplay" : "ffprobe");

    if (!have_tool(need)) {
        fprintf(stderr, "t4port: '%s' ausente - instale o ffmpeg para rodar Bink (.bik)\n", need);
        return 1;
    }

    if (out_mp4) {
        printf("transcodificando %s -> %s\n", path, out_mp4);
        if (run_transcode(path, out_mp4, 1) == 0) return 0;
        remove(out_mp4);
        printf("libx264 indisponivel; usando MPEG-4 Part 2\n");
        return run_transcode(path, out_mp4, 0) == 0 ? 0 : 1;
    }

    if (play) {
        char *const argv[] = { (char *)"ffplay", (char *)"-v", (char *)"error",
                               (char *)"-autoexit", (char *)path, NULL };
        return run_argv(argv) == 0 ? 0 : 1;
    }

    {
        char *const argv[] = {
            (char *)"ffprobe", (char *)"-v", (char *)"error",
            (char *)"-show_entries",
            (char *)"stream=codec_name,width,height,avg_frame_rate,duration:format=duration,size",
            (char *)"-of", (char *)"default=noprint_wrappers=1",
            (char *)path, NULL
        };
        printf("video: %s\n", path);
        return run_argv(argv) == 0 ? 0 : 1;
    }
}

/* ----------------------------------------------------------------- api -- */
/*
 * Opcoes de video do porte: qual API grafica usar. A escolha fica gravada em
 * t4port.cfg (r_api=...) e vale para o launcher/renderer quando o motor 3D
 * estiver pronto; `t4port gfx` ja mostra a API funcionando numa janela.
 */
static int cmd_api(int argc, char **argv)
{
    char lines[8][128];
    char path[1100];
    int n, i;

    if (argc >= 3) {
        T4GfxApi a;
        if (t4_gfx_api_from_name(argv[2], &a) != 0) {
            fprintf(stderr, "t4port: API desconhecida '%s' "
                    "(auto|opengl|vulkan|opengles)\n", argv[2]);
            return 1;
        }
        if (t4_gfx_config_set(a) != 0) {
            fprintf(stderr, "t4port: %s\n", t4_gfx_error());
            return 1;
        }
        printf("API grafica definida: %s\n", t4_gfx_api_name(a));
    }

    t4_gfx_config_path(path, sizeof(path));
    printf("configuracao grafica (%s)\n"
           "  atual: r_api=%s\n", path, t4_gfx_api_name(t4_gfx_config_get()));
    n = t4_gfx_detect(lines, 8);
    for (i = 0; i < n; i++) printf("  %s\n", lines[i]);
    printf("dica: 't4port gfx' abre a janela de teste com a API escolhida\n");
    return 0;
}

/* ----------------------------------------------------------------- gfx -- */

static int cmd_gfx(int argc, char **argv)
{
    T4GfxApi a = t4_gfx_config_get();
    int seconds = 0, w = 0, h = 0, i;

    for (i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--api") == 0 && i + 1 < argc) {
            if (t4_gfx_api_from_name(argv[++i], &a) != 0) {
                fprintf(stderr, "t4port: gfx: API desconhecida '%s'\n", argv[i]);
                return 1;
            }
        } else if (strcmp(argv[i], "--segundos") == 0 && i + 1 < argc) {
            seconds = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--janela") == 0 && i + 2 < argc) {
            w = atoi(argv[++i]);
            h = atoi(argv[++i]);
        } else {
            fprintf(stderr, "uso: t4port gfx [--api auto|opengl|vulkan|opengles] "
                    "[--segundos N] [--janela W H]\n");
            return 1;
        }
    }
    /* default de janela: r_mode do cfg (qualquer WxH); --janela tem prioridade */
    if (w < 64 || h < 64) t4_gfx_window_size(&w, &h);
    if (w < 64) w = 64;
    if (h < 64) h = 64;
    printf("gfx: janela %dx%d (r_mode do cfg; --janela sobrepoe)\n", w, h);

    if (t4_gfx_run(a, w, h, seconds) != 0) {
        fprintf(stderr, "t4port: %s\n", t4_gfx_error());
        return 1;
    }
    return 0;
}

/* --------------------------------------------------------------- mundo -- */

static int cmd_world(int argc, char **argv)
{
    const char *mapa = NULL, *shot = NULL;
    T4GfxApi a = t4_gfx_config_get();
    int seconds = 0, w = 1024, h = 768, i, wire = 0, walk_only = 0;
    int api_explicit = 0;
    char path[2048];
    T4FF ff;
    T4World wv;
    char err[512], info[256];

    memset(&ff, 0, sizeof(ff));
    memset(&wv, 0, sizeof(wv));
    for (i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--api") == 0 && i + 1 < argc) {
            if (t4_gfx_api_from_name(argv[++i], &a) != 0) {
                fprintf(stderr, "t4port: world: API desconhecida '%s'\n", argv[i]);
                return 1;
            }
            api_explicit = 1;
        } else if (strcmp(argv[i], "--segundos") == 0 && i + 1 < argc) {
            seconds = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--janela") == 0 && i + 2 < argc) {
            w = atoi(argv[++i]);
            h = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--shot") == 0 && i + 1 < argc) {
            shot = argv[++i];
        } else if (strcmp(argv[i], "--wire") == 0) {
            wire = 1;
        } else if (strcmp(argv[i], "--walk-only") == 0) {
            walk_only = 1;
        } else if (!mapa) {
            mapa = argv[i];
        } else {
            fprintf(stderr, "uso: t4port world <mapa | arquivo.ff> "
                    "[--api auto|opengl|vulkan|opengles] [--segundos N] "
                    "[--janela W H] [--shot saida.png] [--wire] [--walk-only]\n");
            return 1;
        }
    }
    if (!mapa) {
        fprintf(stderr, "uso: t4port world <mapa | arquivo.ff> "
                "[--api auto|opengl|vulkan|opengles] [--segundos N] "
                "[--janela W H] [--shot saida.png] [--wire] [--walk-only]\n");
        return 1;
    }
    if (w < 64) w = 64;
    if (h < 64) h = 64;

    /* resolve o caminho: direto, ou o mapa no zone/ dos dados do jogo */
    if (strchr(mapa, '/') || has_suffix(mapa, ".ff")) {
        snprintf(path, sizeof(path), "%s", mapa);
    } else {
        const char *gd = getenv("T4_GAME_DIR");
        char bdir[1024];
        char cand[2048];
        int found = 0;
        if (!gd || !*gd) {
            if (resolve_game_root(bdir, sizeof(bdir)) == 0) gd = bdir;
        }
        if (!gd || !*gd) {
            fprintf(stderr, "t4port: world: defina T4_GAME_DIR ou passe o caminho do .ff\n");
            return 1;
        }
        snprintf(cand, sizeof(cand), "%s/zone/english/%s.ff", gd, mapa);
        if (access(cand, R_OK) == 0) {
            snprintf(path, sizeof(path), "%s", cand);
            found = 1;
        } else {
            snprintf(cand, sizeof(cand), "%s/zone/%s.ff", gd, mapa);
            if (access(cand, R_OK) == 0) {
                snprintf(path, sizeof(path), "%s", cand);
                found = 1;
            }
        }
        if (!found) {
            fprintf(stderr, "t4port: world: mapa '%s' nao encontrado em %s/zone/\n",
                    mapa, gd);
            return 1;
        }
    }

    if (t4_ff_load(&ff, path) != 0) {
        fprintf(stderr, "t4port: world: %s: %s\n", path, t4_ff_error());
        return 1;
    }
    if (t4_ff_parse(&ff) != 0) {
        fprintf(stderr, "t4port: world: %s: %s\n", path, t4_ff_error());
        t4_ff_free(&ff);
        return 1;
    }
    if (t4_world_load(&wv, &ff, err, sizeof(err)) != 0) {
        fprintf(stderr, "t4port: %s\n", err);
        t4_ff_free(&ff);
        return 1;
    }
    printf("mundo: %s   (%s)\n", wv.name, path);
    printf("  vertices: %u   indices: %u   triangulos: %u   surfaces: %d\n",
           wv.nverts, wv.nidx, wv.nidx / 3, wv.surfaces);
    printf("  limites: [%.0f %.0f %.0f] .. [%.0f %.0f %.0f]\n",
           wv.mins[0], wv.mins[1], wv.mins[2],
           wv.maxs[0], wv.maxs[1], wv.maxs[2]);
    printf("  walker estruturado: %s\n",
           wv.walk_exact ? "EXATO (terminou no fim do payload)"
                         : "parcial (GfxWorld completo)");
    if (wv.walk_err[0])
        printf("  aviso pos-GfxWorld: %s\n", wv.walk_err);
    fflush(stdout);

    if (walk_only) {
        t4_world_free(&wv);
        t4_ff_free(&ff);
        return 0;
    }

    if (a == T4_GFX_VULKAN) {
        /* --api vulkan explicito: erro honesto (a fase Vulkan ainda nao
         * existe). Sem a flag (api da config): o usuario so quer ver o
         * mundo - segue com OpenGL e avisa. */
        if (api_explicit) {
            fprintf(stderr, "t4port: world: renderizador de mundo ainda e "
                            "OpenGL/ES (Vulkan e a fase seguinte) - "
                            "use --api opengl\n");
            t4_world_free(&wv);
            t4_ff_free(&ff);
            return 1;
        }
        fprintf(stderr, "t4port: world: api 'vulkan' (configurada) ainda nao "
                        "tem renderizador de mundo - seguindo com OpenGL\n");
        a = T4_GFX_OPENGL;
    }
    if (t4_gfx_gl_world_run(w, h, seconds, a == T4_GFX_ES ? 1 : 0,
                            wv.pos, wv.nrm, wv.nverts, wv.idx, wv.nidx,
                            wv.mins, wv.maxs, shot, wire,
                            info, sizeof(info)) != 0) {
        fprintf(stderr, "t4port: %s\n", info);
        t4_world_free(&wv);
        t4_ff_free(&ff);
        return 1;
    }
    printf("%s\n", info);
    t4_world_free(&wv);
    t4_ff_free(&ff);
    return 0;
}

/* ---------------------------------------------------------------- check -- */

static int count_ext_r(const char *dir, const char *ext, int depth)
{
    DIR *d = opendir(dir);
    struct dirent *e;
    int n = 0;
    if (!d) return -1;
    while ((e = readdir(d))) {
        char sub[1200];
        int m;
        if (e->d_name[0] == '.') continue;        /* . .. e escondidos */
        if (has_suffix(e->d_name, ext)) n++;
        if (depth <= 0) continue;
        snprintf(sub, sizeof(sub), "%s/%s", dir, e->d_name);
        m = count_ext_r(sub, ext, depth - 1);     /* -1 se nao for dir */
        if (m > 0) n += m;
    }
    closedir(d);
    return n;
}

/* conta com recursao: o disco retail guarda os .ff em zone/<idioma>/ */
static int count_ext(const char *dir, const char *ext)
{
    return count_ext_r(dir, ext, 2);
}

static int cmd_check(const char *argv0)
{
    char bdir[1024];
    const char *gd = getenv("T4_GAME_DIR");
    int sdl_rc;

    printf("t4port %s (build nativo)\n"
           "  binario            %s\n"
           "  zlib               %s\n"
           "  SDL2               %s\n",
           "0.1.0",
           argv0 ? argv0 : "?",
           zlibVersion(),
#ifdef T4PORT_SDL2_LINKED
           "linkada em tempo de build");
#else
           "binding dinamico (dlopen)");
#endif

    sdl_rc = t4_sdl_load();
    if (sdl_rc == 0)
        printf("  SDL2 status         carregada\n");
    else
        printf("  SDL2 status         INDISPONIVEL: %s\n", t4_sdl_error());
    if (sdl_rc == 0) t4_sdl_unload();

    {
        /* APIs graficas disponiveis (usa a linha 0 do detect = SDL2 ja acima) */
        char glines[8][128];
        int gn = t4_gfx_detect(glines, 8), gi;
        for (gi = 1; gi < gn; gi++) printf("  %s\n", glines[gi]);
    }

    {
        /* placas (qualquer fabricante) + driver NVIDIA */
        T4Gpu gp;
        t4_gapi_gpu(&gp);
        printf("  placas (PCI)       %d NVIDIA / %d AMD / %d Intel%s\n",
               gp.nvidia, gp.amd, gp.intel,
               gp.nouveau ? "  | nouveau em uso" : "");
        if (gp.nvidia || gp.nv_gl[0] || gp.nv_module[0]) {
            printf("  driver NVIDIA      modulo %s | GL %s | Vulkan %s | "
                   "ICD %s\n",
                   gp.nv_module[0] ? gp.nv_module : "?",
                   gp.nv_gl[0] ? gp.nv_gl : "ausente",
                   gp.nv_vk > 0 ? "sim" : (gp.nv_vk == 0 ? "nao" : "?"),
                   gp.nv_icd ? "ativo" : "AUSENTE (t4port gpu --fix)");
        }
    }

    if (!gd || !*gd) {
        /* usa o mesmo resolvedor do comando mods (T4_GAME_DIR, bundle,
         * Flatpak, ~/Plutonium/pluto_t4_full_game, ~/.local/share) */
        if (resolve_game_root(bdir, sizeof(bdir)) == 0) gd = bdir;
    }
    if (gd && *gd) {
        char p[1200];
        int ni, nf;
        printf("  dados do jogo      %s\n", gd);
        snprintf(p, sizeof(p), "%s/main", gd);
        ni = count_ext(p, ".iwd");
        snprintf(p, sizeof(p), "%s/zone", gd);
        nf = count_ext(p, ".ff");
        if (ni >= 0) printf("    .iwd             %d\n", ni);
        if (nf >= 0) printf("    fastfiles        %d (recursivo, inclui zone/<idioma>/)\n", nf);
    } else {
        printf("  dados do jogo      NAO ENCONTRADOS (exporte T4_GAME_DIR=...)\n");
    }
    return 0;
}

/* ---------------------------------------------------------------- main -- */

/* Caminho padrao dos dados quando o binario roda como bundle (.app no macOS).
 * Devolve <diretorio_do_executavel>/../Resources/game se existir. */
static int bundle_game_dir(char *buf, size_t cap)
{
    char exe[1024];
    char *slash;

    if (t4_exe_path(exe, sizeof(exe)) != 0) return -1;

    slash = strrchr(exe, '/');
#ifdef _WIN32
    {
        char *b = strrchr(exe, '\\');
        if (b && (!slash || b > slash)) slash = b;
    }
#endif
    if (slash) *slash = '\0';
    snprintf(buf, cap, "%s/../Resources/game", exe);

    {
        struct stat st;
        if (stat(buf, &st) == 0 && S_ISDIR(st.st_mode)) return 0;
    }
    return -1;
}

int main(int argc, char **argv)
{
    t4_gapi_apply_gpu_env();   /* T4_GPU=nvidia|amd|intel: PRIME/ICD desde ja */
#if !defined(_WIN32)
    static char *default_argv[3];
#endif

    /* o Finder do macOS injeta "-psn_0_XXXX" ao abrir um bundle .app */
    if (argc > 1 && strncmp(argv[1], "-psn_", 5) == 0) {
        int i;
        for (i = 1; i + 1 < argc; i++) argv[i] = argv[i + 1];
        argc--;
    }

    if (argc > 1 && (strcmp(argv[1], "check") == 0 || strcmp(argv[1], "--check") == 0))
        return cmd_check(argv[0]);

    if (argc < 3) {
        if (argc < 2) {
            /* sem argumentos: no Windows o duplo clique no .exe INICIA O
             * JOGO (retail nativo); no Linux/macOS quem faz isso e o
             * launcher (.sh/.desktop) - aqui, com dados do jogo, mostra o
             * inventario, e sem dados a ajuda. Nunca tocar em argv[1] com
             * argc==1: argv[1] e nulo (causava segfault, RC=139) */
#if defined(_WIN32)
            return cmd_play(NULL);
#else
            {
                const char *gd = getenv("T4_GAME_DIR");
                char bdir[1024];
                if (!gd || !*gd) {
                    if (bundle_game_dir(bdir, sizeof(bdir)) == 0) gd = bdir;
                }
                if (gd && *gd) {
                    default_argv[0] = argv[0];
                    default_argv[1] = (char *)"info";
                    default_argv[2] = (char *)gd;
                    argv = default_argv;
                    argc = 3;
                } else {
                    usage();
                    return 1;
                }
            }
#endif
        } else {
            /* subcomando sozinho (ex.: "t4port gsc"): deixa o proprio
             * comando mostrar a ajuda dele em vez da usage generica.
             * Só os que leem argv com guarda (argc > 2) entram aqui. */
            static const char *own_usage[] = { "lan", "mods", "gsc", "ff",
                                               "api", "gfx", "world", "menu",
                                               "gpu", "play", NULL };
            int k, known = 0;
            for (k = 0; own_usage[k]; k++)
                if (strcmp(argv[1], own_usage[k]) == 0) { known = 1; break; }
            if (!known) {
                usage();
                return argc > 1 && strcmp(argv[1], "help") == 0 ? 0 : 1;
            }
        }
    }

    if (strcmp(argv[1], "info") == 0) return cmd_info(argv[2]);

    if (strcmp(argv[1], "zip") == 0)
        return cmd_zip(argv[2], argc > 3 ? argv[3] : NULL);

    if (strcmp(argv[1], "ff") == 0) return cmd_ff(argc, argv);

    if (strcmp(argv[1], "mods") == 0) return cmd_mods(argc, argv);

    if (strcmp(argv[1], "gsc") == 0) return cmd_gsc(argc, argv);

    if (strcmp(argv[1], "iwi") == 0) {
        const char *out = NULL;
        int i;
        for (i = 3; i < argc; i++)
            if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) out = argv[++i];
        return cmd_iwi(argv[2], out);
    }

    if (strcmp(argv[1], "view") == 0) {
        if (strcmp(argv[2], "--iwd") == 0) {
            if (argc < 5) { usage(); return 1; }
            return cmd_view_iwd(argv[3], argv[4]);
        }
        return cmd_view_iwi(argv[2]);
    }

    if (strcmp(argv[1], "video") == 0) {
        const char *out = NULL;
        int play = 0, i;
        for (i = 3; i < argc; i++) {
            if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) out = argv[++i];
            else if (strcmp(argv[i], "--play") == 0) play = 1;
        }
        return cmd_video(argv[2], out, play);
    }

    if (strcmp(argv[1], "extract") == 0) {
        const char *out = NULL;
        int i;
        for (i = 4; i < argc; i++)
            if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) out = argv[++i];
        if (!out) { fprintf(stderr, "t4port: extract exige -o <saida>\n"); return 1; }
        return cmd_extract(argv[2], argv[3], out);
    }

    if (strcmp(argv[1], "lan") == 0) return cmd_lan(argc, argv);

    if (strcmp(argv[1], "api") == 0) return cmd_api(argc, argv);
    if (strcmp(argv[1], "menu") == 0) return t4_menu_cmd(argc, argv);
    if (strcmp(argv[1], "play") == 0)
        return cmd_play(argc > 2 ? argv[2] : NULL);
    if (strcmp(argv[1], "gpu") == 0) return t4_gpu_cmd(argc, argv);

    if (strcmp(argv[1], "gfx") == 0) return cmd_gfx(argc, argv);

    if (strcmp(argv[1], "world") == 0) return cmd_world(argc, argv);

    usage();
    return 1;
}
