/*
 * gsc.c - lexer + parser dos scripts .gsc/.csc (fase 1 da VM).
 *
 * Analise sintatica completa com diagnostico linha/coluna. Sem AST por
 * enquanto: o objetivo desta fase e validar os ~1,4 mil scripts reais
 * extraidos dos .ff e ficar como base da interpretacao (fase 2).
 *
 * Gramatica (levantada sobre os scripts do jogo; ver gsc.h):
 *
 *   programa  := (diretiva | funcao | instrucao)*
 *   funcao    := nome '(' params? ')' bloco
 *   bloco     := '{' instrucao* '}'
 *   instrucao := bloco | if | for | while | do | switch | break | continue
 *              | return | wait expr ';' | expr ( ID expr )* ';'
 *
 *   expressao := C-like: atribuicao direita, ?:, ||, &&, |, ^, &, ==/!=,
 *              < > <= >=, deslocamentos, soma/subtracao, multiplicacao,
 *              divisao e modulo; unarios (! ~ - + ++ --) e sufixos
 *              (args), [i], .membro, ::ns, ::ns(args), [[expr]](args)
 *
 * `self waittill ("death")` e `level thread f()` sao cobertos pela regra
 * `expr (ID expr)*` — em GSC eles sao chamadas "enxertadas" apos o objeto.
 */
#include "gsc.h"

#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    T_EOF = 0, T_ID, T_NUM, T_STR,
    T_LBRACE, T_RBRACE, T_LP, T_RP, T_LB, T_RB, T_LBRB, T_RBRB,
    T_SEMI, T_COMMA, T_COLON, T_DCOLON, T_DOT, T_QUEST, T_HASH,
    T_PLUS, T_MINUS, T_STAR, T_SLASH, T_PCT, T_TILDE, T_NOT,
    T_AMP, T_PIPE, T_CARET, T_SHL, T_SHR, T_AND, T_OR,
    T_EQ, T_NE, T_LT, T_GT, T_LE, T_GE,
    T_ASSIGN, T_PLUSAS, T_MINUSAS, T_STARAS, T_SLASHAS, T_PCTAS,
    T_ANDAS, T_ORAS, T_XORAS, T_SHLAS, T_SHRAS, T_INC, T_DEC
} TType;

typedef struct {
    const char *p, *end;
    int line, col;              /* posicao do proximo caractere */
    TType t;                    /* token atual */
    const char *ts;             /* inicio do token atual */
    int tl;                     /* tamanho do token atual */
    int tline, tcol;            /* onde o token atual comeca */
    int prev_line;              /* linha do token anterior (ponto-e-virgula
                                   opcional antes da quebra de linha) */
    T4GSC *g;
    jmp_buf jb;
} Lex;

static void failp(Lex *L, const char *msg);

/* ---------------------------------------------------------------- lexer -- */

static void skip_space(Lex *L)
{
    for (;;) {
        while (L->p < L->end) {
            char c = *L->p;
            if (c == '\n') { L->line++; L->col = 1; L->p++; }
            else if (c == ' ' || c == '\t' || c == '\r' || c == '\f' ||
                     c == '\v' || c == '`') {   /* '`' aparece em 2 scripts */
                L->col++; L->p++;
            } else if (c == '\\') {
                /* barra solta = continuacao de linha (tolerante) */
                L->col++; L->p++;
            } else break;
        }
        if (L->p + 1 < L->end && L->p[0] == '/' && L->p[1] == '/') {
            while (L->p < L->end && *L->p != '\n') L->p++;
            continue;
        }
        if (L->p + 1 < L->end && L->p[0] == '/' && L->p[1] == '*') {
            L->p += 2;
            while (L->p + 1 < L->end && !(L->p[0] == '*' && L->p[1] == '/')) {
                if (*L->p == '\n') { L->line++; L->col = 1; } else L->col++;
                L->p++;
            }
            if (L->p + 1 < L->end) L->p += 2;
            continue;
        }
        /* /# ... #/ = bloco so-compilado-com-debug (existe nos scripts reais) */
        if (L->p + 1 < L->end && L->p[0] == '/' && L->p[1] == '#') {
            L->p += 2;
            while (L->p + 1 < L->end && !(L->p[0] == '#' && L->p[1] == '/')) {
                if (*L->p == '\n') { L->line++; L->col = 1; } else L->col++;
                L->p++;
            }
            if (L->p + 1 < L->end) L->p += 2;
            continue;
        }
        break;
    }
}

