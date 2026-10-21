/*
 * platform.h - trechos que diferem entre Linux, macOS, FreeBSD e Windows.
 *
 * Todo o resto do nucleo e C11 + POSIX (Linux/macOS/FreeBSD sao quase
 * identicos); aqui ficam as 4 variacoes para nao espalhar #ifdef pelo codigo.
 */
#ifndef T4PORT_PLATFORM_H
#define T4PORT_PLATFORM_H

#include <stddef.h>

/*
 * Caminho absoluto do executavel em running (0 ok, -1 erro).
 *   Linux   : readlink("/proc/self/exe")
 *   macOS   : _NSGetExecutablePath()
 *   FreeBSD : sysctl(KERN_PROC_PATHNAME)
 *   Windows : GetModuleFileNameA()
 */
int t4_exe_path(char *buf, size_t cap);

/* Nome amigavel do SO: "linux", "macos", "freebsd", "windows", "desconhecido". */
const char *t4_os_name(void);

#endif /* T4PORT_PLATFORM_H */
