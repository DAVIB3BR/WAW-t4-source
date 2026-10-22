/*
 * gsc.h - lexer + parser dos scripts do jogo (.gsc/.csc).
 *
 * Os scripts dos mods sao extraidos dos .ff (rawfiles) mas hoje ninguem os
 * le. Este modulo e a base da VM (roadmap: executar os scripts editados):
 *
 *   fase 1 (este arquivo): analise sintatica completa -> estatisticas e
 *     erro com linha/coluna; usada para validar os 1,4 mil scripts reais
 *   fase 2 (futura): AST + interpretador (threads, wait, notify/endon)
 *
 * Gramatica relevante (levantamento feito sobre os scripts reais do jogo):
 *   - diretils: #include caminho\com\barra;  #using_animtree(N);  #define
 *   - funcoes no topo: nome(params) { ... }
 *   - statements: if/else, for, while, do..while, switch/case/default,
 *     break, continue, return, wait <expr>;  e expressoes
 *   - chamadas especiais sao só sufixos de expressao:
 *       self waittill ("death");          -> expr ID expr
 *       level thread zombie_spawn_init(); -> expr ID expr
 *       players[i] [[level.spawnPlayer]]();
 *       animscripts\combat_utility::do_flavor_trait();
 *   - nao existe foreach nem operador -> (-> aparece so em strings/comentarios)
 *   - identificadores podem conter '\' (nomes de script: maps\_utility)
 */
#ifndef T4PORT_GSC_H
#define T4PORT_GSC_H

#include <stddef.h>

typedef struct {
    const char *path;      /* nome usado no diagnostico                 */
    size_t lines;          /* linhas do arquivo                         */
    size_t funcs;          /* funcoes de topo                           */
    size_t includes;       /* #include / #using_animtree                */
    size_t stmts;          /* instrucoes                                */
    size_t calls;          /* chamadas de funcao                        */
    int err_line, err_col; /* localizacao do primeiro erro (se houver)  */
    char err[192];         /* mensagem de erro (vazio se OK)            */
} T4GSC;

/* 0 = parse OK (stats preenchidas); -1 = erro sintatico (ver t4_gsc_error). */
int t4_gsc_parse_mem(T4GSC *g, const char *name, const char *buf, size_t len);
int t4_gsc_parse_file(T4GSC *g, const char *path);

const char *t4_gsc_error(const T4GSC *g);  /* "" se nao houve erro */

#endif /* T4PORT_GSC_H */
