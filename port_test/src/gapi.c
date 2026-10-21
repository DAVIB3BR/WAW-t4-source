/* gapi.c - deteccao de APIs graficas do sistema (cross-platform).
 *
 * Linux/FreeBSD: dlopen de libGL/libGLESv2/libvulkan + varredura dos ICDs
 *                Vulkan instalados (JSON em /usr/share/vulkan/icd.d ...).
 * Windows:       LoadLibrary de opengl32.dll, libGLESv2.dll (ANGLE),
 *                vulkan-1.dll, d3d11.dll, d3d12.dll.
 * macOS:         framework OpenGL, libvulkan (MoltenVK) e Metal.framework.
 *
 * Nenhum contexto e criado aqui: e uma checagem de presença, barata e segura
 * para o menu de video e para `t4port menu --list` (roda sem display).
 */
#include "gapi.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#endif

/* ---------------------------------------------------------------- */
/* primitiva por plataforma: a biblioteca carrega neste sistema?     */
/* ---------------------------------------------------------------- */

#ifdef _WIN32

static const char *try_load(const char *const *names)
{
    int i;
    for (i = 0; names[i]; i++) {
        HMODULE h = LoadLibraryA(names[i]);
        if (h) { FreeLibrary(h); return names[i]; }
    }
    return NULL;
}

#else

static const char *try_load(const char *const *names)
{
    int i;
    for (i = 0; names[i]; i++) {
        void *h = dlopen(names[i], RTLD_LAZY | RTLD_LOCAL);
        if (h) { dlclose(h); return names[i]; }
    }
    return NULL;
}

#endif

/* ---------------------------------------------------------------- */
/* Vulkan: lista os ICDs (drivers instalados) para o menu            */
/* ---------------------------------------------------------------- */

#ifndef _WIN32

/* anexa "texto" a "out" respeitando cap; mantem uma virgula entre itens */
static void append_item(char *out, size_t cap, const char *text, int *count)
{
    size_t used = strlen(out);
    size_t need = strlen(text) + (*count ? 2 : 0);
    if (used + need + 1 >= cap) return;          /* truncado de proposito */
    if (*count) { out[used++] = ','; out[used++] = ' '; out[used] = 0; }
    strncat(out, text, cap - strlen(out) - 1);
    (*count)++;
}

/* "nvidia_icd.x86_64.json" -> "nvidia_icd.x86_64" */
static void json_short_name(const char *file, char *out, size_t cap)
{
    const char *base = strrchr(file, '/');
    size_t n;
    base = base ? base + 1 : file;
    n = strlen(base);
    if (n > 5 && strcmp(base + n - 5, ".json") == 0) n -= 5;
    if (n >= cap) n = cap - 1;
    memcpy(out, base, n);
    out[n] = 0;
}

static void scan_icd_dir(const char *dir, char *out, size_t cap, int *total,
                         int *shown)
{
    DIR *d = opendir(dir);
    struct dirent *e;
    if (!d) return;
    while ((e = readdir(d)) != NULL) {
        size_t n = strlen(e->d_name);
        if (n < 6 || strcmp(e->d_name + n - 5, ".json") != 0) continue;
        (*total)++;
        if (*shown < 4) {                       /* mostra no maximo 4 */
            char shortn[64];
            json_short_name(e->d_name, shortn, sizeof(shortn));
            append_item(out, cap, shortn, shown);
        }
    }
    closedir(d);
}

static void vulkan_icd_detail(char *out, size_t cap)
{
    static const char *const dirs[] = {
        "/usr/share/vulkan/icd.d",
        "/etc/vulkan/icd.d",
        "/usr/local/share/vulkan/icd.d",
        NULL
    };
    char userdir[600];
    int total = 0, shown = 0, i;
    const char *home = getenv("HOME");

    out[0] = 0;
    for (i = 0; dirs[i]; i++)
        scan_icd_dir(dirs[i], out, cap, &total, &shown);
    if (home && *home && strlen(home) < 480) {
        snprintf(userdir, sizeof(userdir), "%s/.local/share/vulkan/icd.d", home);
        scan_icd_dir(userdir, out, cap, &total, &shown);
    }
    if (total == 0) {
        snprintf(out, cap, "0 ICD (loader, no drivers?)");
    } else if (total > shown) {
        size_t used = strlen(out);
        snprintf(out + used, cap - used, ",+%d", total - shown);
    }
}

