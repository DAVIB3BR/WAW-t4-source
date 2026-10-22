# Reverse engineering summary — *Call of Duty: World at War* (engine **IW 3.0**)

> **Target identification** (proven by the data, not by guesswork):
>
> | | |
> |---|---|---|
> | Game | **Call of Duty: World at War** (2008) |
> | Studio | **Treyarch** |
> | Engine | **IW 3.0** (Treyarch's variant of Infinity Ward's 3.0) |
> | Internal codename | "T4" (WaW is T4; 2010's Black Ops is T5) |
> | FastFile | magic `IWffu100`, **revision 387** — identical in **129/129** files |
>
> Direct proof: `pluto_t4_full_game/localization.txt` literally contains
> *"Call of Duty: World at War"* in every Windows UI message.
> The `plutonium.exe` is only the Plutonium **.NET 4.8/WPF** updater
> (`Plutonium.Updater.App`), **not** the engine binary.

Everything below was **proven empirically** (size arithmetic, decoding and
validation against the complete file set), not deduced from third-party
documentation. Uncertainties are explicitly marked.

---

## 1. Inventory of `pluto_t4_full_game/` (8.2 GB)

| Item | Size | What it is |
|---|---|---|
| `plutonium.exe` | 4.8 MB | **Not the game.** **.NET Framework 4.8 (WPF)** launcher/updater: strings `Plutonium.Updater.App`, `C:\GitLab-Runner\...\Plutonium.Updater.App\pdb`, dependencies embedded via **Costura** (MahApps.Metro, MaterialDesign, Sentry, System.Text.Json). Costura = assemblies compressed inside the .exe. |
| `binkw32.dll` | 171 KB | **Bink Video** codec by RAD Game Tools (third-party, normally redistributable). |
| `main/*.iwd` (35) | 5.8 GB | Plain **ZIP**s with the assets: 27,017 entries (17,080 `.wav`, 9,331 `.iwi`, 187 `.cfg`, 359 with no extension…). |
| `zone/english/*.ff` (129) | 2.3 GB | **FastFiles** (`IWffu100`). |
| `main/video/*.bik` | — | **Bink Video 1280×720** (e.g. `nazi_zombie_factory_load.bik` = 15 fps). |
| `localization.txt` | 2 KB | String table (English). |
| `cod.bmp` / `codlogo.bmp` | 650 KB | 640×256 logo (used as the port's icon). |
| `redist/` | 112 MB | **Windows** installers: DirectX, VC++ 2010/2015-2022, .NET 4.7.2. |

**The engine binary (CoDWaW.exe / T4 client) is not in the folder** — it is
downloaded by the launcher into `%LOCALAPPDATA%\Plutonium\bin\t4\`. So there is
nothing to "tear down" on the game-code side here; what can be reversed are the
data formats.

---

## 2. `.ff` — FastFile

```
+0x00  char[8]  magic = "IWffu100"
+0x08  u32      0x00000183 = 387   ← the SAME value in all 129 tested files
+0x0C  ...      a SINGLE zlib (raw deflate) stream that runs until 11 bytes
                of padding (0x00) remain at the end of the file
```

- Validation: `common.ff` = 29,024,448 B compressed → **43,852,190 B**
  decompressed; `ui.ff` = 0.6 MB → 41.5 MB (66× ratio, highly repetitive data).
- The decompressed content starts with 3 `u32`s:

| Offset | `common.ff` | `ui.ff` | Interpretation |
|---|---|---|---|
| +0x00 | `0x029d217a` = 43,837,818 | `0x0278df7a` | ≈ total size − trailer (43,852,190 − 43,837,818 = 14,372) |
| +0x04 | `0x018c5697` = 26,006,167 | | **(open)** likely the data pool size |
| +0x08 | `0x00000a60` = 2,656 | `0x000001e0` = 480 | likely the **asset count** |

- After that, **NUL-terminated strings** can already be read directly (bone
  names `j_spineupper`, animations, sounds `sndnt#jpn_pain`,
  `footstep_right_large`…).
- **Open**: the exact layout of the asset table (pool + string pool) and the
  type list. That is the job of `re/fftool.py` (analysis in progress).

Implemented in C: `src/ff.c` (`t4port ff <file.ff>`).

---

## 3. `.iwd` — asset package

A pure ZIP (signatures `PK\x03\x04`, `PK\x01\x02`, `PK\x05\x06`), no
encryption. The central directory is read directly in `src/iwd.c` (no libzip):

- EOCD searched backwards in the last 65,557 bytes;
- per entry: method (0 stored / 8 deflate), CRC32, sizes and local header
  offset;
- names normalized (`\` → `/`);
- 35 files, **27,017 entries**, all opened successfully in the smoke test.

---

## 4. `.iwi` — textures

### Header (28 bytes) — validated on **100% of the 9,331 textures**

```
+0x00  char[3]  "IWi"
+0x03  u8       version = 6 (in all of them)
+0x04  u8       format (table below)
+0x05  u8       flags (0x00, 0xC0..0xC3, 0x01, 0x02… — 13 distinct values)
+0x06  u16      width
+0x08  u16      height
+0x0A  u16      = 1 (in the samples; exact role open)
+0x0C  u32[4]   offsets/sizes per level ([0] = total file size)
+0x1C  payload  mip chain from largest to smallest, starting at level 0
```

Proofs:
- `loadscreen_mp_makin_day.iwi`: 1024² × 8 B (DXT1) = 524,288 + 28 =
  **524,316 = actual size**;
- `50cal_top_n.iwi`: full BC5 chain = 174,800 + 28 = **174,828 = actual size**;
- survey of all 9,331: **0 divergences** in the size field, **0 bad magics**,
  **0 short payloads**;
- deduced level count: matches a full chain in 8,926/9,331 (e.g. 10 levels for
  512×512, 11 for 1024×1024).

### Formats (identified by size arithmetic in 100% of the cases)

| Code | Format | Size | Files | How it was proven |
|---|---|---|---|---|
| 1 | **BGRA8** | 4 B/px | 48 | 48/48 exact; correct colors (Soviet flag in red) |
| 3 | **A8L8** (`[0]`=alpha, `[1]`=luma) | 2 B/px | 46 | mip chain closes exactly (21,845 px = sum 128²…1×1); correct look |
| 4 | **L8/A8** | 1 B/px | 22 | 22/22 |
| 11 | **A8_DXT1 → BC1** | 8 B/block | 2,667 | 2,667/2,667; perfect images |
| 12 | **A8_DXT5 → BC3** | 16 B/block | 803 | 803/803; diffuse texture `30cal.iwi` decoded correctly |
| 13 | **DXN → BC5** (normal map) | 16 B/block | 5,665 | 5,665/5,665; `_n` suffix; **R mean 129.7 ± 34** (normal-map signature); visible geometry (airplane panels, planks) |
| 2 | **?** | — | 29 | *not identified* (64×64, payload 98,298 = 98,304 − 6 → fits 24 B/px with a 22 B header, contradicting the 28 B) |
| 6,7,8,9 | **?** | — | 51 | *not identified* |

**Coverage: 9,251/9,331 = 99.1%** decodable by the port.

BC5 cross-validation: a smoothness test compared 3 candidate layouts —
`v0,v1` after the 3-bit indices (the BC5 standard) wins with mean R ≈ 129.7
(real normal-map texture); the alternative candidates would give
R = 114/33/0.

Examples decoded by `src/iwi.c`:

| BC3 diffuse (format 12) — compass map, 512×512 | BC5 normal map (format 13), 256×256 |
|---|---|
| <img src="../doc/images/compass_map_mp_makin_day.png" width="260" alt="BC3 compass map"> | <img src="../doc/images/blood_spatter01_col.png" width="180" alt="BC5 normal map"> |

| BC1 load screen (format 11), 1024×1024 | BC3 cursor (format 12), 64×64 |
|---|---|
| <img src="../doc/images/loadscreen_mp_makin_day.jpg" width="330" alt="BC1 load screen"> | <img src="../doc/images/3_cursor3.png" width="64" alt="BC3 cursor"> |

<sub><i>Call of Duty: World at War</i> textures (© Activision/Treyarch) —
local documentation use only.</sub>

Decoders: `src/iwi.c` (BC1/BC3/BC5 + BGRA8/A8L8/L8 → RGBA8) and
`re/iwitool.py` (analysis in progress).

---

## 5. `.bik` — video

Bink Video 1280×720, 15 fps, with a 48 kHz audio track. This system's `ffmpeg`
already ships `binkvideo` + `binkaudio_dct` + `binkaudio_rdft`, so the port uses
ffmpeg as the video backend instead of reimplementing the codec
(`t4port video`).

Validated conversion: `Treyarch.bik` (26.08 s) → MP4 (MPEG-4 Part 2 + AAC).

---

## 6. `localization.txt`

ASCII key/value pairs with CRLF — UI messages (`WIN_LOW_MEMORY_BODY`…),
separated by blank lines, first line `english`.

---

## 7. Uncertainties / next steps

1. Fastfile asset table (pool + string pool + types) — **under analysis**;
2. IWI formats 2 and 6–9 (80 files, 0.86%);
3. exact role of the IWI header `u32[4]` and of the fastfile `u32 = 387`;
4. bit layout of the `.wav` audio inside the `.iwd`s (they look like standard
   PCM — to be validated);
5. the real T4 client (not present) would be needed to check runtime behavior
   (e.g. how the game interprets the asset table).
