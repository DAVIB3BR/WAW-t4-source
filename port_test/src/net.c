/*
 * net.c - camada LAN do t4port (protocolo proprio "T4LAN1" sobre UDP/IPv4).
 *
 * A descoberta e enviada por BROADCAST REAL de cada interface ativa (via
 * getifaddrs -> ifa_broadaddr), alem de 255.255.255.255, 127.0.0.1 e um alvo
 * unicast opcional - ou seja, funciona na Ethernet da LAN, nao so em loopback.
 *
 * Portabilidade: POSIX puro (Linux e macOS). Sem SDL, sem threads.
 */
#include "net.h"

#include <errno.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#if defined(_WIN32)
/* ---- Windows: Winsock2 (o resto do nucleo nao muda) ---- */
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
typedef SOCKET t4_sock;
#define sock_close     closesocket
#define sock_errno     WSAGetLastError()
#define SOCK_EINTR     WSAEINTR
#define SOCK_INVALID   INVALID_SOCKET
static int g_wsa;
static int sock_start(void)
{
    WSADATA wsa;
    if (g_wsa) return 0;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return -1;
    g_wsa = 1;
    return 0;
}
static void sock_stop(void)
{
    if (g_wsa) { WSACleanup(); g_wsa = 0; }
}
#else
/* ---- POSIX: Linux, macOS, FreeBSD (identicos aqui) ---- */
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
typedef int t4_sock;
#define sock_close     close
#define sock_errno     errno
#define SOCK_EINTR     EINTR
#define SOCK_INVALID   (-1)
static int sock_start(void) { return 0; }
static void sock_stop(void) { }
#endif

static char g_nerr[512];

const char *t4_lan_error(void) { return g_nerr; }

static int nfail(const char *msg)
{
    snprintf(g_nerr, sizeof(g_nerr), "%s%s%s", msg ? msg : "",
             (msg && errno) ? " " : "", errno ? strerror(errno) : "");
    errno = 0;
    return -1;
}

/* Falha de socket: codigo serve tanto para errno POSIX (<1000) quanto para
 * WSAE* do Winsock (no Windows strerror nao os conhece). */
static int nsock(const char *msg, int code)
{
    if (code > 0 && code < 1000)
        snprintf(g_nerr, sizeof(g_nerr), "%s: %s", msg, strerror(code));
    else
        snprintf(g_nerr, sizeof(g_nerr), "%s: erro de socket %d", msg, code);
    errno = 0;
    return -1;
}

static int64_t now_ms(void)
{
#if defined(_WIN32)
    return (int64_t)GetTickCount64();
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (int64_t)ts.tv_sec * 1000 + (int64_t)ts.tv_nsec / 1000000;
#endif
}

/* ------------------------------------------------------------------ wire -- */

static const uint8_t PKT_QUERY[] = {
    'T', '4', 'L', 'A', 'N', '1', 0, 'Q', 'U', 'E', 'R', 'Y', 0
};

/* T4LAN1 \0 INFO \0 nome\0 mapa\0 modo\0 "jog/max"\0 proto\0 */
static int build_info(uint8_t *b, size_t cap, const T4LanInfo *in)
{
    char players[24];
    int n;

    snprintf(players, sizeof(players), "%d/%d",
             in->players < 0 ? 0 : in->players,
             in->max_players < 0 ? 0 : in->max_players);
    n = snprintf((char *)b, cap, "%s%cINFO%c%s%c%s%c%s%c%s%c%s",
                 T4LAN_MAGIC, 0, 0,
                 in->name[0] ? in->name : "t4port", 0,
                 in->map[0] ? in->map : "-", 0,
                 in->mode[0] ? in->mode : "-", 0,
                 players, 0, T4LAN_PROTO);
    if (n < 0 || (size_t)n + 1 > cap) return -1;
    return n + 1; /* inclui o NUL final */
}