#else /* _WIN32 */

static void vulkan_icd_detail(char *out, size_t cap)
{
    snprintf(out, cap, "loader (ICDs via registro)");
}

#endif

/* ---------------------------------------------------------------- */
/* probe principal                                                   */
/* ---------------------------------------------------------------- */

int t4_gapi_probe(T4GApi *out, int max)
{
    T4GApi *g;
    int n = 0;
    const char *found;

    if (!out || max < 6) return 0;

    /* ---- OpenGL (desktop) ---- */
    g = &out[n++];
#ifdef _WIN32
    { static const char *const l[] = { "opengl32.dll", NULL };
      found = try_load(l); }
#elif defined(__APPLE__)
    { static const char *const l[] = {
          "/System/Library/Frameworks/OpenGL.framework/OpenGL", NULL };
      found = try_load(l); }
#else
    { static const char *const l[] = { "libGL.so.1", "libGL.so", NULL };
      found = try_load(l); }
#endif
    g->id = "opengl"; g->name = "OpenGL"; g->support = 1;
    g->loader = found ? found : "libGL missing";
    g->detected = found != NULL;
    snprintf(g->detail, sizeof(g->detail), "%s",
             g->detected ? "default backend of the port" : "driver not installed");

    /* ---- OpenGL ES ---- */
    g = &out[n++];
#ifdef _WIN32
    { static const char *const l[] = { "libGLESv2.dll", "libEGL.dll", NULL };
      found = try_load(l); }
#elif defined(__APPLE__)
    { static const char *const l[] = { "libGLESv2.dylib", "libEGL.dylib", NULL };
      found = try_load(l); }
#else
    { static const char *const l[] = { "libGLESv2.so.2", "libGLESv2.so", NULL };
      found = try_load(l); }
#endif
    g->id = "opengles"; g->name = "OpenGL ES"; g->support = 1;
    g->loader = found ? found : "libGLESv2 missing";
    g->detected = found != NULL;
    snprintf(g->detail, sizeof(g->detail), "%s",
             g->detected ? " GLES3 (same GL backend, ES shaders)"
                         : "no ES library on this system");

    /* ---- Vulkan ---- */
    g = &out[n++];
#ifdef _WIN32
    { static const char *const l[] = { "vulkan-1.dll", NULL };
      found = try_load(l); }
#elif defined(__APPLE__)
    { static const char *const l[] = {
          "/usr/local/lib/libvulkan.1.dylib",
          "/opt/homebrew/lib/libvulkan.1.dylib",
          "libvulkan.1.dylib", NULL };
      found = try_load(l); }
#else
    { static const char *const l[] = { "libvulkan.so.1", "libvulkan.so", NULL };
      found = try_load(l); }
#endif
    g->id = "vulkan"; g->name = "Vulkan"; g->support = 1;
    g->loader = found ? found : "libvulkan missing";
    g->detected = found != NULL;
    if (g->detected) {
#ifdef __APPLE__
        snprintf(g->detail, sizeof(g->detail), "MoltenVK (Apple backend)");
#else
        vulkan_icd_detail(g->detail, sizeof(g->detail));
#endif
    } else {
        snprintf(g->detail, sizeof(g->detail), "loader missing");
    }

    /* ---- Direct3D 11 (informativo; sem backend no porte) ---- */
    g = &out[n++];
#ifdef _WIN32
    { static const char *const l[] = { "d3d11.dll", NULL };
      found = try_load(l); }
#else
    found = NULL;
#endif
    g->id = "d3d11"; g->name = "Direct3D 11"; g->support = 0;
    g->loader = "d3d11.dll (Windows)";
    g->detected = found != NULL;
    snprintf(g->detail, sizeof(g->detail), "%s",
             g->detected ? "detected on system; port uses GL/VK"
                         : "not available on this system");

    /* ---- Direct3D 12 (informativo; sem backend no porte) ---- */
    g = &out[n++];
#ifdef _WIN32
    { static const char *const l[] = { "d3d12.dll", NULL };
      found = try_load(l); }
#else
    found = NULL;
#endif
    g->id = "d3d12"; g->name = "Direct3D 12"; g->support = 0;
    g->loader = "d3d12.dll (Windows)";
    g->detected = found != NULL;
    snprintf(g->detail, sizeof(g->detail), "%s",
             g->detected ? "detected on system; port uses GL/VK"
                         : "not available on this system");

    /* ---- Metal (informativo; sem backend no porte) ---- */
    g = &out[n++];
#ifdef __APPLE__
    { static const char *const l[] = {
          "/System/Library/Frameworks/Metal.framework/Metal", NULL };
      found = try_load(l); }
#else
    found = NULL;
#endif
    g->id = "metal"; g->name = "Metal"; g->support = 0;
    g->loader = "Metal.framework (macOS)";
    g->detected = found != NULL;
    snprintf(g->detail, sizeof(g->detail), "%s",
             g->detected ? "detected on system; port uses GL/VK"
                         : "not available on this system");

    return n;
}

