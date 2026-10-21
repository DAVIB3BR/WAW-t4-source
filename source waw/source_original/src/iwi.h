/*
 * iwi.h - texturas .iwi do motor Treyarch (CoD: World at War).
 *
 * Cabeçalho comprovado por verificacao de contas (doc/RE_SUMMARY.md):
 *   +0x00  char[3]  "IWi"
 *   +0x03  u8       versao (6 nas amostras)
 *   +0x08  (ver abaixo)
 *   +0x04  u8       formato:  11 = bloco de 8 bytes  (DXT1/BC1)
 *                            12 = bloco de 16 bytes (DXT3/BC2 ou DXT5/BC3)
 *   +0x05  u8       flags (0xC3 nas amostras)
 *   +0x06  u16      largura
 *   +0x08  u16      altura
 *   +0x0A  u16      campo nao identificado (1 nas amostras)
 *   +0x0C  u32[4]   = tamanho total do arquivo (repetido 4x)
 *   +0x1C  payload  (no minimo 1 nivel; a cadeia de mips e deduzida do tam.)
 *
 * Decodificamos sempre o nivel 0 (maior) para RGBA8.
 */
#ifndef T4_IWI_H
#define T4_IWI_H

#include <stddef.h>
#include <stdint.h>

#define T4_IWI_HEADER_SIZE 28
#define T4_IWI_FORMAT_BGRA8 1   /* nao comprimido, B,G,R,A */
#define T4_IWI_FORMAT_A8L8  3   /* nao comprimido, [0]=alpha [1]=luma */
#define T4_IWI_FORMAT_L8    4   /* nao comprimido, 1 byte/pixel */
#define T4_IWI_FORMAT_DXT1  11  /* BC1, 8 B/bloco  (2.667 arq.) */
#define T4_IWI_FORMAT_DXT5  12  /* BC3, 16 B/bloco (803 arq.) */
#define T4_IWI_FORMAT_DXN   13  /* BC5, 16 B/bloco, mapa normal (5.665 arq.) */

typedef struct {
    int      version;
    int      format;
    int      flags;
    int      width;
    int      height;
    int      field_0x0A;
    uint32_t file_size_field; /* u32[4] em +0x0C (todas iguais) */
    const uint8_t *payload;
    size_t   payload_len;
} T4IwiInfo;

int t4_iwi_parse(const uint8_t *data, size_t len, T4IwiInfo *out);

/*
 * Decodifica o nivel 0 para RGBA8. *rgba = malloc(w*h*4), liberar com free().
 * Retorna 0 ok; <0 erro com mensagem em t4_iwi_error().
 */
int         t4_iwi_decode_rgba(const uint8_t *data, size_t len, uint8_t **rgba, int *w, int *h);
const char *t4_iwi_error(void);

#endif /* T4_IWI_H */