/* Copia o campo NUL-terminado a partir de *i; 0 se acabou/invalido. */
static int next_field(const uint8_t *p, size_t len, size_t *i,
                      char *dst, size_t dstsz)
{
    size_t s = *i, e, n;
    for (e = s; e < len && p[e] != 0; e++) { }
    if (e >= len) return 0;
    n = e - s;
    if (n >= dstsz) n = dstsz - 1;
    memcpy(dst, p + s, n);
    dst[n] = '\0';
    *i = e + 1;
    return 1;
}

static int parse_info(const uint8_t *p, size_t len, const char *ip, int port,
                      T4LanHost *h)
{
    size_t i = T4LAN_MAGIC_LEN;

    if (len < T4LAN_MAGIC_LEN + 6) return -1;
    if (memcmp(p, T4LAN_MAGIC, T4LAN_MAGIC_LEN) != 0 || p[i] != 0) return -1;
    i++;
    if (i + 5 > len || memcmp(p + i, "INFO", 4) != 0 || p[i + 4] != 0)
        return -1;
    i += 5;

    memset(h, 0, sizeof(*h));
    snprintf(h->ip, sizeof(h->ip), "%s", ip);
    h->port = port;
    if (!next_field(p, len, &i, h->name, sizeof(h->name)) ||
        !next_field(p, len, &i, h->map, sizeof(h->map)) ||
        !next_field(p, len, &i, h->mode, sizeof(h->mode)) ||
        !next_field(p, len, &i, h->players, sizeof(h->players)) ||
        !next_field(p, len, &i, h->proto, sizeof(h->proto)))
        return -1;
    return 0;
}

/* ----------------------------------------------------------------- host -- */

static volatile sig_atomic_t g_stop;

static void on_stop(int sig) { (void)sig; g_stop = 1; }

int t4_lan_host(const T4LanInfo *info, int port,
                void (*on_query)(const char *, int, void *), void *ud)
{
    t4_sock fd;
    int one = 1, nsrc = 0;
    struct sockaddr_in me;
    uint32_t seen_addr[T4LAN_MAX_HOSTS];
    uint16_t seen_port[T4LAN_MAX_HOSTS];

    g_nerr[0] = '\0';
    g_stop = 0;
    if (!info || port <= 0 || port > 65535)
        return nfail("lan host: parametros invalidos");
    if (sock_start() != 0) return nsock("WSAStartup", sock_errno);

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == SOCK_INVALID) return nsock("socket", sock_errno);
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&one, sizeof(one));

    memset(&me, 0, sizeof(me));
    me.sin_family = AF_INET;
    me.sin_port = htons((uint16_t)port);
    me.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(fd, (struct sockaddr *)&me, sizeof(me)) < 0) {
        int r = nsock("bind udp", sock_errno);
        sock_close(fd);
        return r;
    }

#if defined(_WIN32)
    /* Windows nao tem sigaction; signal() cobre Ctrl+C/encerramento. */
    signal(SIGINT, on_stop);
    signal(SIGTERM, on_stop);
#else
    {
        struct sigaction sa;
        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = on_stop;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0; /* sem SA_RESTART: recvfrom deve ser interrompido */
        sigaction(SIGINT, &sa, NULL);
        sigaction(SIGTERM, &sa, NULL);
    }