const T4GApi *t4_gapi_find(const T4GApi *list, int n, const char *id)
{
    int i;
    if (!list || !id) return NULL;
    for (i = 0; i < n; i++)
        if (strcmp(list[i].id, id) == 0) return &list[i];
    return NULL;
}

const char *t4_gapi_os_name(void)
{
#ifdef _WIN32
    return "windows";
#elif defined(__APPLE__)
    return "macos";
#elif defined(__FreeBSD__)
    return "freebsd";
#else
    return "linux";
#endif
}

/* ================================================================== */
/* GPU: placas (PCI) + drivers + ICD NVIDIA                            */
/* ================================================================== */

#include <sys/stat.h>
#include <sys/types.h>

#if defined(_WIN32)
#include <direct.h>
#define t4_mkdir(p) _mkdir(p)
#else
#define t4_mkdir(p) mkdir((p), 0755)
#endif

#ifndef _WIN32

/* le um arquivo curto no formato "0xXXXX"; -1 se falhar */
static long read_hex_file(const char *path)
{
    char buf[32];
    FILE *fp = fopen(path, "r");
    long v = -1;
    if (!fp) return -1;
    if (fgets(buf, sizeof(buf), fp)) sscanf(buf, "%lx", &v);
    fclose(fp);
    return v;
}

/* conta placas por fabricante em /sys/bus/pci/devices (qualquer modelo:
 * GT, GTX, RTX, Quadro, Tesla, MX, Radeon, Arc, UHD ... 0x10de/0x1002/
 * 0x8086 na classe 0x03xx = controlador de video/3D) */
static void scan_pci(T4Gpu *g)
{
    static const char *const dirs[] = { "/sys/bus/pci/devices",
                                        "/dev/pci", NULL };
    int i;
    for (i = 0; dirs[i]; i++) {
        DIR *d = opendir(dirs[i]);
        struct dirent *e;
        if (!d) continue;
        while ((e = readdir(d)) != NULL) {
            char p[600];
            long vend, cls;
            if (e->d_name[0] == '.') continue;
            snprintf(p, sizeof(p), "%s/%s/vendor", dirs[i], e->d_name);
            vend = read_hex_file(p);
            snprintf(p, sizeof(p), "%s/%s/class", dirs[i], e->d_name);
            cls = read_hex_file(p);
            if (vend < 0 || cls < 0) continue;
            if ((cls >> 16) != 0x03) continue;   /* 03xx = display/3D */
            if (vend == 0x10de) g->nvidia++;
            else if (vend == 0x1002 || vend == 0x1022) g->amd++;
            else if (vend == 0x8086) g->intel++;
        }
        closedir(d);
    }
}

static void read_nv_module(T4Gpu *g)
{
    char line[256];
    FILE *fp = fopen("/proc/driver/nvidia/version", "r");
    const char *m;
    size_t n = 0;
    if (!fp) return;
    if (fgets(line, sizeof(line), fp)) {
        m = strstr(line, "Kernel Module");
        if (m) {
            m += strlen("Kernel Module");
            while (*m == ' ' || *m == '\t') m++;
            while ((m[n] >= '0' && m[n] <= '9') || m[n] == '.') n++;
            if (n >= sizeof(g->nv_module)) n = sizeof(g->nv_module) - 1;
            memcpy(g->nv_module, m, n);
            g->nv_module[n] = '\0';
        }
    }
    fclose(fp);
}

