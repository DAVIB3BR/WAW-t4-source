/*
 * platform.c - implementacao por SO (ver platform.h).
 */
#include "platform.h"

#include <stdio.h>
#include <string.h>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#include <stdlib.h>
#elif defined(__FreeBSD__) || defined(__DragonFly__)
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#endif

#if defined(_WIN32)

int t4_exe_path(char *buf, size_t cap)
{
    DWORD n;
    if (!buf || cap == 0) return -1;
    n = GetModuleFileNameA(NULL, buf, (DWORD)cap);
    if (n == 0 || n >= (DWORD)cap) return -1;
    return 0;
}

const char *t4_os_name(void) { return "windows"; }

#elif defined(__APPLE__)

int t4_exe_path(char *buf, size_t cap)
{
    uint32_t sz = (uint32_t)cap;
    if (!buf || cap == 0) return -1;
    if (_NSGetExecutablePath(buf, &sz) != 0) return -1;
    return 0;
}

const char *t4_os_name(void) { return "macos"; }

#elif defined(__FreeBSD__) || defined(__DragonFly__)

int t4_exe_path(char *buf, size_t cap)
{
    int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1 };
    size_t len = cap;
    if (!buf || cap == 0) return -1;
    if (sysctl(mib, 4, buf, &len, NULL, 0) != 0) return -1;
    return 0;
}

const char *t4_os_name(void) { return "freebsd"; }

#else

#include <unistd.h>

int t4_exe_path(char *buf, size_t cap)
{
    ssize_t n;
    if (!buf || cap == 0) return -1;
    n = readlink("/proc/self/exe", buf, cap - 1);
    if (n <= 0) return -1;
    buf[n] = '\0';
    return 0;
}

#if defined(__linux__)
const char *t4_os_name(void) { return "linux"; }
#else
const char *t4_os_name(void) { return "desconhecido"; }
#endif

#endif
