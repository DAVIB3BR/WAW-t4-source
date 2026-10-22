/*
 * mods.c - extracao de rawfiles e gerenciamento de mods (ver mods.h).
 *
 * Caminho dos dados: os .gsc/.csc do jogo sao assets "rawfile" (tipo 32)
 * dentro dos .ff; o parser de ff.c ja os entrega com exatidao. Aqui eles sao
 * gravados em <raiz>/mods/<nome>/ para edicao, e consultados na ordem
 * overlay -> original.
 */
#include "mods.h"

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#if defined(_WIN32)
#include <direct.h>
#define t4_mkdir(p) _mkdir(p)
#else
#define t4_mkdir(p) mkdir((p), 0755)
#endif

static char g_merr[512];

const char *t4_mods_error(void) { return g_merr; }

static int mfail(const char *msg)
{
    snprintf(g_merr, sizeof(g_merr), "%s%s%s", msg ? msg : "",
             (msg && errno) ? " " : "", errno ? strerror(errno) : "");
    errno = 0;
    return -1;
}

/* Erro LOGICO (nao vindo de syscall): nao herda errno obsoleto. */
static int mset(const char *msg)
{
    snprintf(g_merr, sizeof(g_merr), "%s", msg ? msg : "erro");
    errno = 0;
    return -1;
}

/* ------------------------------------------------------------------ fs -- */

static int is_dir(const char *p)
{
    struct stat st;
    return stat(p, &st) == 0 && S_ISDIR(st.st_mode);
}

static int is_file(const char *p)
{
    struct stat st;
    return stat(p, &st) == 0 && S_ISREG(st.st_mode);
}

/* mkdir -p sobre um caminho de diretorio (ou o pai, se terminar em arquivo). */
static int mkdir_p(const char *path)
{
    char buf[1024];
    size_t i, n;

    n = strlen(path);
    if (n == 0 || n >= sizeof(buf)) return -1;
    memcpy(buf, path, n + 1);
    for (i = 1; i <= n; i++) {
        if (buf[i] != '/' && buf[i] != '\0') continue;
        char save = buf[i];
        buf[i] = '\0';
        if (t4_mkdir(buf) != 0 && errno != EEXIST) return -1;
        buf[i] = save;
    }
    return 0;
}

/* Caminho relativo seguro (sem "..", sem "/", inicial, sem '\'). */
static int safe_rel(const char *p)
{
    size_t i, len;
    if (!p || !*p) return 0;
    len = strlen(p);
    if (len > 511 || p[0] == '/' || p[0] == '\\') return 0;
    if (strstr(p, "..") != NULL) return 0;
    for (i = 0; i < len; i++) {
        unsigned char c = (unsigned char)p[i];
        /* Só ASCII imprimível: nomes com bytes >= 0x80 que observamos vêm de
         * lixo no meio de outros assets, não são caminhos reais do jogo, e
         * quebram UTF-8 (fatia o build do Flatpak e ferramentas de arquivo). */
        if (c < 32 || c > 126 || c == '\\') return 0;
    }
    return 1;
}

/* Nome de mod: [A-Za-z0-9_.-], sem "/" nem "..". */
static int safe_mod(const char *p)
{
    size_t i;
    if (!p || !*p || strlen(p) > 63) return 0;
    if (!strcmp(p, ".") || !strcmp(p, "..")) return 0;
    for (i = 0; p[i]; i++) {
        char c = p[i];
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
              (c >= '0' && c <= '9') || c == '_' || c == '-' || c == '.'))
            return 0;
    }
    return 1;
}

static int write_bytes(const char *path, const void *data, size_t len)
{
    FILE *fp = fopen(path, "wb");
    if (!fp) return -1;
    if (len && fwrite(data, 1, len, fp) != len) {
        fclose(fp);
        return -1;
    }
    return fclose(fp) == 0 ? 0 : -1;
}

/* Sufixo ".ff" (insensivel a maiusculas). */
static int has_ff_suffix(const char *s)
{
    size_t n = strlen(s);
    return n > 3 && s[n - 3] == '.' &&
           (s[n - 2] == 'f' || s[n - 2] == 'F') &&
           (s[n - 1] == 'f' || s[n - 1] == 'F');
}

/* --------------------------------------------------------------- extracao -- */

int t4_assets_extract_rawfiles(const T4FF *ff, const T4FFAsset *assets,
                               size_t count, const char *out_dir,
                               int *written)
{
    size_t i;
    int ok = 0, skipped = 0;

    g_merr[0] = '\0';
    if (!ff || !assets || !out_dir || !*out_dir)
        return mset("extract: parametros");
    if (mkdir_p(out_dir) != 0) return mfail("mkdir do diretorio de saida");

    for (i = 0; i < count; i++) {
        char path[1024], parent[1024];
        char *slash;

        if (assets[i].type != T4_ASSET_RAWFILE) continue;
        if (!assets[i].name || !assets[i].buf || assets[i].buf_len == 0)
            continue; /* sem nome ou rawfile vazio */
        if (!safe_rel(assets[i].name)) { skipped++; continue; }

        snprintf(path, sizeof(path), "%s/%s", out_dir, assets[i].name);
        snprintf(parent, sizeof(parent), "%s", path);
        slash = strrchr(parent, '/');
        if (slash) {
            *slash = '\0';
            if (mkdir_p(parent) != 0) continue;
        }
        if (write_bytes(path, assets[i].buf, assets[i].buf_len) == 0)
            ok++;
        else
            skipped++;
    }

    if (written) *written = ok;
    if (!ok && skipped) {
        snprintf(g_merr, sizeof(g_merr), "extract: nenhum arquivo gravado (%d recusados)",
                 skipped);
        return -1;
    }
    return 0;
}