/* versao das libs userspace: libGLX_nvidia.so.0 -> ...so.470.256.02
 * (o symlink .so.0 e estavel desde ~295 e o alvo traz a versao instalada,
 *  cobrindo340/390/470/525+/qualquer branch proprietaria) */
static void read_nv_userspace(T4Gpu *g)
{
    static const char *const links[] = {
        "/usr/lib64/libGLX_nvidia.so.0", "/usr/lib/libGLX_nvidia.so.0",
        "/usr/lib/x86_64-linux-gnu/libGLX_nvidia.so.0",
        "/usr/local/lib/libGLX_nvidia.so.0", NULL };
    char buf[300];
    int i;
    for (i = 0; links[i]; i++) {
        ssize_t n = readlink(links[i], buf, sizeof(buf) - 1);
        const char *base, *dot;
        if (n <= 0) continue;
        buf[n] = '\0';
        base = strrchr(buf, '/');
        base = base ? base + 1 : buf;
        dot = strstr(base, "libGLX_nvidia.so.");
        if (!dot) continue;
        dot += strlen("libGLX_nvidia.so.");
        if (!*dot) continue;
        snprintf(g->nv_user, sizeof(g->nv_user), "%s", dot);
        return;
    }
}

/* instalacao com symlink .so.0 quebrado/ausente: acha a lib versionada */
static int find_nv_versioned(char *out, size_t cap)
{
    static const char *const dirs[] = { "/usr/lib64", "/usr/lib",
                                        "/usr/lib/x86_64-linux-gnu",
                                        "/usr/local/lib", NULL };
    int i;
    for (i = 0; dirs[i]; i++) {
        DIR *d = opendir(dirs[i]);
        struct dirent *e;
        if (!d) continue;
        while ((e = readdir(d)) != NULL) {
            if (strncmp(e->d_name, "libGLX_nvidia.so.", 17) != 0) continue;
            if (!e->d_name[17]) continue;
            snprintf(out, cap, "%s/%s", dirs[i], e->d_name);
            closedir(d);
            return 1;
        }
        closedir(d);
    }
    return 0;
}

/* o driver NVIDIA exporta Vulkan? (mesmo teste do ICD do loader) */
static int nv_has_vulkan(const char *lib)
{
    void *h;
    int ok = 0;
    if (!lib || !*lib) return 0;
    h = dlopen(lib, RTLD_LAZY | RTLD_LOCAL);
    if (!h) return 0;
    ok = dlsym(h, "vk_icdGetInstanceProcAddr") != NULL;
    dlclose(h);
    return ok;
}

static int icd_file_exists(void)
{
    static const char *const dirs[] = { "/usr/share/vulkan/icd.d",
                                        "/etc/vulkan/icd.d",
                                        "/usr/local/share/vulkan/icd.d",
                                        NULL };
    char userdir[600];
    const char *home = getenv("HOME");
    int i, found = 0;
    for (i = 0; dirs[i] && !found; i++) {
        DIR *d = opendir(dirs[i]);
        struct dirent *e;
        if (!d) continue;
        while ((e = readdir(d)) != NULL)
            if (strncmp(e->d_name, "nvidia_icd", 10) == 0) { found = 1; break; }
        closedir(d);
    }
    if (!found && home && *home && strlen(home) < 480) {
        snprintf(userdir, sizeof(userdir), "%s/.local/share/vulkan/icd.d",
                 home);
        DIR *d = opendir(userdir);
        struct dirent *e;
        if (d) {
            while ((e = readdir(d)) != NULL)
                if (strncmp(e->d_name, "nvidia_icd", 10) == 0) {
                    found = 1;
                    break;
                }
            closedir(d);
        }
    }
    return found;
}

#endif /* !_WIN32 - helpers POSIX-only */

#ifdef _WIN32
/* versao Windows do teste: o ICD da NVIDIA e o proprio nvoglv64.dll */
static int nv_has_vulkan(const char *lib)
{
    HMODULE h = LoadLibraryA(lib);
    int ok;
    if (!h) return 0;
    ok = GetProcAddress(h, "vk_icdGetInstanceProcAddr") != NULL;
    FreeLibrary(h);
    return ok;
}
#endif

