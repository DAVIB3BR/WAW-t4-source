# t4port — original source snapshot

This folder is a **frozen snapshot** of the port's own C11 source tree, made on
**2026-10-18**. It is the "original" (pre-graphics-layer) state of the native
port of *Call of Duty: World at War* (IW 3.0 engine, internal code "T4"), kept
for reference so the live tree in `../src/` can keep evolving without losing
this baseline.

## What is inside

| Path | What it is |
|---|---|
| `src/*.c`, `src/*.h` | The whole native core: IWD archives, fastfiles, IWI textures, GSC lexer/parser, mods pipeline, LAN/UDP, platform layer, dynamic SDL2 binding |
| `CMakeLists.txt` | CMake build (Linux/macOS/FreeBSD native + Flatpak) |
| `platforms/linux/build.sh` | `.sh` launcher build script |
| `platforms/windows/build.sh` | cross-build of `t4port.exe` with zig |
| `platforms/macos/build.sh` | macOS `.app` bundle script (needs a real Mac) |
| `platforms/freebsd/build.sh` | FreeBSD build script |

## Build (same as the live tree)

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/t4port check
```

## Scope at snapshot time

- Asset parsing: 129/129 fastfiles walked, IWD and IWI formats reversed.
- Mods: `t4port mods init|unpack|list|which` + overlay priority over `.ff`.
- GSC: phase 1 (syntax validator), 1391/1391 real scripts parse clean.
- LAN: UDP discovery protocol (T4LAN1) host/discover.
- Graphics: SDL2 dynamic binding for the texture viewer only — **no OpenGL /
  Vulkan layer yet** (that work lands afterwards in `../src/gfx*.{c,h}`).
- Game VM (phase 2), renderer and audio: not implemented yet.

© Activision/Treyarch — the game data this port reads is not included here and
must not be redistributed; local/personal use only.
