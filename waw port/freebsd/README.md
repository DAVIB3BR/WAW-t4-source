# T4Port — FreeBSD

<p align="center">
  <img src="../../doc/images/loadscreen_zombie_asylum.jpg" width="420"
       alt="Call of Duty: World at War — Nazi Zombies asylum load screen">
  <br>
  <sub><i>Call of Duty: World at War</i> — Nazi Zombies asylum load screen
  (<code>images/loadscreen_zombie_asylum.iwi</code>), decoded by this port —
  © Activision/Treyarch, local/personal documentation use only.</sub>
</p>

## Requirements (once)

```sh
pkg install git cmake ninja zlib
pkg install sdl2          # optional; without it dlopen is used after installing
pkg install ffmpeg        # optional; only for "t4port video" (.bik)
```

## Build

```sh
./build.sh          # cmake + ninja + smoke test
```

## Why the code should already compile

The core is C11 + POSIX, and FreeBSD is POSIX like Linux. The only real
differences are already isolated in `src/platform.c`:

```c
#elif defined(__FreeBSD__) || defined(__DragonFly__)
    sysctl({CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1}, ...)   /* exe path */
```

Everything else that varies per OS (`_WIN32`) is the Windows path, not
FreeBSD's: `net.c` (sockets), `sdl2_dyn.c` (`dlopen`), `mods.c`
(`mkdir(p,0755)`) and `main.c` (`fork/exec`) are identical to the Linux ones.

## LAN on FreeBSD

`lan host` / `lan discover` use `getifaddrs()` + directed broadcast — both
available on FreeBSD, so the broadcast leaves through the real Ethernet
interface (e.g. `192.168.100.255`), same as on Linux.

## Status

✅ **Cross-built here (Linux) as a real FreeBSD ELF** — `./cross-build.sh`
compiles every source with `zig cc -target x86_64-freebsd` (headers from
`platforms/freebsd/.sysroot`, extracted from FreeBSD `base.txz`) and links
against the sysroot's own `crt1.o`/`libc.so.7`/`libm.so.5`. The result in
`dist/freebsd/t4port` is `ELF 64-bit ... version 1 (FreeBSD), dynamically
linked, interpreter /libexec/ld-elf.so.1, for FreeBSD 14.5` with
`NEEDED libc.so.7`/`libm.so.5` — a genuine FreeBSD binary, nothing emulated.
Runtime validation still needs a real FreeBSD machine (`./t4port --check`);
`./build.sh` remains the native build when you are already on FreeBSD.
DragonFly BSD should work (same `sysctl` branch).