static int is_alnum_c(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
           (c >= '0' && c <= '9') || c == '_';
}

static void scan(Lex *L)
{
    const char *s;

    skip_space(L);
    L->prev_line = L->tline;
    L->ts = L->p;
    L->tline = L->line;
    L->tcol = L->col;

    if (L->p >= L->end) { L->t = T_EOF; L->tl = 0; return; }

    /* identificador (pode conter '\': maps\_utility) */
    if ((L->p[0] >= 'a' && L->p[0] <= 'z') ||
        (L->p[0] >= 'A' && L->p[0] <= 'Z') || L->p[0] == '_') {
        L->p++;
        while (L->p < L->end) {
            char c = *L->p;
            if (is_alnum_c(c)) { L->p++; continue; }
            if (c == '\\' && L->p + 1 < L->end &&
                ((L->p[1] >= 'a' && L->p[1] <= 'z') ||
                 (L->p[1] >= 'A' && L->p[1] <= 'Z') || L->p[1] == '_')) {
                L->p += 2;
                continue;
            }
            break;
        }
        L->t = T_ID;
        L->tl = (int)(L->p - L->ts);
        L->col += L->tl;           /* o loop acima so andou com o ponteiro */
        return;
    }

    /* numero: 123 | 1.5 | .5 | 1e-3 | 0xFF */
    if ((L->p[0] >= '0' && L->p[0] <= '9') ||
        (L->p[0] == '.' && L->p + 1 < L->end && L->p[1] >= '0' && L->p[1] <= '9')) {
        if (L->p[0] == '0' && L->p + 1 < L->end &&
            (L->p[1] == 'x' || L->p[1] == 'X')) {
            L->p += 2;
            while (L->p < L->end &&
                   ((*L->p >= '0' && *L->p <= '9') ||
                    (*L->p >= 'a' && *L->p <= 'f') ||
                    (*L->p >= 'A' && *L->p <= 'F')))
                L->p++;
        } else {
            while (L->p < L->end && *L->p >= '0' && *L->p <= '9') L->p++;
            if (L->p < L->end && *L->p == '.') {
                L->p++;
                while (L->p < L->end && *L->p >= '0' && *L->p <= '9') L->p++;
            }
            if (L->p < L->end && (*L->p == 'e' || *L->p == 'E')) {
                const char *q = L->p + 1;
                if (q < L->end && (*q == '+' || *q == '-')) q++;
                if (q < L->end && *q >= '0' && *q <= '9') {
                    L->p = q;
                    while (L->p < L->end && *L->p >= '0' && *L->p <= '9') L->p++;
                }
            }
        }
        L->t = T_NUM;
        L->tl = (int)(L->p - L->ts);
        L->col += L->tl;
        return;
    }

    /* string "..." ou '...' (escapes: \x consome 2 chars) */
    if (L->p[0] == '"' || L->p[0] == '\'') {
        char q = L->p[0];
        L->p++;
        while (L->p < L->end && *L->p != q) {
            if (*L->p == '\\' && L->p + 1 < L->end) {
                if (L->p[1] == '\n') { L->line++; L->col = 0; }
                L->p += 2;
            } else {
                if (*L->p == '\n') { L->line++; L->col = 0; }
                L->p++;
            }
        }
        if (L->p < L->end) L->p++;  /* fecha aspas */
        L->t = T_STR;
        L->tl = (int)(L->p - L->ts);
        L->col += L->tl;
        return;
    }

#define EMIT(ch, ty) \
    do { L->p++; L->col++; L->t = (ty); L->tl = 1; return; } while (0)
#define EMIT2(ty) \
    do { L->p += 2; L->col += 2; L->t = (ty); L->tl = 2; return; } while (0)

    s = L->p;
    switch (s[0]) {
    case '{': EMIT('{', T_LBRACE);
    case '}': EMIT('}', T_RBRACE);
    case '(': EMIT('(', T_LP);
    case ')': EMIT(')', T_RP);
    case ';': EMIT(';', T_SEMI);
    case ',': EMIT(',', T_COMMA);
    case '?': EMIT('?', T_QUEST);
    case '#': EMIT('#', T_HASH);
    case '~': EMIT('~', T_TILDE);
    case '[':
        if (L->p + 1 < L->end && L->p[1] == '[') EMIT2(T_LBRB);
        EMIT('[', T_LB);
    case ']':
        /* ']]' NAO e token: em a[b[c]] os dois colchetes finais sao
         * independentes; o fechamento do ponteiro [[ ]] sao dois ']'. */
        EMIT(']', T_RB);
    case ':':
        if (L->p + 1 < L->end && L->p[1] == ':') EMIT2(T_DCOLON);
        EMIT(':', T_COLON);
    case '.':
        EMIT('.', T_DOT);   /* .5 ja foi pego pelo numero acima */
    case '!':
        if (L->p + 1 < L->end && L->p[1] == '=') EMIT2(T_NE);
        EMIT('!', T_NOT);
    case '=':
        if (L->p + 1 < L->end && L->p[1] == '=') EMIT2(T_EQ);
        EMIT('=', T_ASSIGN);
    case '<':
        if (L->p + 2 < L->end && L->p[1] == '<' && L->p[2] == '=') {
            L->p += 3; L->col += 3; L->t = T_SHLAS; L->tl = 3; return;
        }
        if (L->p + 1 < L->end && L->p[1] == '<') EMIT2(T_SHL);
        if (L->p + 1 < L->end && L->p[1] == '=') EMIT2(T_LE);
        EMIT('<', T_LT);
    case '>':
        if (L->p + 2 < L->end && L->p[1] == '>' && L->p[2] == '=') {
            L->p += 3; L->col += 3; L->t = T_SHRAS; L->tl = 3; return;
        }
        if (L->p + 1 < L->end && L->p[1] == '>') EMIT2(T_SHR);
        if (L->p + 1 < L->end && L->p[1] == '=') EMIT2(T_GE);
        EMIT('>', T_GT);
    case '&':
        if (L->p + 1 < L->end && L->p[1] == '&') EMIT2(T_AND);
        if (L->p + 1 < L->end && L->p[1] == '=') EMIT2(T_ANDAS);
        EMIT('&', T_AMP);
    case '|':
        if (L->p + 1 < L->end && L->p[1] == '|') EMIT2(T_OR);
        if (L->p + 1 < L->end && L->p[1] == '=') EMIT2(T_ORAS);
        EMIT('|', T_PIPE);
    case '^':
        if (L->p + 1 < L->end && L->p[1] == '=') EMIT2(T_XORAS);
        EMIT('^', T_CARET);
    case '+':
        if (L->p + 1 < L->end && L->p[1] == '+') EMIT2(T_INC);
        if (L->p + 1 < L->end && L->p[1] == '=') EMIT2(T_PLUSAS);
        EMIT('+', T_PLUS);
    case '-':
        if (L->p + 1 < L->end && L->p[1] == '-') EMIT2(T_DEC);
        if (L->p + 1 < L->end && L->p[1] == '=') EMIT2(T_MINUSAS);
        EMIT('-', T_MINUS);
    case '*':
        if (L->p + 1 < L->end && L->p[1] == '=') EMIT2(T_STARAS);
        EMIT('*', T_STAR);
    case '/':
        if (L->p + 1 < L->end && L->p[1] == '=') EMIT2(T_SLASHAS);
        EMIT('/', T_SLASH);
    case '%':
        if (L->p + 1 < L->end && L->p[1] == '=') EMIT2(T_PCTAS);
        EMIT('%', T_PCT);
    default:
        /* caractere fora da grammatica: erro (evita loop/parse silencioso) */
        failp(L, "caractere nao reconhecido no script");
    }
#undef EMIT
#undef EMIT2
}

