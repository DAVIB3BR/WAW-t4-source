/*
 * mods.h - extracao de rawfiles (GSC/CSC/etc.) e gerenciamento de mods.
 *
 * Os scripts do jogo (.gsc/.csc) vivem DENTRO dos .ff como assets "rawfile"
 * (tipo 32), ja extraidos com exatidao pelo parser de ff.c. Este modulo joga
 * esse conteudo no disco, montando um workspace editavel:
 *
 *   <raiz_do_jogo>/mods/<nome>/maps/_zombiemode.gsc   <- mod (overlay)
 *
 * Ordem de resolucao prevista para o motor (futuro loop de jogo):
 *   1) mods/<nome>/<caminho>   (overlay do mod)
 *   2) conteudo original dos .ff / .iwd
 *
 * ATENCAO: extrair/ editar scripts ainda nao faz o jogo roda-los - falta a
 * VM de GSC (roadmap). Este modulo e o lado "dados" do suporte a mods.
 */
#ifndef T4PORT_MODS_H
#define T4PORT_MODS_H

#include "ff.h"

#include <stddef.h>

/* Escreve em out_dir (relativo ao proprio caminho do asset) todos os
 * rawfiles com buffer. Retorna 0 ok, <0 erro (t4_mods_error()). */
int t4_assets_extract_rawfiles(const T4FF *ff, const T4FFAsset *assets,
                               size_t count, const char *out_dir,
                               int *written);

/* Cria <root>/mods/<nome>/ com MOD.txt de instrucoes. */
int t4_mods_init(const char *root, const char *name);

/* Extrai TODOS os rawfiles de TODOS os .ff de <root>/zone em
 * <root>/mods/<nome>/ (workspace do mod). Imprime progresso. */
int t4_mods_unpack(const char *root, const char *name);

/* Lista os mods existentes em <root>/mods/. */
int t4_mods_list(const char *root);

/* De onde viria <rel>: overlay (mods/<nome>/<rel>) ou, com search_ff, o .ff
 * original que o contem. Retorna 0 encontrado, 1 nao encontrado, <0 erro. */
int t4_mods_which(const char *root, const char *name, const char *rel,
                  int search_ff);

const char *t4_mods_error(void);

#endif /* T4PORT_MODS_H */