/* guia das faixas do driver proprietario: contextualiza a saida do
 * `t4port gpu` (o que e REAL sempre vem do dlsym vk_icd, nunca daqui) */
static void nv_expected(const char *ver, char *out, size_t cap)
{
    int major = (ver && *ver) ? atoi(ver) : 0;
    if (major <= 0) { snprintf(out, cap, "versao desconhecida"); return; }
    if (major < 360)
        snprintf(out, cap, "%d.xx: no Vulkan (use OpenGL)", major);
    else if (major < 390)
        snprintf(out, cap, "%d.xx: Vulkan 1.0 (Maxwell+)", major);
    else if (major < 470)
        snprintf(out, cap, "%d.xx: Vulkan 1.1", major);
    else if (major < 525)
        snprintf(out, cap, "%d.xx: Vulkan 1.2 (branch legado)", major);
    else
        snprintf(out, cap, "%d.xx: Vulkan 1.3", major);
}

int t4_gapi_gpu(T4Gpu *out)
{
#ifndef _WIN32
    static const char *const nv_gl_libs[] = {
        "libGLX_nvidia.so.0", "libGLX_nvidia.so", NULL };
#endif
    const char *found;
    int have_pcidir;

    if (!out) return 0;
    memset(out, 0, sizeof(*out));

#ifndef _WIN32
    have_pcidir = access("/sys/bus/pci/devices", F_OK) == 0;
    if (have_pcidir) {
        scan_pci(out);
        if (access("/sys/module/nouveau", F_OK) == 0) out->nouveau = 1;
    }
    /* driver proprietario NVIDIA: placa, modulo e GL valem em qualquer SO
     * onde o driver existe (Linux/FreeBSD); contagem PCI e so Linux. */
    read_nv_module(out);
    read_nv_userspace(out);
#else
    (void)have_pcidir;
#endif
#ifdef _WIN32
    found = try_load((const char *[]){ "nvoglv64.dll", "nvcuda.dll", NULL });
    if (found) {
        if (!out->nvidia) out->nvidia = 1;
        snprintf(out->nv_gl, sizeof(out->nv_gl), "%s", found);
        out->nv_vk = nv_has_vulkan("nvoglv64.dll") ? 1 : -1;
    }
#else
    found = try_load(nv_gl_libs);
    if (found) {
        snprintf(out->nv_gl, sizeof(out->nv_gl), "%s", found);
        if (!out->nvidia) out->nvidia = 1;  /* driver presente = placa ativa */
        out->nv_vk = nv_has_vulkan(found);
    } else {
        /* symlink .so.0 quebrado/instalacao incompleta: versao direta
         * (lib versionada ainda presente) - cobre atualizacoes/rolls  * de versao do driver proprietario */
        char abs[320];
        if (find_nv_versioned(abs, sizeof(abs)) &&
            try_load((const char *[]){ abs, NULL })) {
            snprintf(out->nv_gl, sizeof(out->nv_gl), "%s", abs);
            if (!out->nvidia) out->nvidia = 1;
            out->nv_vk = nv_has_vulkan(abs);
        }
    }
#endif
#ifndef _WIN32
    out->nv_icd = icd_file_exists();
#else
    out->nv_icd = 0;
#endif
    return out->nvidia + out->amd + out->intel;
}

/* ---- ICD do usuario (sem root) ---------------------------------- */

#ifndef _WIN32
static void mkdir_p(char *path)
{
    char *p;
    for (p = path + 1; *p; p++)
        if (*p == '/') { *p = '\0'; t4_mkdir(path); *p = '/'; }
    t4_mkdir(path);
}
#endif