/* --------------------------------------------------------------- parser -- */

static void failp(Lex *L, const char *msg)
{
    snprintf(L->g->err, sizeof(L->g->err), "%s", msg);
    L->g->err_line = L->tline;
    L->g->err_col = L->tcol;
    longjmp(L->jb, 1);
}

static int  at(Lex *L, TType t) { return L->t == t; }
static void adv(Lex *L)         { scan(L); }

static int is_id(Lex *L, const char *s)
{
    size_t n = strlen(s);
    return L->t == T_ID && (size_t)L->tl == n && strncmp(L->ts, s, n) == 0;
}

static int eat(Lex *L, TType t)
{
    if (L->t == t) { scan(L); return 1; }
    return 0;
}

/* Qual e o proximo token (lookahead de 1)? */
static int peek_is(Lex *L, TType ty)
{
    Lex t = *L;
    adv(&t);
    return t.t == ty;
}

static void need(Lex *L, TType t, const char *what)
{
    if (!eat(L, t)) failp(L, what);
}

static void parse_expr(Lex *L);
static void parse_args(Lex *L);

static void parse_args(Lex *L)
{
    if (!at(L, T_RP)) {
        parse_expr(L);
        while (eat(L, T_COMMA)) parse_expr(L);
    }
}

static void parse_primary(Lex *L)
{
    switch (L->t) {
    case T_NUM:
    case T_STR:
        adv(L);
        return;
    case T_ID:
        adv(L);
        return;
    case T_LP:
        adv(L);
        /* (a, b, c) e legal em GSC: retorna multiplos valores */
        if (!at(L, T_RP)) {
            parse_expr(L);
            while (eat(L, T_COMMA)) parse_expr(L);
        }
        need(L, T_RP, "esperava ')'");
        return;
    case T_LB:                    /* [] e [x] = literal de array */
        adv(L);
        if (!at(L, T_RB)) {
            parse_expr(L);
            while (eat(L, T_COMMA)) parse_expr(L);
        }
        need(L, T_RB, "esperava ']'");
        return;
    case T_PCT:                   /* %anim_name = referencia de animacao */
        adv(L);
        if (at(L, T_ID) || at(L, T_NUM)) adv(L);
        else failp(L, "esperava nome de animacao apos '%'");
        return;
    case T_HASH:                  /* #animtree = referencia global de arvores */
        adv(L);
        if (at(L, T_ID) || at(L, T_NUM)) adv(L);
        else failp(L, "esperava nome apos '#'");
        return;
    case T_DCOLON:                 /* ::funcao -> referencia literal de funcao
                                    * (ex.: array_thread(arr, ::spawn_one)) */
        adv(L);
        if (!at(L, T_ID)) failp(L, "esperava nome apos '::'");
        adv(L);
        if (eat(L, T_LP)) {
            parse_args(L);
            need(L, T_RP, "esperava ')'");
            L->g->calls++;
        }
        return;
    case T_LBRB:                 /* [[ funcao_ptr ]] (args) */
        adv(L);
        parse_expr(L);
        need(L, T_RB, "esperava ']' (ponteiro de funcao)");
        need(L, T_RB, "esperava ']]' (ponteiro de funcao)");
        need(L, T_LP, "ponteiro de funcao deve ser chamado: [[fn]]()");
        parse_args(L);
        need(L, T_RP, "esperava ')'");
        L->g->calls++;
        return;
    default:
        failp(L, "esperava expressao");
    }
}

