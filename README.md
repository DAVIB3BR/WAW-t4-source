<p align="center">
  <img src="port_test/doc/images/cod_waw_logo.png" width="420"
       alt="Call of Duty: World at War — logo/icon">
</p>

<h1 align="center">t4port — native port of <i>Call of Duty: World at War</i></h1>

<p align="center">
  <b>Reverse engineering of the game's formats</b> + a <b>100% native C core</b>
  (engine IW 3.0 / Treyarch "T4") + packaging for Linux (<code>.sh</code>,
  Flatpak), Windows (<code>.exe</code>), macOS (<code>.app</code>) and FreeBSD.
</p>

<p align="center">
  <i>Maps shown: <b>mp_dome</b> · <b>zombie_asylum</b> · <b>mp_makin_day</b> ·
  <b>mp_airfield</b> — loadscreens straight from the game data.</i>
</p>

<p align="center">
  <img src="port_test/doc/images/loadscreen_mp_dome.jpg" width="24%" alt="mp_dome loadscreen">
  <img src="port_test/doc/images/loadscreen_zombie_asylum.jpg" width="24%" alt="zombie_asylum loadscreen">
  <img src="port_test/doc/images/loadscreen_mp_makin_day.jpg" width="24%" alt="mp_makin_day loadscreen">
  <img src="port_test/doc/images/loadscreen_mp_airfield.jpg" width="24%" alt="mp_airfield loadscreen">
</p>

> **Image credits**: game logo and loadscreens are from *Call of Duty: World at
> War* (**© Activision / Treyarch**) — extracted from a local copy of the game
> and shown here for identification/documentation of this research project
> only. All rights belong to their owners.

---

## 📁 What is in this repository

| Folder | What it is |
|---|---|
| **[`waw port/`](waw%20port/)** | Ready-to-build **deliverable**: full source, prebuilt binaries (Windows `.exe`, FreeBSD), complete docs ([README](waw%20port/README.md)), RE tools |
| **[`port_test/`](port_test/)** | The **development tree**: `src/` (the native engine core), `packaging/` (Flatpak manifest), per-OS build scripts, docs and tools |

Full documentation: **[`waw port/README.md`](waw%20port/README.md)** · project map: [`waw port/PROJECT.md`](waw%20port/PROJECT.md)

---

## 🎮 The three game modes — every asset decoded, script-by-script

The **whole data side** of the game is decoded and readable with this port's
tools: **1,391/1,391 GSC scripts parse** (VM phase 1), all fastfiles walk
**byte-exact**, and 99.1% of the 9,331 textures decode natively (BC1/BC3/BC5...).

### 🧟 Nazi Zombies — the co-op survival mode

Escalating rounds, points, unlockable areas, **Perks-a-Cola** machines and the
**Pack-a-Punch** — all of it lives in the data, fully extracted:

| Retail map | Internal name | Content |
|---|---|---|
| Nacht der Untoten | `nazi_zombie_prototype` | `.ff` + `_load.ff` |
| Verrückt | `nazi_zombie_asylum` | `.ff` + `_load.ff` + `_patch.ff` |
| Shi No Numa | `nazi_zombie_sumpf` | `.ff` + `_load.ff` + `_patch.ff` |
| Der Riese | `nazi_zombie_factory` | `.ff` + `_load.ff` + `_patch.ff` |

**14 zombie fastfiles**, **42 extracted scripts** driving the zombiemode logic,
**100 files** with "zombie" in the name (weapons, perks, spawners, air-support),
intro cinematics decodable with `t4port video` (`.bik`). Every zombie script
parses — including the injected-call style (`self waittill("death")`,
`level thread f()`).

<p align="center">
  <img src="waw%20port/doc/images/ls_zombie1.jpg" width="30%" alt="Nazi Zombies key art">
  <img src="waw%20port/doc/images/ls_zombie_sumpf.jpg" width="30%" alt="Shi No Numa">
  <img src="waw%20port/doc/images/ls_zombie_factory.jpg" width="30%" alt="Der Riese">
</p>

### 🎖️ Campaign — two theaters, 14 missions

The **US Marine Corps in the Pacific** (Peleliu, Makin, Okinawa) and the **Red
Army's push into Berlin** (Seelow Heights, Reichstag):

| Theater | Missions (internal names) |
|---|---|
| Pacific — USMC | `pel1`, `pel1a`, `pel1b`, `pel2` (Peleliu) · `mak` (Makin Atoll) · `oki2`, `oki3` (Okinawa) · `pby_fly` (PBY Catalina) · `sniper` |
| Eastern Front — Red Army | `see1`, `see2` (Seelow Heights) · `ber1`, `ber2`, `ber3`, `ber3b` (Berlin / Reichstag) |

**30 campaign fastfiles**, **113 extracted scripts** (all editable via the
mods overlay), cinematics (`intro_pac.ff`, `outro.ff`, `credits.ff`) and
`.bik` videos decoded natively.

### 🔫 Multiplayer — 23 maps, 9 gamemodes, own LAN protocol

- **9 gamemodes** as readable scripts: `tdm`, `dm`, `dom`, `sd` (Search &
  Destroy), `ctf`, `twar` (War), `koth` (Headquarters), `sab` (Sabotage) and
  `sur` (Survival — exclusive to WaW).