#endif

    for (;;) {
        uint8_t buf[512], out[512];
        struct sockaddr_in src;
        socklen_t slen = sizeof(src);
        ssize_t n;
        int k, dup = 0;
        char ip[INET_ADDRSTRLEN];

        n = recvfrom(fd, (char *)buf, sizeof(buf), 0, (struct sockaddr *)&src, &slen);
        if (n < 0) {
            if (sock_errno == SOCK_EINTR) break; /* Ctrl+C / kill */
            k = nsock("recvfrom", sock_errno);
            sock_close(fd);
            return k;
        }
        if ((size_t)n < sizeof(PKT_QUERY)) continue;
        if (memcmp(buf, PKT_QUERY, sizeof(PKT_QUERY)) != 0) continue;

        inet_ntop(AF_INET, &src.sin_addr, ip, sizeof(ip));

        /* Uma unica notificacao por cliente distinto. */
        for (k = 0; k < nsrc; k++)
            if (seen_addr[k] == src.sin_addr.s_addr &&
                seen_port[k] == src.sin_port)
                dup = 1;
        if (!dup && nsrc < T4LAN_MAX_HOSTS) {
            seen_addr[nsrc] = src.sin_addr.s_addr;
            seen_port[nsrc] = src.sin_port;
            nsrc++;
            if (on_query)
                on_query(ip, (int)ntohs(src.sin_port), ud);
        }

        k = build_info(out, sizeof(out), info);
        if (k > 0)
            sendto(fd, (const char *)out, (size_t)k, 0, (struct sockaddr *)&src, slen);
    }

    sock_close(fd);
    sock_stop();
    return 0;
}

/* ------------------------------------------------------------- discover -- */

static int push_target(struct sockaddr_in *t, int *nt, uint32_t addr, int port)
{
    int i;
    if (*nt >= 16) return 0;
    for (i = 0; i < *nt; i++)
        if (t[i].sin_addr.s_addr == addr && t[i].sin_port == htons((uint16_t)port))
            return 0;
    memset(&t[*nt], 0, sizeof(t[*nt]));
    t[*nt].sin_family = AF_INET;
    t[*nt].sin_port = htons((uint16_t)port);
    t[*nt].sin_addr.s_addr = addr;
    (*nt)++;
    return 1;
}

int t4_lan_discover(int port, int wait_ms, const char *only_ip,
                    T4LanHost **out, int *count)
{
    t4_sock fd;
    int one = 1, nt = 0, sent = 0, nres = 0, i;
    struct sockaddr_in targets[16];
#if !defined(_WIN32)
    struct ifaddrs *ifa = NULL, *p;
#endif
    T4LanHost *res = NULL;
    int64_t deadline;

    g_nerr[0] = '\0';
    if (out) *out = NULL;
    if (count) *count = 0;
    if (port <= 0 || port > 65535 || wait_ms < 0) return nfail("lan discover: parametros");
    if (sock_start() != 0) return nsock("WSAStartup", sock_errno);

    /* Alvos: alvo explicito, senao broadcast da LAN + loopback. */
    if (only_ip && only_ip[0]) {
        struct in_addr a;
        if (inet_pton(AF_INET, only_ip, &a) != 1)
            return nfail("lan discover: --to invalido");
        push_target(targets, &nt, a.s_addr, port);
    } else {
        push_target(targets, &nt, htonl(INADDR_BROADCAST), port);
        push_target(targets, &nt, htonl(INADDR_LOOPBACK), port);
#if !defined(_WIN32)
        /* Broadcast dirigido de cada interface ativa (Ethernet/Wi-Fi).
         * No Windows nao ha getifaddrs em mingw -> 255.255.255.255 + loopback. */
        if (getifaddrs(&ifa) == 0) {
            for (p = ifa; p; p = p->ifa_next) {
                struct sockaddr_in *b;
                if (!p->ifa_addr || p->ifa_addr->sa_family != AF_INET) continue;
                if (!p->ifa_broadaddr || p->ifa_broadaddr->sa_family != AF_INET)
                    continue;
                if (!(p->ifa_flags & IFF_UP) || !(p->ifa_flags & IFF_BROADCAST))
                    continue;
                b = (struct sockaddr_in *)p->ifa_broadaddr;
                if (b->sin_addr.s_addr == 0) continue;
                push_target(targets, &nt, b->sin_addr.s_addr, port);
            }
        }
#endif
    }

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == SOCK_INVALID) {
#if !defined(_WIN32)
        freeifaddrs(ifa);
#endif
        return nsock("socket", sock_errno);
    }
    setsockopt(fd, SOL_SOCKET, SO_BROADCAST, (const char *)&one, sizeof(one));

    for (i = 0; i < nt; i++)
        if (sendto(fd, (const char *)PKT_QUERY, sizeof(PKT_QUERY), 0,
                   (struct sockaddr *)&targets[i], sizeof(targets[i])) >= 0)
            sent++;