static void parse_postfix_tail(Lex *L)
{
    for (;;) {
        if (at(L, T_LP)) {                    /* chamada */
            adv(L);
            parse_args(L);
            need(L, T_RP, "esperava ')'");
            L->g->calls++;
        } else if (at(L, T_LB)) {             /* índice */
            adv(L);
            parse_expr(L);
            need(L, T_RB, "esperava ']'");
        } else if (at(L, T_DOT)) {            /* membro */
            adv(L);
            if (!at(L, T_ID)) failp(L, "esperava nome apos '.'");
            adv(L);
        } else if (at(L, T_DCOLON)) {         /* namespace: maps\x::f() */
            adv(L);
            if (!at(L, T_ID)) failp(L, "esperava funcao apos '::'");
            adv(L);
            if (eat(L, T_LP)) {
                parse_args(L);
                need(L, T_RP, "esperava ')'");
                L->g->calls++;
            }
        } else if (at(L, T_LBRB)) {           /* [[fn]]() infixo */
            adv(L);
            parse_expr(L);
            need(L, T_RB, "esperava ']'");
            need(L, T_RB, "esperava ']]'");
            need(L, T_LP, "ponteiro de funcao deve ser chamado");
            parse_args(L);
            need(L, T_RP, "esperava ')'");
            L->g->calls++;
        } else if (at(L, T_INC) || at(L, T_DEC)) {
            adv(L);
        } else if (at(L, T_ID) &&
                   (peek_is(L, T_LP) || peek_is(L, T_DCOLON) ||
                    peek_is(L, T_DOT) || is_id(L, "thread"))) {
            /* chamada "injetada" no objeto, legal em qualquer expressao:
             *   if ( level.friends array_contains_element( self ) )
             *   if ( self maps\_laststand::player_is_in_laststand() )
             *   if ( other thread friendly_mg42_useable( mg42, node ) )
             *   endPos = self endnode.origin + ( 0, 0, 1 );                 */
            adv(L);
            parse_postfix_tail(L);   /* .membro / [i] ::f / chamadas */
            if (at(L, T_LP)) {
                adv(L);
                parse_args(L);
                need(L, T_RP, "esperava ')'");
                L->g->calls++;
            }
        } else break;
    }
}