- **23 maps** (46 fastfiles) · **102 multiplayer scripts** + **222 client-side
  `.csc`** files.
- Killstreak logic plain and editable: `_ac130.gsc`, `_helicopter.gsc`,
  `_dogs.gsc` (attack dogs), `_laststand.gsc`, `_minefields.gsc`…
- **Co-op Campaign and Arcade Mode** are in the data too.
- **Cut content**: `iw_04.iwd` still carries loadscreens for **6 maps that
  never shipped** — `atoll`, `beachhead`, `carrier`, `cavern`, `temp`,
  `wetlands`.
- **LAN layer**: the port implements its own `T4LAN1` discovery/host protocol
  over UDP **28960** (the classic CoD server port).

> **Honest status**: the *data/script/rendering* side is done and proven; the
> **GSC VM phase 2** (AST + interpreter) that *executes* the scripts is the
> roadmap's big item. Today you **play the game** through the classic retail
> client — the launcher starts it for you on every platform (see below).

---

## 🚀 Quick start

```bash
# Linux — the launcher STARTS THE GAME (classic retail client)
./t4port.sh

# in-game video options (native window, English, detects your APIs)
./t4port.sh menu            # Options > Graphics: VIDEO API / RESOLUTION / FULLSCREEN / VSYNC

# native 3D window — the map's own geometry, zero emulation
./t4port.sh world mp_dome

# explore the data
./t4port.sh ff zone/english/mp_dome.ff      # walk a fastfile
./t4port.sh mods unpack mymod               # extract every GSC script
./t4port.sh lan host --name "srv" --map nazi_zombie_factory --mode zombies
```

---

## 🖥️ Platforms

### 🐧 Linux — the reference platform

- **Launcher** (`t4port.sh` / `.desktop`): no arguments **starts the game** —
  the classic retail client under a Wine bridge, from its own prefix, offline
  (campaign, zombies, LAN).
- **Flatpak** (`com.t4port.engine`): ships the native engine + 8.2 GB of game
  data. No wine inside the sandbox — the icon **delegates the game start to the
  host** via `flatpak-spawn --host`.
- Native tools need **nothing proprietary**: `menu`, `world`, `gfx`, `ff`,
  `zip`, `iwi`, `mods`, `gsc`, `lan` are pure C11 + SDL2 + zlib.
- Build: `platforms/linux/build.sh` (or CMake + Ninja).

### 🪟 Windows — fully native, no Wine

- **Double-click `t4port.exe` → the game starts** (`CoDWaW.exe` found in the
  game folder and launched natively — the retail client is a Windows PE, no
  emulation at all).
- Same engine subcommands (`menu`, `world`, `ff`, `mods`...), Winsock for the
  LAN layer.
- Cross-build from Linux/macOS: `platforms/windows/build.sh` (zig mingw or
  mingw-w64) → `dist/windows/t4port.exe` (PE32+), **zero warnings**.
- Still pending: testing the SDL2 `view` window on physical Windows hardware.

### 🍎 macOS — packaging ready

- `platforms/macos/build.sh` produces the `.app` bundle (engine + data in
  `Contents/Resources/game`) — **needs a real Mac** to build/run.
- Honest hardware note: **NVIDIA GPUs are supported only up to macOS 10.13
  High Sierra** (with the NVIDIA web driver); from Mojave on there is no
  official driver and the game would run on the system's OpenGL.
- The engine already handles the `.app` layout (`bundle_game_dir` →
  `../Resources/game`) and the Finder's `-psn_*` argument.

### 😈 FreeBSD — cross-built and ready to test

- The core is pure POSIX; the only OS-specific piece (`sysctl
  KERN_PROC_PATHNAME`) is already in `platform.c`.
- **Cross-built from Linux**: `platforms/freebsd/cross-build.sh` compiles every
  source with `zig cc -target x86_64-freebsd` using the extracted FreeBSD
  sysroot and links against its own `crt1.o`/`libc.so.7`/`libm.so.5` →
  `dist/freebsd/t4port` = `ELF ... for FreeBSD 14.5`, **zero warnings**.
- On a FreeBSD box: `platforms/freebsd/build.sh` (native). Runtime validation
  (`./t4port --check`) still awaits a real FreeBSD machine.

### 🧭 Status table

| Platform | Build | Status |
|---|---|---|
| Linux (`.sh` + Flatpak) | `platforms/linux/build.sh` | ✅ working |
| Windows (`t4port.exe`) | `platforms/windows/build.sh` (zig/mingw) | ✅ cross-built, warning-free |
| FreeBSD | `platforms/freebsd/cross-build.sh` | ✅ cross-built (`ELF ... FreeBSD 14.5`) |
| macOS (`.app`) | `platforms/macos/build.sh` | ⏳ needs a real Mac |

---

## ⚠️ Status: vibe-coded

This project is **vibe-coded** — written in an AI-assisted, exploratory way —
and **it still presents errors**: expect rough edges, gaps and bugs. Treat it
as a learning/demo artifact around the T4 (IW 3.0) engine, **not** as a
finished product.
