/*
 * net.h - camada LAN do t4port (protocolo proprio "T4LAN1" sobre UDP/IPv4).
 *
 * ESTE E O PROTOCOLO NOSSO, nao o do jogo original: o binario do motor
 * (client_mp_x86.exe / cliente T4) nao esta nesta pasta, entao o protocolo de
 * rede do WaW/Plutonium nao pode ser revertido aqui. Esta camada e a base do
 * server browser do porte: quando o loop de jogo existir, ele se apoia nesta
 * descoberta de hosts para multiplayer LAN.
 *
 * Wire format (tudo ASCII, campos separados por NUL; pacote termina em NUL):
 *
 *   QUERY  (cliente -> broadcast/unicast)      T4LAN1 \0 QUERY \0
 *   INFO   (anfitriao -> cliente, unicast)
 *          T4LAN1 \0 INFO \0 <nome>\0 <mapa>\0 <modo>\0 <jog/max>\0 <proto>\0
 *
 * Porta padrao UDP: 28960 (convencao dos servidores de CoD).
 *
 * Sem dependencias alem de POSIX (Linux e macOS).
 */
#ifndef T4PORT_NET_H
#define T4PORT_NET_H

#define T4LAN_MAGIC     "T4LAN1"
#define T4LAN_MAGIC_LEN 6
#define T4LAN_PORT      28960
#define T4LAN_PROTO     "t4port/0.1"
#define T4LAN_MAX_HOSTS 64

/* Identidade do anfitriao (usado por t4_lan_host). */
typedef struct {
    char name[64];
    char map[64];
    char mode[64];
    int  players;     /* jogadores atuais */
    int  max_players; /* capacidade */
} T4LanInfo;

/* Um host encontrado por t4_lan_discover(). */
typedef struct {
    char ip[16];       /* "a.b.c.d" */
    int  port;
    char name[64];
    char map[64];
    char mode[64];
    char players[16];  /* "2/8" */
    char proto[32];
} T4LanHost;

/*
 * Anfitriao: vincula UDP `port` e responde INFO a cada QUERY ate o sinal
 * encerrar (SIGINT/SIGTERM -> retorna 0). `on_query` (opcional) e chamado uma
 * vez por cliente distinto que nos consultou.
 * Retorna 0 ok, <0 erro (t4_lan_error()).
 */
int t4_lan_host(const T4LanInfo *info, int port,
                void (*on_query)(const char *ip, int sport, void *ud),
                void *ud);

/*
 * Descoberta: envia QUERY em broadcast (+ loopback, + broadcast de cada
 * interface) e coleta INFO por `wait_ms` ms. `only_ip` (opcional) restringe
 * o envio a um alvo unicast (testes). *out sai com malloc(); conteudo em
 * *count (0 = nenhum host). Retorna 0 ok, <0 erro.
 */
int t4_lan_discover(int port, int wait_ms, const char *only_ip,
                    T4LanHost **out, int *count);

/* Preenche *out com os IPv4 locais ativos (para o banner do anfitriao).
 * Retorna a contagem (max `max` entradas). */
int t4_lan_local_ips(char (*out)[16], int max);

const char *t4_lan_error(void);

#endif /* T4PORT_NET_H */