static void parse_postfix(Lex *L)
{
    parse_primary(L);
    parse_postfix_tail(L);
}

static void parse_unary(Lex *L)
{
    if (at(L, T_NOT) || at(L, T_TILDE) || at(L, T_PLUS) || at(L, T_MINUS) ||
        at(L, T_INC) || at(L, T_DEC) || at(L, T_AMP)) {
        /* '&' tambem e unario: SaveGame( "x", &"LOCALIZE_KEY", ... ) */
        adv(L);
        parse_unary(L);
        return;
    }
    parse_postfix(L);
}

#define BINLEVEL(name, next, op1, op2, op3)                    \
    static void name(Lex *L)                                   \
    {                                                          \
        next(L);                                               \
        /* !at(T_EOF) evita loop infinito se o arquivo corta    \
         * no meio da expressao (T_EOF nunca e operador). */    \
        while (!at(L, T_EOF) &&                                 \
               (at(L, op1) || at(L, op2) || at(L, op3))) {     \
            adv(L);                                            \
            next(L);                                           \
        }                                                      \
    }

/* Niveis binarios (o 3o argumento e T_EOF = nunca casa). */
BINLEVEL(parse_mul,    parse_unary, T_STAR, T_SLASH, T_PCT)
BINLEVEL(parse_add,    parse_mul,   T_PLUS, T_MINUS, T_EOF)
BINLEVEL(parse_shift,  parse_add,   T_SHL,  T_SHR,   T_EOF)
BINLEVEL(parse_rel,    parse_shift, T_LT,   T_GT,    T_LE)
static void parse_rel2(Lex *L)
{
    parse_rel(L);
    while (at(L, T_GE)) { adv(L); parse_rel(L); }
}
BINLEVEL(parse_eq,     parse_rel2,  T_EQ,   T_NE,    T_EOF)
BINLEVEL(parse_band,   parse_eq,    T_AMP,  T_EOF,   T_EOF)
BINLEVEL(parse_bxor,   parse_band,  T_CARET, T_EOF,  T_EOF)
BINLEVEL(parse_bor,    parse_bxor,  T_PIPE, T_EOF,   T_EOF)
BINLEVEL(parse_land,   parse_bor,   T_AND,  T_EOF,   T_EOF)
BINLEVEL(parse_lor,    parse_land,  T_OR,   T_EOF,   T_EOF)
#undef BINLEVEL

static void parse_ternary(Lex *L)
{
    parse_lor(L);
    if (at(L, T_QUEST)) {
        adv(L);
        parse_expr(L);
        need(L, T_COLON, "esperava ':' no operador ?:");
        parse_ternary(L);
    }
}

static void parse_assign(Lex *L)
{
    parse_ternary(L);
    if (at(L, T_ASSIGN) || at(L, T_PLUSAS) || at(L, T_MINUSAS) ||
        at(L, T_STARAS) || at(L, T_SLASHAS) || at(L, T_PCTAS) ||
        at(L, T_ANDAS) || at(L, T_ORAS) || at(L, T_XORAS) ||
        at(L, T_SHLAS) || at(L, T_SHRAS)) {
        adv(L);
        parse_assign(L);        /* associativo a direita */
    }
}

static void parse_expr(Lex *L) { parse_assign(L); }

static void parse_stmt(Lex *L);
static void parse_block(Lex *L);

