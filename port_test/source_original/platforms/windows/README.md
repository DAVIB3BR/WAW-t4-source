# T4Port — Windows (`t4port.exe`)

<p align="center">
  <img src="../../doc/images/loadscreen_sp_ber3b.jpg" width="420"
       alt="Call of Duty: World at War — Berlin load screen">
  <br>
  <sub><i>Call of Duty: World at War</i> — Berlin load screen
  (<code>images/loadscreen_sp_ber3b.iwi</code>), decoded by the port's own IWI
  decoder — © Activision/Treyarch, local/personal documentation use only.</sub>
</p>

## Build

```sh
./build.sh                    # generates ../../dist/windows/t4port.exe
```

Works on Linux, macOS, MSYS2 or FreeBSD. It tries, in order:

1. **zig** (recommended — bundled mingw toolchain, downloaded automatically)
2. `x86_64-w64-mingw32-gcc` (system mingw-w64)
3. `clang --target=x86_64-windows-gnu`

The script downloads and compiles its own static zlib (`.zlib/`, cached) when
the toolchain has no Windows `-lz`.

## Testing (without a Windows machine)

```sh
wine64 ../../dist/windows/t4port.exe --check
wine64 ../../dist/windows/t4port.exe ff <game>/zone/english/<map>.ff
wine64 ../../dist/windows/t4port.exe lan discover
wine64 ../../dist/windows/t4port.exe api          # graphics API detection
wine64 ../../dist/windows/t4port.exe gfx --segundos 1   # window test (needs SDL2.dll)
```

## What changed in the code for Windows

- `platform.c`: `GetModuleFileNameA`.
- `net.c`: Winsock2 (`WSAStartup`, `closesocket`, Winsock `char*` casts).
- `sdl2_dyn.c`: `LoadLibrary`/`GetProcAddress` (loads `SDL2.dll` from PATH).
- `main.c`: `_spawnvp` instead of `fork/exec` (Bink transcoding).
- `mods.c`: `_mkdir`.

## Status

✅ **Compiled here** (zig 0.13, PE32+ x86-64, `-Wall -Wextra` with no
warnings) and **tested under Wine**: `.ff` parsing identical to the Linux
binary (136 MB file), Winsock answering, `mods list` OK, graphics API
detection reporting `opengl32.dll` and the Vulkan loader.

⚠️ Not tested on physical Windows — still to validate there: SDL2 (`view`/
`gfx`, drop `SDL2.dll` next to the `.exe`) and `mods/` path lookup. LAN
discovery on Windows uses `255.255.255.255` + loopback (no `getifaddrs`;
`GetAdaptersAddresses` is a pending improvement).
