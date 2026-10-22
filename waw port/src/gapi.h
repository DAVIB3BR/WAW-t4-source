/* gapi.h - deteccao de APIs graficas disponiveis no sistema.
 *
 * O porte (Linux .sh/Flatpak, macOS .app, Windows .exe, FreeBSD) precisa
 * listar no menu de video apenas as APIs que existem na maquina do usuario.
 * Esta camada NAO cria contextos: checa a existencia dos loaders/bibliotecas
 * (dlopen/LoadLibrary) e, no Vulkan, dos ICDs instalados.
 *
 * "detected"  = a biblioteca/driver existe no sistema.
 * "support"   = o porte tem backend proprio para ela (GL, GLES, Vulkan).
 * Direct3D e Metal aparecem como informativos (sem backend no porte ainda).
 */
#ifndef T4_GAPI_H
#define T4_GAPI_H

#include <stddef.h>

typedef struct {
    const char *id;      /* chave estavel = nome usado em r_api: opengl|opengles|vulkan|d3d11|d3d12|metal */
    const char *name;    /* nome amigavel para o menu */
    int         detected;/* loader/biblioteca encontrada neste sistema */
    int         support; /* o porte tem backend? 1=sim 0=nao */
    const char *loader;  /* arquivo testado (informativo) */
    char        detail[96]; /* detalhe: ICDs do Vulkan, framework etc. */
} T4GApi;

/* Preenche out[0..max-1] e retorna o numero de APIs encontradas. */
int t4_gapi_probe(T4GApi *out, int max);

/* Procura por id ("opengl"...); NULL se nao existir. */
const T4GApi *t4_gapi_find(const T4GApi *list, int n, const char *id);

/* Nome curto do sistema operacional para a saida do --list. */
const char *t4_gapi_os_name(void);

/* ---------------------------------------------------------------- */
/* GPU / drivers - cobre qualquer placa (NVIDIA, AMD, Intel)          */
/* ---------------------------------------------------------------- */

typedef struct {
    int  nvidia, amd, intel;  /* placas PCI (classe 0x03) por fabricante */
    int  nouveau;             /* kernel nouveau em uso (driver aberto) */
    char nv_module[40];       /* versao do modulo NVIDIA ("" = sem driver) */
    char nv_user[40];         /* versao das libs userspace (symlink .so.X) */
    char nv_gl[48];           /* lib GL NVIDIA carregavel ("" = ausente) */
    int  nv_vk;               /* driver NVIDIA exporta Vulkan (vk_icd) */
    int  nv_icd;              /* ICD nvidia (usuario ou sistema) presente */
} T4Gpu;

/* Preenche *out e retorna o total de placas encontradas
 * (0 = contagem PCI indisponivel neste SO; as checagens de driver
 *  continuam valendo). */
int t4_gapi_gpu(T4Gpu *out);

/* Cria (enable=1) ou remove (enable=0) o ICD do usuario
 * ~/.local/share/vulkan/icd.d/nvidia_icd.json — funciona sem root e sem
 * tocar no driver. So cria se houver placa NVIDIA, driver carregavel e
 * Vulkan de verdade (símbolo vk_icdGetInstanceProcAddr).
 * 1 = criado/removido, 0 = nada a fazer, -1 = sem driver, -2 = driver
 * sem Vulkan, -3 = SO nao suportado. */
int t4_gapi_nvidia_icd(int enable);

/* T4_GPU=nvidia|amd|intel (lido uma vez no inicio): aplica env de PRIME/
 * DRI e força o ICD Vulkan do fabricante via VK_DRIVER_FILES (+alias
 * VK_ICD_FILENAMES). Sem T4_GPU (ou =auto) nao faz nada. */
void t4_gapi_apply_gpu_env(void);

/* Comando `t4port gpu [--fix|--unfix]` (relatorio + correcao). */
int t4_gpu_cmd(int argc, char **argv);

#endif /* T4_GAPI_H */
