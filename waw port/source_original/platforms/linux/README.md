# T4Port — Linux

## Build

```sh
./build.sh              # cmake + ninja + ./t4port --check
./build.sh --flatpak    # + packages the Flatpak (uses packaging/linux/)
```

`build.sh` delegates to the project root:

- binary: `cmake -S ../.. -B ../../build && cmake --build ../../build`
- `.sh` + Flatpak: `../../packaging/linux/build-flatpak.sh [--clean --install]`

## Packages

- `packaging/linux/t4port.sh` — launcher that finds the game data
  (`T4_GAME_DIR`, `/app/share/t4port/game`, `~/Plutonium/pluto_t4_full_game`, …).
- `packaging/linux/com.t4port.engine.yml` — Flatpak manifest (embeds SDL2
  2.32.10 + 8.2 GB of data; the user's `mods/` is **not** embedded).

## Requirements

`gcc`/`clang`, `cmake` ≥ 3.16, `ninja` (optional), `zlib`. SDL2 is optional:
without `libsdl2-dev` the build uses dlopen on the runtime `.so`.

## Status

✅ Built and tested on openSUSE Leap 15.6; `ff`, `zip`, `iwi`, `mods`, `lan`
validated; passes 129/129 `.ff` under ASan/UBSan.

<p align="center">
  <img src="../../doc/images/loadscreen_mp_makin_day.jpg" width="420"
       alt="Call of Duty: World at War — Makin Day load screen">
  <br>
  <sub><i>Call of Duty: World at War</i> — “Makin Day” load screen
  (© Activision/Treyarch), decoded natively by <code>t4port iwi</code>;
  local documentation use only.</sub>
</p>