#if !defined(_WIN32)
    freeifaddrs(ifa);
#endif
    if (!sent) {
        int r = nsock("sendto (nenhum alvo aceito)", sock_errno);
        sock_close(fd);
        return r;
    }
    if (getenv("T4LAN_DEBUG")) {
        for (i = 0; i < nt; i++) {
            char a[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &targets[i].sin_addr, a, sizeof(a));
            fprintf(stderr, "[t4lan] alvo %d: %s:%d\n", i, a, port);
        }
    }

    res = (T4LanHost *)calloc(T4LAN_MAX_HOSTS, sizeof(T4LanHost));
    if (!res) {
        int r = nfail("sem memoria");
        sock_close(fd);
        return r;
    }

    deadline = now_ms() + wait_ms;
    for (;;) {
        fd_set rfds;
        struct timeval tv;
        int64_t left = deadline - now_ms();
        struct sockaddr_in src;
        socklen_t slen = sizeof(src);
        uint8_t buf[512];
        ssize_t n;
        char ip[INET_ADDRSTRLEN];
        T4LanHost h;

        if (left <= 0) break;
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);
        tv.tv_sec = (time_t)(left / 1000);
        tv.tv_usec = (long)((left % 1000) * 1000);
        if (select((int)fd + 1, &rfds, NULL, NULL, &tv) <= 0) break;

        n = recvfrom(fd, (char *)buf, sizeof(buf), 0, (struct sockaddr *)&src, &slen);
        if (n < 0) {
            if (sock_errno == SOCK_EINTR) continue;
            break;
        }
        inet_ntop(AF_INET, &src.sin_addr, ip, sizeof(ip));
        if (parse_info(buf, (size_t)n, ip, (int)ntohs(src.sin_port), &h) != 0)
            continue;
        if (h.port <= 0) h.port = port;

        {
            int dup = 0;
            for (i = 0; i < nres; i++)
                if (strcmp(res[i].ip, h.ip) == 0 && res[i].port == h.port &&
                    strcmp(res[i].name, h.name) == 0)
                    dup = 1;
            if (dup) continue;
        }
        if (nres >= T4LAN_MAX_HOSTS) continue;
        res[nres++] = h;
    }

    sock_close(fd);
    sock_stop();
    if (out) *out = res; else free(res);
    if (count) *count = nres;
    return 0;
}

/* ------------------------------------------------------------- enderecos -- */

int t4_lan_local_ips(char (*out)[16], int max)
{
    int n = 0;

    if (!out || max <= 0) return 0;
#if defined(_WIN32)
    /* Sem getifaddrs em mingw; o banner mostra so "escutando na porta".
     * (GetAdaptersAddresses pode ser adicionado depois, -liphlpapi.) */
    (void)n;
    return 0;
#else
    {
        struct ifaddrs *ifa = NULL, *p;
        if (getifaddrs(&ifa) != 0) return 0;
        for (p = ifa; p && n < max; p = p->ifa_next) {
            if (!p->ifa_addr || p->ifa_addr->sa_family != AF_INET) continue;
            if (!(p->ifa_flags & IFF_UP)) continue;
            {
                struct sockaddr_in *a = (struct sockaddr_in *)p->ifa_addr;
                if (a->sin_addr.s_addr == 0) continue;
                inet_ntop(AF_INET, &a->sin_addr, out[n], 16);
                n++;
            }
        }
        freeifaddrs(ifa);
    }
    return n;
#endif
}