/* --------------------------------------------------------------- listagem -- */

/* Raizes de .ff: <root>/zone (um nivel de subpastas, ex. zone/english). */
static int collect_ff(const char *root, char out[][1024], int max)
{
    char zone[1024];
    DIR *d;
    struct dirent *e;
    int n = 0;

    snprintf(zone, sizeof(zone), "%s/zone", root);
    d = opendir(zone);
    if (!d) return 0;
    while ((e = readdir(d)) != NULL && n < max) {
        char p[1024];
        if (e->d_name[0] == '.') continue;
        snprintf(p, sizeof(p), "%s/%s", zone, e->d_name);
        if (is_file(p) && has_ff_suffix(e->d_name)) {
            snprintf(out[n++], 1024, "%s", p);
        } else if (is_dir(p)) {
            DIR *d2 = opendir(p);
            struct dirent *e2;
            if (!d2) continue;
            while ((e2 = readdir(d2)) != NULL && n < max) {
                char p2[1024];
                if (e2->d_name[0] == '.') continue;
                if (!has_ff_suffix(e2->d_name)) continue;
                snprintf(p2, sizeof(p2), "%s/%s", p, e2->d_name);
                if (is_file(p2)) snprintf(out[n++], 1024, "%s", p2);
            }
            closedir(d2);
        }
    }
    closedir(d);
    return n;
}

static int count_files(const char *dir, int depth)
{
    DIR *d;
    struct dirent *e;
    int n = 0;

    if (depth > 6) return 0;
    d = opendir(dir);
    if (!d) return 0;
    while ((e = readdir(d)) != NULL) {
        char p[1024];
        if (e->d_name[0] == '.' &&
            (e->d_name[1] == '\0' || (e->d_name[1] == '.' && e->d_name[2] == '\0')))
            continue;
        snprintf(p, sizeof(p), "%s/%s", dir, e->d_name);
        if (is_dir(p)) n += count_files(p, depth + 1);
        else if (is_file(p)) n++;
    }
    closedir(d);
    return n;
}

/* ------------------------------------------------------------------ init -- */

static const char MOD_TXT =
    0; /* placeholder para evitar -Wpedantic em string longa (ver abaixo) */

int t4_mods_init(const char *root, const char *name)
{
    char dir[1024], doc[1024];
    static const char txt[] =
        "Mod do T4Port (Call of Duty: World at War, motor IW 3.0)\n"
        "\n"
        "Como funciona\n"
        "  mods/<nome>/  e o OVERLAY: arquivos aqui tem prioridade sobre os\n"
        "  .ff originais. Os scripts .gsc/.csc extraidos de todos os .ff\n"
        "  ficam nas mesmas pastas dos originais (maps/, zombiemode/, ...).\n"
        "\n"
        "Comandos\n"
        "  t4port mods unpack <nome>      extrai todos os scripts do jogo\n"
        "  t4port mods list               lista os mods\n"
        "  t4port mods which <caminho> --mod <nome> --search\n"
        "                                 mostra de onde viria o arquivo\n"
        "  t4port ff <arquivo.ff> -o dir  extrai so um .ff\n"
        "\n"
        "Ainda falta a VM de GSC para que o jogo EXECUTE esses scripts\n"
        "(roadmap); hoje o lado de dados ja esta completo.\n";

    g_merr[0] = '\0';
    if (!root || !safe_mod(name)) return mset("mods init: nome invalido");

    snprintf(dir, sizeof(dir), "%s/mods", root);
    if (mkdir_p(dir) != 0) return mfail("mods init: cria mods/");
    snprintf(dir, sizeof(dir), "%s/mods/%s", root, name);
    if (mkdir_p(dir) != 0) return mfail("mods init: cria mods/<nome>");

    snprintf(doc, sizeof(doc), "%s/MOD.txt", dir);
    if (write_bytes(doc, txt, sizeof(txt) - 1) != 0)
        return mfail("mods init: MOD.txt");
    (void)MOD_TXT;
    return 0;
}

/* ---------------------------------------------------------------- unpack -- */

