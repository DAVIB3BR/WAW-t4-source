# How This Project Was Built

A native, offline port/engine layer for **Call of Duty: World at War**
(Infinity Ward IW 3.0 — internal name *T4*, 2008) targeting **Linux,
macOS, Windows and FreeBSD**. No Steam, no launcher, no network dependency:
everything runs from a plain copy of the game data.

---

## 1. Languages — what runs where

| Piece | Language / tech |
|---|---|
| Original game (2008) | **C/C++** (IW 3.0 engine, x86 Windows PE), gameplay logic in **GSC** scripts executed by the game's VM, data packed in fastfiles (`.ff`) and `.iwd` archives |
| This port/engine layer | **C11** — portable, dependency-light C (no C++), built with **CMake + Ninja** |
| Graphics | **OpenGL 4.5** (core, with fallback down to 2.1), **OpenGL ES 3.2**, **Vulkan 1.0** — all bound **at runtime** (`dlopen`/`LoadLibrary`), no build-time SDK needed |
| Windowing | **SDL2**, loaded dynamically (works natively and inside the Flatpak sandbox) |
| Data compression | **zlib** (the only link-time dependency) — inflates `.ff` blocks, writes PNG screenshots |
| Research tooling | **Python 3** (`re/fftool.py`, `re/iwitool.py`, `re/gen_tables.py`, `tools/*.py`) |
| Cross builds | **zig** (mingw) for the Windows `.exe`, Apple toolchain for the `.app`, **zig + FreeBSD sysroot** for the FreeBSD ELF (`cross-build.sh`) |

**Wine is never used to run the port** — only to run the *original* Windows
client for compatibility testing and as the current way to play the full
game offline while the native renderer matures.

---

## 2. What we had to do (the process)

1. **Ground-truth inventory first.** Counted the real game data with `find`
   before writing any parser: **35 `.iwd`**, **128 `.ff`**
   (including `zone/<language>/`). Every later test compares against these
   numbers — if `--check` drifts, a test breaks.

2. **Reverse engineering the formats.** The `.ff` fastfile format (zlib
   blocks, header magic, offset tables), `.iwi` textures (decode → PNG),
   `.gsc` script extraction, and the per-map zone asset tables. All research
   scripts live in `re/` so the pipeline is reproducible, not folklore.

3. **Engine skeleton.** `src/main.c` grew a subcommand CLI
   (`--check`, `info`, `ff`, `mods`, `lan`, `world`, `gfx`, `menu`, `gpu`,
   …), a config file, a small platform layer and honest diagnostics: the
   tool tells you exactly what is missing instead of failing silently.

4. **Graphics layer from scratch.** SDL2 is opened dynamically; the OpenGL
   backend asks for 4.5 core and walks down (4.3 → 3.3 → 2.1) so old
   drivers still run; GLES reuses the same backend with ES-profile shaders;
   the Vulkan backend is written against raw function pointers
   (instance → surface → swapchain → render pass → frame sync). On top of
   that sits a **software-rendered video menu** (single RGBA texture,
   embedded 5×7 bitmap font) and `gapi.c`, which probes what the system
   actually has — including every Vulkan ICD JSON installed.

5. **GPU support for every vendor, every driver.** PCI scan counts NVIDIA /
   AMD / Intel cards; NVIDIA gets the deep treatment: kernel module +
   userspace library versions, real Vulkan capability probe
   (`vk_icdGetInstanceProcAddr`), a **user-level ICD fix** (writes
   `~/.local/share/vulkan/icd.d/nvidia_icd.json`, no root, reversible),
   expected-capability table per driver branch, device scoring that
   prefers the **discrete** GPU, and `T4_GPU=nvidia|amd|intel` to force a
   vendor (PRIME/Optimus included).

6. **Game data systems.** Structured fastfile walker (`zonewalk.c`,
   byte-exact vs. the validated Python reference), GfxWorld extraction +
   native OpenGL window (`world.c`), asset tables, LAN discovery
   (`QUERY` → `INFO`), mod folder loading (zombies / campaign /
   multiplayer), GSC parsing.

7. **Packaging per OS.** Linux: `.sh` launcher + **Flatpak**
   (`com.t4port.engine`); macOS: `.app` bundle layout; Windows:
   `dist/windows/t4port.exe` cross-compiled with zig; FreeBSD: native
   `build.sh` plus `cross-build.sh`.

8. **Testing discipline.** An **ASan/UBSan** build is run over the command
   battery; fastfile counts are asserted against ground truth; the menu and
   API detector are smoke-tested; the Windows `.exe` is executed under
   Wine; cross-builds are compiled for Windows (PE32+) and checked against
   real **FreeBSD 14.5 headers**; `tools/make_source_waw.sh` is
   idempotent and excludes caches/toolchains so the deliverable stays lean.

9. **Single deliverable tree.** Everything ships in **`source waw/`**:
   one `src/` plus flat per-OS folders (`linux/`, `macos/`, `windows/`,
   `freebsd/`) with a `build.sh` each, regenerated from the workshop tree
   (`port/`) by `tools/make_source_waw.sh`.