int t4_gapi_nvidia_icd(int enable)
{
#ifndef _WIN32
    char dir[600], path[700];
    const char *home = getenv("HOME");
    T4Gpu g;
    FILE *fp;
#endif

#ifdef _WIN32
    (void)enable;
    return -3;                     /* Windows: ICD vem do registro */
#else
    if (!enable) {                 /* --unfix: remove dos dois locais */
        int removed = 0;
        if (home && *home && strlen(home) < 480) {
            snprintf(path, sizeof(path),
                     "%s/.local/share/vulkan/icd.d/nvidia_icd.json", home);
            if (remove(path) == 0) removed = 1;
        }
        if (remove("/usr/share/vulkan/icd.d/nvidia_icd.json") == 0)
            removed = 1;
        return removed;
    }

    t4_gapi_gpu(&g);
    if (!g.nvidia && !g.nv_gl[0]) return -1;
    if (!g.nv_gl[0]) return -1;
    if (!g.nv_vk) return -2;
    if (g.nv_icd) return 0;

    if (!home || !*home || strlen(home) > 480) return -1;
    snprintf(dir, sizeof(dir), "%s/.local/share/vulkan/icd.d", home);
    mkdir_p(dir);
    snprintf(path, sizeof(path), "%s/nvidia_icd.json", dir);
    fp = fopen(path, "w");
    if (!fp) return -1;
    {
        /* SONAME se ele carrega; caminho absoluto quando o symlink .so.0
         * esta quebrado (fallback p/ instalacoes incompletas) */
        const char *lib = strchr(g.nv_gl, '/') ? g.nv_gl
                                                  : "libGLX_nvidia.so.0";
        fprintf(fp,
                "{\n"
                "    \"file_format_version\": \"1.0.0\",\n"
                "    \"ICD\": {\n"
                "        \"library_path\": \"%s\"\n"
                "    }\n"
                "}\n", lib);
    }
    fclose(fp);
    return 1;
#endif
}

/* ---- T4_GPU: força fabricante (GL PRIME/DRI + ICD Vulkan) --------- */

#ifndef _WIN32
/* acha o json do ICD do fabricante (64-bit; ignora arquivos .i686) */
static int find_icd_json(const char *vendor, char *out, size_t cap)
{
    static const char *const dirs[] = { "/usr/share/vulkan/icd.d",
                                        "/etc/vulkan/icd.d",
                                        "/usr/local/share/vulkan/icd.d",
                                        NULL };
    char userdir[600];
    const char *home = getenv("HOME");
    const char *prefix = vendor;
    int i;

    if (strcmp(vendor, "amd") == 0) prefix = "radeon";
    for (i = 0; i < 4; i++) {
        const char *dir = NULL;
        DIR *d;
        struct dirent *e;
        if (i < 3) dir = dirs[i];
        else {
            if (!home || !*home || strlen(home) > 480) break;
            snprintf(userdir, sizeof(userdir),
                     "%s/.local/share/vulkan/icd.d", home);
            dir = userdir;
        }
        d = opendir(dir);
        if (!d) continue;
        while ((e = readdir(d)) != NULL) {
            size_t n = strlen(e->d_name);
            if (strncmp(e->d_name, prefix, strlen(prefix)) != 0) continue;
            if (n < 5 || strcmp(e->d_name + n - 5, ".json") != 0) continue;
            if (strstr(e->d_name, ".i686")) continue;  /* manda so no 64 */
            snprintf(out, cap, "%s/%s", dir, e->d_name);
            closedir(d);
            return 1;
        }
        closedir(d);
    }
    return 0;
}
#endif /* !_WIN32 */

void t4_gapi_apply_gpu_env(void)
{
#ifndef _WIN32
    const char *v = getenv("T4_GPU");
    char icd[700];

    if (!v || !*v) return;
    if (strcmp(v, "nvidia") == 0) {
        setenv("__NV_PRIME_RENDER_OFFLOAD", "1", 1);
        setenv("__GLX_VENDOR_LIBRARY_NAME", "nvidia", 1);
    } else if (strcmp(v, "amd") == 0 || strcmp(v, "intel") == 0) {
        setenv("DRI_PRIME", "1", 1);
    } else {
        return;                          /* auto/valor desconhecido */
    }
    if (find_icd_json(v, icd, sizeof(icd))) {
        setenv("VK_DRIVER_FILES", icd, 1);     /* loader novo */
        setenv("VK_ICD_FILENAMES", icd, 1);    /* loader antigo */
    }
#endif
}

/* ---- comando `t4port gpu` --------------------------------------- */

