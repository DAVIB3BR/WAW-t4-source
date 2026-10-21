/*
 * iwd.h - Leitor de arquivos .iwd do motor Treyarch (CoD: WaW).
 *
 * Um .iwd e um ZIP comum (local file headers + central directory), usado para
 * empacotar os assets do jogo (images/, sounds/, .cfg etc).
 * Aqui lemos o central directory diretamente em vez de depender de libzip,
 * para manter o porte sem dependencias alem de zlib.
 */
#ifndef T4_IWD_H
#define T4_IWD_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define T4_IWD_MAX_NAME 512

typedef struct {
    char     name[T4_IWD_MAX_NAME]; /* normalizado com '/' */
    uint32_t method;                /* 0 = stored, 8 = deflate */
    uint32_t comp_size;
    uint32_t uncomp_size;
    uint32_t local_offset;          /* offset do local file header */
    uint32_t crc32;
} T4ZipEntry;

typedef struct {
    char       path[512];
    FILE      *fp;
    T4ZipEntry *entries;
    size_t     count;
    uint64_t   total_uncomp;
} T4Iwd;

/* Abre e faz o parse do central directory. Retorna 0 ok, <0 erro (errno/t4_last_error). */
int  t4_iwd_open(T4Iwd *z, const char *path);
void t4_iwd_close(T4Iwd *z);

size_t             t4_iwd_count(const T4Iwd *z);
const T4ZipEntry  *t4_iwd_at(const T4Iwd *z, size_t index);
const T4ZipEntry  *t4_iwd_find(const T4Iwd *z, const char *name);

/*
 * Le e descompacta uma entrada. *out e alocado com malloc() e deve ser
 * liberado pelo chamador (mesmo quando o metodo e stored).
 * Retorna 0 ok, <0 erro.
 */
int t4_iwd_read(const T4Iwd *z, const T4ZipEntry *e, uint8_t **out, size_t *out_len);

/* Mensagem de erro da ultima operacao (estatica, nao thread-safe). */
const char *t4_iwd_error(void);

#endif /* T4_IWD_H */