---

## 3. How to run it

### Linux — start the game

```bash
./t4port.sh            # root shortcut — STARTS THE GAME (classic retail client)
./t4port.sh menu       # in-game Options > Graphics (video settings, English)
./t4port.sh play       # retail client (classic, offline) under Wine
./t4port.sh world mp_dome   # native 3D window with the map's own geometry
./t4port.sh --check    # diagnostics only (engine, data, wine)
./t4port.sh info       # engine inventory / tool index
```

No arguments **starts the game** (classic retail client: campaign,
zombies, LAN offline). The port's own video settings are the in-game
Options > Graphics screen (`t4port menu`, in English). Every subcommand is
100% native C — no Wine in the port. Wine exists only in `play`/`--wine`
as an optional bridge to the
classic retail client (`CoDWaW.exe`) in its own prefix (`~/.wine-waw`),
offline — campaign, zombies and LAN. It never auto-selects the Plutonium
online launcher; their online service belongs to their launcher (project
rule).

### Engine tools (native, no Wine)

```bash
./t4port.sh gpu               # cards, driver versions, Vulkan/ICD status
./t4port.sh menu --list       # video APIs detected on this system
./t4port.sh gfx --api vulkan --segundos 5   # render a test window
./t4port.sh world mp_dome --walk-only       # walk a .ff (no window), stats
T4_GPU=nvidia ./t4port.sh gfx --api opengl  # force a vendor
```

### Other platforms

- **Windows**: `windows/` → `build.sh` (or use the prebuilt
  `dist/windows/t4port.exe`); the game itself runs natively as usual.
  Double-clicking the `.exe` **starts the game** (`CoDWaW.exe` from the
  game folder — no Wine needed on Windows).
- **macOS**: `macos/build.sh` produces the `.app` — needs a real Mac
  (Apple dropped NVIDIA after 10.13; the port says so honestly).
- **FreeBSD**: `freebsd/build.sh` on a FreeBSD box, or `freebsd/cross-build.sh`
  from Linux/macOS (zig + extracted FreeBSD sysroot) — the latter produced
  the validated `dist/freebsd/t4port` ELF 14.5 here.

---

## 4. Project rules we kept

- Offline first: no Steam, no launcher, no phone-home.
- Plutonium's online mode is **their** launcher's job — never modified,
  never emulated, `plutonium.exe` never chosen automatically.
- Original Activision data stays local/personal — never redistributed.
- Honest bug handling: ASan batteries, ground-truth counts, and
  documentation that admits what is *not* finished.
- All documentation in English (US); logo at the top of the main README.

---

## 5. Folder map (`source waw/`)

```
src/            all engine code (C11): main, gfx (GL/GLES/Vulkan),
                gapi (API/GPU probe), menu, zonewalk, world, net (LAN), ...
linux/          build.sh for Linux
macos/          build.sh for the .app
windows/        build.sh (zig/mingw cross → t4port.exe)
freebsd/        build.sh (native) + cross-build.sh
packaging/      Flatpak manifest, .desktop, icons, t4port.sh launcher
doc/            logo and images
re/             reverse-engineering tools (fastfile, iwi, tables)
tools/          make_source_waw.sh (source generator) + probes
dist/           prebuilt binaries: windows/t4port.exe (+pdb),
                freebsd/t4port (cross-built ELF 14.5)
source_original/ snapshot of the untouched original sources/scripts
t4port.sh       root shortcut → packaging/linux/t4port.sh
```

---

## 6. Known issues / roadmap (honest status)

- **World renderer: geometry ✅, shading 🟨.** The earlier "no GfxWorld in
  this .ff" bug is **fixed**: the structured walker (`zonewalk.c`) walks the
  whole zone payload byte-exactly (564/564 assets on `mp_dome.ff`, identical
  to the validated Python reference) and `t4port world <map>` renders the
  extracted geometry (321,384 verts / 217,710 tris on `mp_dome`) in a native
  SDL2+OpenGL window — validated here on OpenGL 4.5 (NVIDIA) and clean under
  ASan/UBSan. Still missing for a real scene: materials, textures, xmodel/
  xanim and the GSC VM; the Vulkan world backend falls back to OpenGL with a
  warning today.
- Because the game *logic* (VM, game loop, physics) is still being
  reconstructed, **complete gameplay today runs through the original
  client** (`./t4port.sh play`, Wine on Linux, native on Windows) while the
  native layer grows file by file.
- macOS `.app` must be built and validated on a real Mac (Apple SDK; the
  scripts are ready). FreeBSD binaries: the cross-built ELF 14.5 is
  validated at the binary level (dynamic, `libc.so.7`/`libm.so.5`), the
  runtime still needs a real FreeBSD machine.
- Pending chores from the original data copy: remove crack leftovers
  (`SteamGG` URL, `Read_Me_Instructions.txt`) and merge the extra `zone/`
  DLCs from `pluto_t4_full_game/`.