static void parse_stmt(Lex *L)
{
    L->g->stmts++;

    if (at(L, T_LBRACE)) { parse_block(L); return; }
    if (eat(L, T_SEMI)) return;

    if (is_id(L, "if")) {
        adv(L);
        need(L, T_LP, "esperava '(' apos if");
        parse_expr(L);
        need(L, T_RP, "esperava ')' do if");
        parse_stmt(L);
        if (is_id(L, "else")) { adv(L); parse_stmt(L); }
        return;
    }
    if (is_id(L, "for")) {
        adv(L);
        need(L, T_LP, "esperava '(' apos for");
        if (!at(L, T_SEMI)) parse_expr(L);
        need(L, T_SEMI, "esperava ';' no for");
        if (!at(L, T_SEMI)) parse_expr(L);
        need(L, T_SEMI, "esperava ';' no for");
        if (!at(L, T_RP)) parse_expr(L);
        need(L, T_RP, "esperava ')' do for");
        parse_stmt(L);
        return;
    }
    if (is_id(L, "while")) {
        adv(L);
        need(L, T_LP, "esperava '(' apos while");
        parse_expr(L);
        need(L, T_RP, "esperava ')' do while");
        parse_stmt(L);
        return;
    }
    if (is_id(L, "do")) {
        adv(L);
        parse_stmt(L);
        if (!is_id(L, "while")) failp(L, "esperava 'while' no do..while");
        adv(L);
        need(L, T_LP, "esperava '(' apos while");
        parse_expr(L);
        need(L, T_RP, "esperava ')'");
        need(L, T_SEMI, "esperava ';' no do..while");
        return;
    }
    if (is_id(L, "switch")) {
        adv(L);
        need(L, T_LP, "esperava '(' apos switch");
        parse_expr(L);
        need(L, T_RP, "esperava ')' do switch");
        need(L, T_LBRACE, "esperava '{' do switch");
        while (!at(L, T_RBRACE) && !at(L, T_EOF)) {
            if (is_id(L, "case"))      { adv(L); parse_expr(L); }
            else if (is_id(L, "default")) adv(L);
            else failp(L, "esperava 'case' ou 'default'");
            need(L, T_COLON, "esperava ':' apos case/default");
            while (!at(L, T_RBRACE) && !at(L, T_EOF) &&
                   !is_id(L, "case") && !is_id(L, "default"))
                parse_stmt(L);
        }
        need(L, T_RBRACE, "esperava '}' do switch");
        return;
    }
    if (is_id(L, "break") || is_id(L, "continue")) {
        adv(L);
        need(L, T_SEMI, "esperava ';' apos break/continue");
        return;
    }
    if (is_id(L, "return")) {
        adv(L);
        if (!at(L, T_SEMI)) parse_expr(L);
        need(L, T_SEMI, "esperava ';' apos return");
        return;
    }
    if (is_id(L, "wait")) {
        adv(L);
        parse_expr(L);
        need(L, T_SEMI, "esperava ';' apos wait");
        return;
    }
    /* include sem '#': "include maps\\mp\\_utility;" (existe no jogo) */
    if (is_id(L, "include")) {
        L->g->includes++;
        adv(L);
        if (at(L, T_ID) || at(L, T_STR)) adv(L);
        else failp(L, "esperava caminho apos include");
        eat(L, T_SEMI);
        return;
    }

    /* expressao, com "enxertos" estilo GSC:
     *   self waittill ("death");          -> expr ID expr
     *   level thread zombie_spawn_init(); -> expr ID expr               */
    /* chamadas enxertadas (self waittill(..), level thread f(), ...) sao
     * tratadas dentro da propria expressao (ver parse_postfix_tail) */
    parse_expr(L);
    if (at(L, T_SEMI)) { adv(L); return; }
    /* Os scripts do jogo tem ";' omitido antes da quebra de linha
     * (ex.: animscripts/traverse/stairs_down.gsc) -> aceita. */
    if (L->tline != L->prev_line) return;
    failp(L, "esperava ';' ao final da instrucao");
}

static void parse_block(Lex *L)
{
    need(L, T_LBRACE, "esperava '{'");
    while (!at(L, T_RBRACE) && !at(L, T_EOF)) parse_stmt(L);
    need(L, T_RBRACE, "esperava '}'");
}

/* Eh `nome ( params ) {` (definicao de funcao de topo)? Com lookahead. */
static int looks_like_funcdef(Lex *L)
{
    Lex t = *L;                     /* copia do estado do lexer */

    adv(&t);                        /* passa o nome */
    if (!at(&t, T_LP)) return 0;
    adv(&t);
    {
        int depth = 1;
        while (depth > 0) {
            if (at(&t, T_EOF) || at(&t, T_SEMI) || at(&t, T_LBRACE)) return 0;
            if (at(&t, T_LP)) depth++;
            else if (at(&t, T_RP)) depth--;
            if (depth == 0) break;
            adv(&t);
        }
    }
    adv(&t);
    return at(&t, T_LBRACE);
}

