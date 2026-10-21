/*
 * ff.h - FastFiles do motor IW 3.0 (Call of Duty: World at War, Treyarch).
 *
 * Layout do arquivo em disco (comprovado por analise em 129/129 arquivos,
 * ver doc/RE_SUMMARY.md e re/FF_FORMAT.md):
 *   +0x00  char[8]  magic = "IWffu100"
 *   +0x08  u32      versao = 387 (IGUAL em todos os .ff do WaW)
 *   +0x0C  ...      UM UNICO stream zlib; sobram ~11 bytes de padding
 *
 * Conteudo descompactado (XFile + XAssetList):
 *   +0x00  u32  size          (tamanho logico dos dados)
 *   +0x04  u32  externalSize  (tamanho do pool externo / bloco grande)
 *   +0x08  u32  blocks[7]     (tamanhos por bloco: TEMP, RUNTIME,
 *                              LARGE_RUNTIME, PHYSICAL_RUNTIME,
 *                              VIRTUAL, LARGE, PHYSICAL)
 *   +0x24  XAssetList {
 *            u32  stringCount
 *            u32  strings*     (FOLLOWING = 0xFFFFFFFF, inline)
 *            u32  assetCount
 *            u32  assets*      (FOLLOWING em todos os .ff testados)
 *          }
 *   +0x34  u32  stringSlots[stringCount]   (ponteiros p/ string pool)
 *          char stringsFOLLOWING[]         (as strings referenciadas acima)
 *          XAsset   assets[assetCount]     { u32 type; u32 header*; }
 *          ... dados inline dos assets ...
 *
 * Modelo de ponteiros:
 *   0xFFFFFFFF = FOLLOWING (o dado segue inline no stream)
 *   0xFFFFFFFE = INSERT    (slot inserido no bloco VIRTUAL)
 *   outro      = offset + 1, com o indice do bloco nos 3 bits altos
 */
#ifndef T4_FF_H
#define T4_FF_H

#include <stddef.h>
#include <stdint.h>

#define T4_FF_MAGIC    "IWffu100"
#define T4_FF_VERSION  387u   /* versao retail do WaW; igual em 129/129 .ff */
#define T4_FF_PTR_FOLLOWING 0xFFFFFFFFu
#define T4_FF_PTR_INSERT    0xFFFFFFFEu

/* Enum XAssetType da v387 (WaW/T4), mapeado empiricamente - ver re/fftool.py. */
enum {
    T4_ASSET_XMODELPIECES   = 0,
    T4_ASSET_PHYSPRESET     = 1,
    T4_ASSET_PHYSCONSTRAINTS= 2,
    T4_ASSET_DESTRUCTIBLEDEF= 3,
    T4_ASSET_XANIMPARTS     = 4,
    T4_ASSET_XMODEL         = 5,
    T4_ASSET_MATERIAL       = 6,
    T4_ASSET_TECHNIQUE_SET  = 7,
    T4_ASSET_IMAGE          = 8,
    T4_ASSET_SOUND          = 9,
    T4_ASSET_CLIPMAP        = 11,
    T4_ASSET_CLIPMAP_PVS    = 12,
    T4_ASSET_COMWORLD       = 13,
    T4_ASSET_GAMEWORLD_SP   = 14,
    T4_ASSET_GAMEWORLD_MP   = 15,
    T4_ASSET_GFXWORLD       = 17,
    T4_ASSET_LIGHT_DEF      = 18,
    T4_ASSET_FONT           = 20,
    T4_ASSET_MENULIST       = 21,
    T4_ASSET_LOCALIZE_ENTRY = 23,
    T4_ASSET_CHARACTER      = 24,
    T4_ASSET_WEAPONDEF      = 25,
    T4_ASSET_FX             = 26,
    T4_ASSET_SNDGLOBALS     = 27,
    T4_ASSET_RAWFILE        = 32,
    T4_ASSET_STRINGTABLE    = 33
};

typedef struct {
    uint8_t *data;   /* conteudo descompactado (malloc) */
    size_t   len;
    uint32_t hdr0;   /* u32 em +0x08 do arquivo comprimido */
    uint32_t body0;  /* u32 em +0x00 do conteudo */
    uint32_t body1;  /* u32 em +0x04 */
    uint32_t body2;  /* u32 em +0x08 */
    /* -- campos do XFile/XAssetList preenchidos por t4_ff_parse() -- */
    uint32_t size;          /* +0x00 */
    uint32_t external_size; /* +0x04 */
    uint32_t blocks[7];     /* +0x08 */
    uint32_t asset_count;   /* nº de XAsset no array */
    uint32_t string_count;  /* nº de script strings (preenchido por t4_ff_parse) */
    uint32_t data_start;    /* offset onde comecam os dados inline dos assets */
    uint32_t *asset_types;  /* asset_count entradas (aponta p/ data) */
} T4FF;

/* Um asset com layout conhecido, resultante da caminhada exata. */
typedef struct {
    uint32_t index;      /* indice no array de assets */
    uint32_t type;       /* XAssetType */
    const char *name;    /* nome inline (NUL-terminado dentro de data), ou NULL */
    size_t   name_len;
    const uint8_t *buf;  /* buffer do rawfile (NULL se nao for rawfile ou sem buffer) */
    size_t   buf_len;
} T4FFAsset;

/* Carrega e descompacta. Retorna 0 ok, <0 erro (mensagem em t4_ff_error()). */
int  t4_ff_load(T4FF *ff, const char *path);
void t4_ff_free(T4FF *ff);
const char *t4_ff_error(void);

/* Parseia o XAssetList apos t4_ff_load (preenche asset_count/data_start/
 * asset_types). Retorna 0 ok, <0 erro. */
int t4_ff_parse(T4FF *ff);

/* Caminhada exata dos assets (espelha re/fftool.py, validada em 129/129).
 * Aloca *out (malloc) com *count entradas; devolve 0 ok, <0 erro.
 * `collect_rawfiles` preenche buf/buf_len dos rawfiles.
 * Estatisticas (assets exatos/pulados) sao devolvidas via ponteiros opcionais. */
int t4_ff_walk(const T4FF *ff, int collect_rawfiles,
               T4FFAsset **out, size_t *count,
               size_t *n_exact, size_t *n_skipped, int *anchor_failed);

#endif /* T4_FF_H */