int t4_gpu_cmd(int argc, char **argv)
{
    T4Gpu g;
    int i, do_fix = 0, do_unfix = 0;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--fix") == 0) do_fix = 1;
        else if (strcmp(argv[i], "--unfix") == 0) do_unfix = 1;
        else if (strcmp(argv[i], "--help") == 0) {
            printf("t4port gpu [--fix|--unfix]\n"
                   "  (sem argumentos) relatorio de placas e drivers\n"
                   "  --fix     cria o ICD Vulkan NVIDIA do usuario (sem root)\n"
                   "  --unfix   remove o ICD criado por --fix\n"
                   "  T4_GPU=nvidia|amd|intel antes do comando força a GPU\n");
            return 0;
        }
    }

    if (do_unfix) {
        int r = t4_gapi_nvidia_icd(0);
        printf("%s\n", r > 0
                   ? "ICD NVIDIA removido (Vulkan volta ao padrao no proximo inicio)"
                   : "nada a remover (ICD NVIDIA nao existia)");
        return 0;
    }

    t4_gapi_gpu(&g);

    printf("placas (PCI): %d NVIDIA, %d AMD, %d Intel\n",
           g.nvidia, g.amd, g.intel);
    if (!g.nvidia && !g.amd && !g.intel)
        printf("  (contagem PCI disponivel apenas no Linux nesta versao)\n");
    if (g.nouveau)
        printf("kernel nouveau em uso (driver aberto NVIDIA)\n");

    if (g.nvidia || g.nv_gl[0] || g.nv_module[0]) {
        printf("driver NVIDIA:\n");
        printf("  modulo kernel   %s\n",
               g.nv_module[0] ? g.nv_module
                              : "nao encontrado (/proc/driver/nvidia)");
        printf("  libs userspace  %s\n",
               g.nv_user[0] ? g.nv_user : "?");
        {
            char exp[72];
            nv_expected(g.nv_module[0] ? g.nv_module : g.nv_user,
                        exp, sizeof(exp));
            printf("  faixa do driver %s | real: %s\n", exp,
                   g.nv_vk > 0 ? "sim" : (g.nv_vk == 0 ? "nao" : "?"));
        }
        printf("  OpenGL          %s\n",
               g.nv_gl[0] ? g.nv_gl : "libGLX_nvidia AUSENTE");
        printf("  Vulkan          %s\n",
               g.nv_vk > 0 ? "SIM (driver exporta vk_icd)"
                           : (g.nv_vk == 0 ? "NAO - este driver nao tem Vulkan"
                                           : "desconhecido neste SO"));
        printf("  ICD (vulkan)    %s\n",
               g.nv_icd ? "ativo" : "ausente");
        if (g.nv_icd && !g.nv_gl[0])
            printf("  (ICD restante sem driver - limpe com --unfix)\n");

        if (do_fix) {
            int r = t4_gapi_nvidia_icd(1);
            switch (r) {
            case 1:
                printf("  ICD criado: ~/.local/share/vulkan/icd.d/"
                       "nvidia_icd.json\n");
                printf("  pronto: o Vulkan ja enxerga a placa NVIDIA "
                       "(remover: --unfix)\n");
                break;
            case 0:
                printf("  ICD ja existia (nada a fazer)\n");
                break;
            case -2:
                printf("  recusado: o driver NVIDIA instalado nao tem "
                       "Vulkan (atualize o driver)\n");
                break;
            case -3:
                printf("  --fix nao se aplica neste SO\n");
                break;
            default:
                printf("  erro ao criar o ICD (verifique ~/.local)\n");
            }
        } else if (!g.nv_icd && g.nv_vk > 0) {
            printf("  -> Vulkan da NVIDIA nao registrado; ative com: "
                   "t4port gpu --fix\n");
        }
    } else {
        printf("driver NVIDIA nao encontrado neste sistema\n");
#ifdef __APPLE__
        printf("  (macOS: NVIDIA so funciona ate o 10.13 High Sierra com web "
               "driver;\n   a partir do Mojave nao ha driver oficial - a GPU, "
               "se existir, nao e usavel\n   e o jogo roda pelo OpenGL do "
               "proprio sistema)\n");
#elif defined(__FreeBSD__)
        printf("  (FreeBSD: com o x11/nvidia-driver instalado a placa e as "
               "libs\n   aparecem aqui; a contagem PCI precisa do sysfs do "
               "Linux)\n");
#endif
    }

    printf("forcar GPU: T4_GPU=nvidia|amd|intel t4port gfx --api vulkan\n");
    printf("  (T4_GPU=nvidia = PRIME/GL na NVIDIA + so o ICD dela no "
           "Vulkan)\n");
    return 0;
}