/* Pula uma diretiva: come tokens ate ';' ou fim da linha. */
static void skip_directive(Lex *L)
{
    int dl = L->tline;

    while (!at(L, T_EOF) && !at(L, T_SEMI) && L->tline == dl) adv(L);
    eat(L, T_SEMI);
}

static void parse_directive(Lex *L)
{
    adv(L);                                  /* passa '#' */
    if (!at(L, T_ID)) failp(L, "esperava nome de diretiva apos '#'");

    if (is_id(L, "include")) {
        L->g->includes++;
        adv(L);
        if (at(L, T_ID) || at(L, T_STR)) adv(L);
        else failp(L, "esperava caminho apos #include");
        eat(L, T_SEMI);
        return;
    }
    if (is_id(L, "using_animtree")) {
        L->g->includes++;
        adv(L);
        need(L, T_LP, "esperava '(' apos #using_animtree");
        /* no jogo real: #using_animtree( "generic_human" ); e tambem
         * #using_animtree( 0 ); -> aceita texto, numero ou nome */
        if (at(L, T_NUM) || at(L, T_STR) || at(L, T_ID)) adv(L);
        else failp(L, "esperava numero/texto em #using_animtree");
        need(L, T_RP, "esperava ')'");
        eat(L, T_SEMI);
        return;
    }
    /* #define e diretils desconhecidas: pula a linha */
    skip_directive(L);
}

static void parse_program(Lex *L)
{
    while (!at(L, T_EOF)) {
        if (at(L, T_HASH)) { parse_directive(L); continue; }
        /* colchete-estrela sobrando no fim de arquivo existe em scripts
         * reais (comentario orfao) -> pula */
        if (at(L, T_STAR) && peek_is(L, T_SLASH)) { adv(L); adv(L); continue; }
        if (at(L, T_ID) && looks_like_funcdef(L)) {
            adv(L);                        /* nome da funcao */
            need(L, T_LP, "esperava '(' na definicao de funcao");
            if (!at(L, T_RP)) {            /* parametros */
                if (!at(L, T_ID)) failp(L, "esperava parametro (nome)");
                adv(L);
                while (eat(L, T_COMMA)) {
                    if (!at(L, T_ID)) failp(L, "esperava parametro (nome)");
                    adv(L);
                }
            }
            need(L, T_RP, "esperava ')'");
            L->g->funcs++;
            parse_block(L);
            continue;
        }
        parse_stmt(L);                     /* tolerancia: stmt no topo */
    }
}

/* ------------------------------------------------------------- API ------ */

const char *t4_gsc_error(const T4GSC *g)
{
    return (g && g->err[0]) ? g->err : "";
}

int t4_gsc_parse_mem(T4GSC *g, const char *name, const char *buf, size_t len)
{
    Lex L;

    if (!g || !buf) return -1;
    memset(g, 0, sizeof(*g));
    g->path = name ? name : "<mem>";

    memset(&L, 0, sizeof(L));
    L.p = buf;
    L.end = buf + len;
    L.line = 1;
    L.col = 1;
    L.g = g;

    if (setjmp(L.jb) != 0) return -1;      /* failp() pulou aqui */

    scan(&L);
    parse_program(&L);

    g->lines = (size_t)L.line;
    return 0;
}

int t4_gsc_parse_file(T4GSC *g, const char *path)
{
    FILE *f;
    char *buf;
    long n;
    int rc;

    if (!g || !path) return -1;
    memset(g, 0, sizeof(*g));
    g->path = path;

    f = fopen(path, "rb");
    if (!f) {
        snprintf(g->err, sizeof(g->err), "nao abriu o arquivo");
        g->err_line = 1;
        g->err_col = 1;
        return -1;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (n = ftell(f)) < 0) {
        fclose(f);
        snprintf(g->err, sizeof(g->err), "nao mediu o arquivo");
        return -1;
    }
    rewind(f);
    buf = (char *)malloc((size_t)n + 1);
    if (!buf) {
        fclose(f);
        snprintf(g->err, sizeof(g->err), "sem memoria");
        return -1;
    }
    {
        size_t rd = fread(buf, 1, (size_t)n, f);
        buf[rd] = '\0';
        fclose(f);
        rc = t4_gsc_parse_mem(g, path, buf, rd);
    }
    free(buf);
    return rc;
}