int t4_mods_unpack(const char *root, const char *name)
{
    static char ffs[512][1024];
    char outdir[1024];
    int nff, i, total = 0, ff_ok = 0;

    g_merr[0] = '\0';
    if (!root || !safe_mod(name)) return mset("mods unpack: nome invalido");
    if (t4_mods_init(root, name) != 0) return -1;

    nff = collect_ff(root, ffs, 512);
    if (nff == 0) return mset("mods unpack: nenhum .ff em <raiz>/zone");

    snprintf(outdir, sizeof(outdir), "%s/mods/%s", root, name);
    printf("extraindo rawfiles de %d .ff para %s/mods/%s\n", nff, root, name);

    for (i = 0; i < nff; i++) {
        T4FF ff;
        T4FFAsset *assets = NULL;
        size_t count = 0, n_exact = 0, n_skip = 0;
        int anchor = 0, written = 0;

        if (t4_ff_load(&ff, ffs[i]) != 0) {
            printf("  AVISO %s: %s\n", ffs[i], t4_ff_error());
            continue;
        }
        if (t4_ff_parse(&ff) != 0) {
            printf("  AVISO %s: %s\n", ffs[i], t4_ff_error());
            t4_ff_free(&ff);
            continue;
        }
        if (t4_ff_walk(&ff, 1, &assets, &count, &n_exact, &n_skip, &anchor) != 0) {
            printf("  AVISO %s: %s\n", ffs[i], t4_ff_error());
            t4_ff_free(&ff);
            continue;
        }
        if (t4_assets_extract_rawfiles(&ff, assets, count, outdir, &written) != 0)
            printf("  AVISO %s: %s\n", ffs[i], t4_mods_error());
        if (written) {
            const char *base = strrchr(ffs[i], '/');
            printf("  %-36s %4d rawfiles%s\n", base ? base + 1 : ffs[i],
                   written, anchor ? " (ancora parcial)" : "");
            total += written;
            ff_ok++;
        }
        free(assets);
        t4_ff_free(&ff);
    }

    printf("pronto: %d arquivos de %d/%d .ff em mods/%s/\n", total, ff_ok, nff, name);
    if (!total) return mset("mods unpack: nenhum rawfile extraido");
    return 0;
}

/* ------------------------------------------------------------------ list -- */

int t4_mods_list(const char *root)
{
    char dir[1024];
    DIR *d;
    struct dirent *e;
    int n = 0;

    g_merr[0] = '\0';
    if (!root) return mset("mods list: raiz");
    snprintf(dir, sizeof(dir), "%s/mods", root);
    d = opendir(dir);
    if (!d) {
        printf("nenhum mod (diretorio %s nao existe - use: t4port mods init <nome>)\n",
               dir);
        return 0;
    }
    printf("mods em %s:\n", dir);
    while ((e = readdir(d)) != NULL) {
        char p[1024];
        if (e->d_name[0] == '.') continue;
        snprintf(p, sizeof(p), "%s/%s", dir, e->d_name);
        if (!is_dir(p)) continue;
        printf("  %-24s %5d arquivos\n", e->d_name, count_files(p, 0));
        n++;
    }
    closedir(d);
    if (!n) printf("  (nenhum - use: t4port mods init <nome>)\n");
    return 0;
}

/* ------------------------------------------------------------------ which -- */

int t4_mods_which(const char *root, const char *name, const char *rel,
                  int search_ff)
{
    static char ffs[512][1024];
    char overlay[1024];
    int nff, i;

    g_merr[0] = '\0';
    if (!root || !safe_rel(rel)) return mset("mods which: caminho invalido");

    if (name && *name) {
        if (!safe_mod(name)) return mset("mods which: nome de mod invalido");
        snprintf(overlay, sizeof(overlay), "%s/mods/%s/%s", root, name, rel);
        if (is_file(overlay)) {
            printf("overlay   %s\n", overlay + (root ? strlen(root) : 0));
            return 0;
        }
    }

    if (!search_ff) {
        printf("nao esta no overlay%s%s%s\n", name && *name ? " (mods/" : "",
               name && *name ? name : "", name && *name ? "/)" : "");
        printf("dica: --search varre os .ff; mods unpack monta a copia editavel\n");
        return 1;
    }

    nff = collect_ff(root, ffs, 512);
    printf("buscando \"%s\" em %d .ff...\n", rel, nff);
    for (i = 0; i < nff; i++) {
        T4FF ff;
        T4FFAsset *assets = NULL;
        size_t count = 0, n_exact = 0, n_skip = 0, k;
        int anchor = 0, hit = 0;
        const char *base;

        if (t4_ff_load(&ff, ffs[i]) != 0 || t4_ff_parse(&ff) != 0) {
            t4_ff_free(&ff);
            continue;
        }
        if (t4_ff_walk(&ff, 0, &assets, &count, &n_exact, &n_skip, &anchor) != 0) {
            t4_ff_free(&ff);
            continue;
        }
        for (k = 0; k < count; k++)
            if (assets[k].type == T4_ASSET_RAWFILE && assets[k].name &&
                strcmp(assets[k].name, rel) == 0)
                hit = 1;
        base = strrchr(ffs[i], '/');
        if (hit) printf("base      %s\n", base ? base + 1 : ffs[i]);
        free(assets);
        t4_ff_free(&ff);
        if (hit) return 0;
    }
    printf("nao encontrado em nenhum .ff\n");
    return 1;
}
