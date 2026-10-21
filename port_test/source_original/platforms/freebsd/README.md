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

⚠️ **Script and code ready; build not executed here** — this machine is
Linux/openSUSE and I have no FreeBSD to run it. The first `./build.sh` on
FreeBSD 14+ is what will confirm it; any compilation warning is contained in
`platform.c`/`net.c`. DragonFly BSD should work (same `sysctl` branch).
