# Call of Duty: World at War (T4 / Treyarch) `.ff` fastfile format

Reverse-engineered container + asset-stream layout for the PC files shipped in
`pluto_t4_full_game/zone/english/*.ff` (129 files, all `IWffu100` version **387**).

Everything below is either read straight out of the files or taken from the
load rules that OpenAssetTools reimplements for T4.  The Python 3.6 reference
implementation is [`fftool.py`](fftool.py) in this directory; it walks **every**
asset of every shipped file and stops exactly at the end of the payload
(see [Validation](#validation)).

---

## 1. Sources

* **Files analysed**: `/home/davi/Plutonium/pluto_t4_full_game/zone/english/*.ff`
  (129 files, 150 211 assets total).
* **OpenAssetTools** — <https://github.com/Laupetin/OpenAssetTools> (cloned,
  `src/` tree used as ground truth for the load rules):
  * `src/ZoneCode/Game/T4/XAssets/*.txt` — per-asset "ZoneCode" load commands
    (`set count`, `set string`, `set condition … never`, `set block … RUNTIME`,
    `reorder:`, …).  These files are generated from the game's own load code and
    are the authoritative description of which pointee a pointer member has and
    how many elements it holds.
  * `src/Common/Game/T4/T4_Assets.h` — struct/union/typedef definitions
    (sizes, member order, pointer/array declarations).
  * `src/ZoneCodeGeneratorLib/` — turns the ZoneCode commands into the reader
    and writer templates (`ZoneLoadTemplate.cpp`, `ZoneWriteTemplate.cpp`),
    which document the stream order.
  * `src/ZoneWriting/Zone/Stream/ZoneOutputStream.cpp` — pointer encoding
    (`FOLLOWING`/`INSERT`), the reusable/alias dedup, block bookkeeping.
  * `src/ZoneLoading/Zone/Stream/ZoneInputStream.cpp` — the matching reader.
* No published documentation of the T4 container was found; the container
  (header + zlib + index) was derived from the files themselves.

---

## 2. Container

```
offset  size  field
      0     8  magic  "IWffu100"          (ASCII, no NUL)
      8     4  u32 LE version             (387 in every file analysed)
     12     *  zlib stream (RFC 1950, one stream for the whole payload)
```

`zlib.decompress(file[12:])` yields the payload.  Two sizes are stored in the
payload header, not in the container header:

```
payload+0x00  u32  size          declared payload size == actual length - 36
payload+0x04  u32  externalSize  total size in the *unlinked* (external) form
payload+0x08  u32  xblockSize[7] bookkeeping counters, see section 4
payload+0x24         XAssetList, see section 3
```

`size == len(payload) - 36` holds for all 129 files (36 = 0x24, the size of the
fixed header in front of the XAssetList).  `externalSize` is unrelated to the
on-disk size (e.g. `ber1.ff`: compressed 73 749 696, payload 151 699 979,
`externalSize` 221 121 912).

---

## 3. XAssetList (payload + 0x24)

```c
struct ScriptStringList { uint32 count; const char** strings; };   //  8 bytes
struct XAssetList       { ScriptStringList stringList;             // +0x00
                          uint32 assetCount;                       // +0x08
                          XAsset* assets; };                       // +0x0C  (16 bytes total)

struct XAsset { AssetType type;            // +0x00
                XAssetUnion data; };       // +0x04  (8 bytes, a pointer union)
```

(the layout is fixed by `ContentLoader::Load()`, which fills offsets 0/4/8/12 of
the list before pushing the insert block).

| offset | size | field |
|---|---|---|
| +0x00 | 4 | `scriptStringCount` |
| +0x04 | 4 | `scriptStrings` pointer (always `0xFFFFFFFF` or `0`) |
| +0x08 | 4 | `assetCount` |
| +0x0C | 4 | `assets` pointer (always `0xFFFFFFFF`) |

then, immediately:

* `scriptStringCount` × u32 pointer — `0` = null, `0xFFFFFFFF` = the string
  itself follows inline as a NUL-terminated byte string.  These strings are the
  `scriptstring` values referenced by assets (e.g. `DynEntityDef::targetname`,
  `pathnode_constant_t::targetname`).  `fftool.py info` prints their count;
  `ber1.ff` has 1141, `code_post_gfx_mp.ff` 154, `*_load.ff` 0.
* `assetCount` × 8 bytes, an array of `XAsset` couples:

  ```
  u32 type       asset type id (section 8)
  u32 data       pointer to the asset instance; 0xFFFFFFFF = "inline/following"
  ```

  In every shipped file the `data` pointer is `0xFFFFFFFF`, i.e. all assets are
  stored **inline, in list order**, as one continuous content stream that starts
  right after the couple array (`content_pos` in `fftool.py`).

`fftool.py` therefore does not need the couple offsets at all: it walks the
content stream from `content_pos` and consumes assets one after another.  That
the walk ends exactly at the end of the payload for every file is the strongest
evidence that the per-asset consumption rules below are complete.

Note that `ContentLoader::Load()` pushes `XFILE_BLOCK_VIRTUAL` before loading
the script strings and the asset array, and in the shipped files almost every
alias pointer decodes to block 4 (`VIRTUAL`): sampling every pointer member of
every asset, 184 733 of 188 092 aliases in `ber1.ff`, 134 802 of 136 309 in
`mak.ff` and 7 202 of 9 481 in `common.ff` use block 4 (the rest are blocks 2
`LARGE_RUNTIME`, 6 `PHYSICAL` and rarely 1 `RUNTIME`).  The content stream thus
lives in the `VIRTUAL` arena.

---

## 4. XBlocks and the "runtime block" rule

The seven `xblockSize[i]` counters are in `XFileBlock` order (from
`T4_Assets.h` + `ZoneLoaderFactoryT4::SetupBlock`):

```
0 TEMP              1 RUNTIME            2 LARGE_RUNTIME      3 PHYSICAL_RUNTIME
4 VIRTUAL           5 LARGE              6 PHYSICAL
```

They are what the game's loader uses to `Alloc()` the seven in-memory XFile
arenas (`StepAllocXBlocks` reads exactly `7 × u32` right after `size` /
`externalSize`), so they are **not** a partition of the payload: they do not sum
to the payload length and no contiguous region boundary could be derived from
them (`ber1.ff`: sum 135 611 537 vs. payload 151 699 979;
`code_post_gfx_mp.ff`: sum 1,390,311 vs. 1,443,835).  Only `TEMP`, `VIRTUAL`
and `LARGE` are ever non-zero in the shipped files (`ber1.ff` also has
`RUNTIME = 0x20f6d0`).  The parser never needs them.
See [Open questions](#open-questions).

What *does* matter is the rule that comes with the block system:

> A pointer member marked `set block <member> XFILE_BLOCK_RUNTIME;` in ZoneCode
> has its pointee in an XFile *runtime* block.  The writer emits **no bytes**
> for it (`ZoneOutputStream::WriteDataInBlock` returns `nullptr`) and the loader
> zeroes it without reading (`ZoneInputStream::LoadDataFromBlock`), so such a
> pointee consumes **0 stream bytes even when the stored pointer is
> `0xFFFFFFFF`**.

Alignment (`set allocalign <member> <n>;`) likewise only advances the block
position counters, never the stream.

Only these T4 members are runtime-block members (see `RUNTIME` in `fftool.py`):

| struct | members |
|---|---|
| `GfxWorld` | `reflectionProbeTextures`, `lightmapPrimaryTextures`, `lightmapSecondaryTextures`, `cellCasterBits`, `sceneDynModel`, `sceneDynBrush`, `primaryLightEntityShadowVis`, `primaryLightDynEntShadowVis`, `nonSunPrimaryLightForModelDynEnt` |
| `GfxWorldDpvsPlanes` | `sceneEntCellBits` |
| `GfxWorldDpvsStatic` | `smodelVisData`, `surfaceVisData`, `lodData`, `surfaceMaterials`, `surfaceCastsSunShadow` |
| `GfxWorldDpvsDynamic` | `dynEntCellBits`, `dynEntVisData` |
| `PathData` | `basenodes` |
| `clipMap_t` | `dynEntPoseList[0..1]`, `dynEntClientList[0..1]`, `dynEntServerList[0..1]`, `dynEntCollList[0..3]` |

---

## 5. Pointers

Every pointer member is a 4-byte little-endian value inside its owning struct
blob.  The value is either an inline-ness marker or a *zone pointer* into a
block:

| value | meaning | stream effect |
|---|---|---|
| `0xFFFFFFFF` | **FOLLOWING** — the pointee is stored inline, at the current stream cursor | consume the pointee |
| `0xFFFFFFFE` | **INSERT** — same as FOLLOWING (the writer switches to the insert block, `XFILE_BLOCK_VIRTUAL`, first) | consume the pointee |
| anything else | **alias** — a zone pointer into an XFile block, or `NULL` | consume **nothing** |

A zone pointer packs the block index into the **top 3 bits**
(`ZoneConstants::OFFSET_BLOCK_BIT_COUNT = 3`) and the block offset into the low
29 bits, plus one:

```
ptr = (blockIndex << 29) | (blockOffset + 1)
```

so `0x81e236f9`, `0x8431a105` … all decode to block 4 (`VIRTUAL`) at
offsets 0x1e236f8 / 0x431a104.  `0x00000000` is a null pointer.
`is_following(v) == (v == 0xFFFFFFFF or v == 0xFFFFFFFE)` — the two markers are
*exact* values, not masks.

Consequences that the parser must implement:

* An asset instance pointer, a material pointer inside a model, a plane pointer
  inside a brush side, … are aliases and consume nothing.  This is why many
  assets in a fastfile are "hollow": e.g. every `sound` asset in `ber1.ff` is a
  12-byte blob with no pointee, because the alias list, the loaded-sound data
  etc. are shared/aliased.
* `pathnode_tree_t::u.child[2]` in `ber1.ff` holds `0x8431a105`-style addresses
  of sibling tree nodes that are stored in the same blob array — the 1351 tree
  nodes are written as one array and their children point into it.
* Pointers can be *arrays* (`dynEntDefList[2]`, `pathnode_tree_info_t::child[2]`,
  `StringTable::values`) or *embedded arrays of pointers*
  (`MenuList::menus`, `multiDef_s::dvarList[32]`).

---

## 6. Struct serialization

An asset (and every pointee struct) is stored as a **raw memory-layout blob**:

* the size is `sizeof(struct)` with MSVC layout — member offsets, alignment
  padding *between* members and the tail padding up to the struct alignment
  (e.g. `cbrush_t` is 80 bytes with `align = 16`, `GfxImage` 36 bytes with
  `align = 4`);
* pointer members occupy 4 bytes in the blob; their pointees are **not** part of
  the blob;
* a `set condition … never` member still occupies its bytes in the blob — the
  condition only suppresses *pointee processing*;
* embedded arrays declared as `set arraysize <member> <expr>` (e.g.
  `MaterialTechnique::passArray[passCount]`) enlarge the blob, so the struct's
  serialized length differs from `sizeof(struct)`;
* unions are stored as their largest member; which member is "active" is
  recovered from sibling fields (section 7).

No inter-blob padding was observed: every struct that can appear as a pointee
array element has a size that is a multiple of 4 (the only sizes that are not
multiples of 4 belong to 1/2-byte leaf types such as `XSurfaceTri`), so a
4/8/16-byte alignment before the next blob is always a no-op.  `fftool.py`
consumes blobs back-to-back and still lands exactly on the end of every file.

**Order of pointees** (from `ZoneWriteTemplate.cpp`): for a struct, all of its
pointer members' pointees are consumed in *member declaration order*, and for an
array of structs all element blobs are stored first, followed by the pointees of
every element in element order:

```
[ elem0 blob ][ elem1 blob ] … [ elemN blob ] [ elem0 pointees ] [ elem1 pointees ] …
```

---

## 7. Recovering counts, strings and the active union member

ZoneCode commands map onto the format as follows:

| ZoneCode | meaning in the stream |
|---|---|
| *(nothing)* | pointer member = single pointee (one struct / one string) |
| `set count <member> <expr>;` | pointee is an array of `expr` elements, `expr` evaluated in the owning struct's field namespace (e.g. `set count planes planeCount;`) |
| `set string <member>;` | pointee is a NUL-terminated byte string |
| `set scriptstring <member>;` | the field holds an index into the XAssetList script-string table — **no stream bytes** |
| `set block <member> XFILE_BLOCK_RUNTIME;` | pointee consumes 0 bytes (section 4) |
| `set condition <member> <cond>;` | pointee is only present when the condition holds; `never` means "never present" |
| `set reusable <member>;` | only affects the writer's alias dedup (a pointer already written for that type is patched to point at the earlier copy); on the read side it is just a normal pointer — **no size change** |
| `set allocalign <member> <n>;` | counter alignment only, no stream bytes |
| `reorder: …;` | pointee order rewrite (below) |

`reorder:` without a leading `…` moves the listed members to the **front** of
the pointee order; with a leading `…` the first listed member is an **anchor**
that stays at its declaration position and the remaining listed members are
loaded immediately after it.  Examples: `GfxWorld` (`materialMemory` anchor,
then `vd`, `vld`), `clipMap_t` (`leafs` anchor, then `leafbrushes`,
`leafbrushNodes`, `leafsurfaces`), `WeaponDef` (the five accuracy-graph members
after `aiVsAiAccuracyGraphName`), `MaterialTechnique` (`passArray` before
`name`), `XSurface` (`zoneHandle`, `vertInfo`, `verts0`, `vertList`,
`triIndices`).

Unions whose active member depends on a sibling field are resolved with
`set condition`:

| union | selector |
|---|---|
| `cLeafBrushNode_s::data` | `leafBrushCount > 0` → `data.leaf` (brushes: `u16[leafBrushCount]`), else the children struct |
| `pathnode_tree_t::u` | `axis >= 0` → `u.child[2]` (two pointers to other tree nodes), else `u.s` = `{ s32 nodeCount; u16* nodes }` |
| `pathnode_t::transient` | `never` — its three pointer members (`pNextOpen`, `pPrevOpen`, `pParent`) never have a pointee |
| `expressionEntry::data` | `type == EET_OPERAND` → `Operand` |
| `Operand::internals` | `dataType == VAL_STRING` → `internals.stringVal` |
| `itemDef_s::typeData` | `itemDefData_t::data` is `never`; the active member (`editField` / `listBox` / `multi` / `enumDvarName`) depends on `itemDef_s::type` |
| `MaterialShaderArgument::u` | `type == MTL_ARG_*` selects `literalConst` / `codeConst` / `codeSampler` / `nameHash` |
| `GfxPortalWritable` | `writable` is `never` → `hullPoints` / `queuedParent` never have a pointee |
| `XAnimPartTrans::u` / `XAnimDeltaPartQuat::u` | `size > 0` → frames sub-struct, else one inline frame |
| `XAnimParts::indices` | `numframes < 256` → `u8[indexCount]`, else `u16[indexCount]` |

Note that `axis` in `pathnode_tree_t` is a **signed** int stored in an unsigned
field read — it must be sign-extended before testing `>= 0`
(676 of the 1351 tree nodes of `ber1.ff` are leaf nodes with `axis == -1`).

---

## 8. Asset types

`u32` type id used in the XAssetList couples (`t4ff.ASSET_TYPES` /
`fftool.TYPE_STRUCT`).  Counts are totals over all 129 shipped files.

| id | name | root struct | instances |
|---|---|---|---|
| 0 | xmodelpieces | `XModelPieces` | 0 |
| 1 | physpreset | `PhysPreset` | 80 |
| 2 | physconstraints | `PhysConstraints` | 2 |
| 3 | destructibledef | `DestructibleDef` | 131 |
| 4 | xanim | `XAnimParts` | 10 353 |
| 5 | xmodel | `XModel` | 11 152 |
| 6 | material | `Material` | 1 379 |
| 7 | techset | `MaterialTechniqueSet` | 20 225 |
| 8 | image | `GfxImage` | 13 |
| 9 | sound | `snd_alias_list_t` | 79 889 |
| 10 | loadedsound | `LoadedSound` | 0 |
| 11 | clipmap | `clipMap_t` | 22 |
| 12 | clipmap_pvs | `clipMap_t` | 23 |
| 13 | comworld | `ComWorld` | 45 |
| 14 | gameworld_sp | `GameWorldSp` | 22 |
| 15 | gameworld_mp | `GameWorldMp` | 23 |
| 16 | mapents | `MapEnts` | 0 |
| 17 | gfxworld | `GfxWorld` | 45 |
| 18 | lightdef | `GfxLightDef` | 60 |
| 19 | uimap | `UiMap` | 0 |
| 20 | font | `Font_s` | 18 |
| 21 | menulist | `MenuList` | 47 |
| 22 | menu | `menuDef_t` | 0 |
| 23 | localize | `LocalizeEntry` | 16 813 |
| 24 | weapon | `WeaponDef` | 719 |
| 25 | snddriverglobals | `SndDriverGlobals` | 2 |
| 26 | fx | `FxEffectDef` | 6 008 |
| 27 | impactfx | `FxImpactTable` | 11 |
| 28–31 | aitype / mptype / character / xmodelalias | — | 0 |
| 32 | rawfile | `RawFile` | 3 048 |
| 33 | stringtable | `StringTable` | 81 |
| 34 | packindex | `PackIndex` | 0 |

Types 11 and 12 share `clipMap_t`; 14 and 15 share `PathData`.

### 8.1 Root structs worth spelling out

```
RawFile            12 B  { char* name; int len; char* buffer }
                    pointees: name (string), buffer (len + 1 bytes, NUL padded)
StringTable        16 B  { char* name; int columnCount; int rowCount; char** values }
                    pointees: name, then values = char*[columnCount*rowCount]
LocalizeEntry       8 B  { char* value; char* name }
snd_alias_list_t   12 B  { char* aliasName; snd_alias_t* head; int count }
                    pointees: aliasName, head = snd_alias_t[count]
LoadedSound        12 B  { char* name; snd_asset sound }
MapEnts            12 B  { char* name; char* entityString; int numEntityChars }
                    pointees: name, entityString (numEntityChars bytes of text)
GameWorldSp/Mp     44 B  { char* name; PathData path }
PathData           40 B  { u32 nodeCount; pathnode_t* nodes (runtime: basenodes;
                           u16* chainNodeForNode [nodeCount]; u16* nodeForChainNode;
                           int visBytes; char* pathVis; int nodeTreeCount;
                           pathnode_tree_t* nodeTree }
clipMap_t         332 B  30 pointer members, every count named (see clipMap_t.txt)
GfxWorld          800 B  29 pointer members, 9 of them runtime-block
WeaponDef        2476 B  57 pointer members
SndDriverGlobals 23556 B single blob, one string pointee
```

`clipMap_t` counts (all from `set count`): `planeCount`, `numStaticModels`,
`numMaterials`, `numBrushSides`, `numBrushEdges` (`cbrushedge_t` = `char` → raw
bytes), `numNodes`, `numLeafs`, `leafbrushNodesCount`, `numLeafBrushes`
(`LeafBrush` = `unsigned short` → u16 array), `numLeafSurfaces` (u32 array),
`vertCount`/`numBrushVerts` (`vec3_t` is a union → 12·n raw bytes), `nuinds`
(u16), `3*triCount` (u16), `((3*triCount+31)/32)*4`, `borderCount`,
`partitionCount`, `aabbTreeCount`, `numSubModels`, `numBrushes`,
`numClusters*clusterBytes`, `dynEntCount[0..1]` (two pointer members), and
`num_constraints`.  Inside a `cbrush_t`, `sides` / `baseAdjacentSide` / `verts`
are declared `set reusable` **without** a count, so each is a single pointee —
the bulk data lives in the `clipMap_t` arrays and shows up here as aliases.

---

## 9. Worked example — `maps/ber1.gsc` inside `ber1.ff`

```
container header          12 B          "IWffu100" + version 387
zlib                      …             -> 151 699 979 payload bytes
payload header            0x00..0x23    size=151699943 externalSize=221121912
                                        7 xblock counters
XAssetList                0x24          1141 script strings, 4330 assets
content stream            55456         asset 0 = localize, …
asset 1114  gfxworld      39605884..95378057
asset 1115  gameworld_sp  95378057..96914203
asset 1116  clipmap       96914203..104870964   (entityString text at 104208462)
asset 3970  rawfile       139922828..139939600  "maps/ber1.gsc"  (16 745 bytes)
…
payload end               151699979
```

`fftool.py extract … -o DIR` writes the 56 `.gsc` files of `ber1.ff`
(`maps/ber1.gsc`, `maps/ber1_anim.gsc`, `maps/createfx/ber1_fx.gsc`, …).

---

## 10. Validation

`allwalk.py` (scratch, `/tmp/opencode/`) walks every asset of every shipped file
and checks that the cursor ends exactly at the end of the payload.  Result:
**129 of 129 files exact, 150 211 assets, no error, no residual bytes**
(full listing in [`walk_validation.txt`](walk_validation.txt)):

| file | assets | result |
|---|---|---|
| `ber1.ff` | 4330 | exact (end 151 699 979) |
| `ber1_load.ff` | 5 | exact |
| `ber2.ff` | 4777 | exact |
| `ber2_load.ff` | 5 | exact |
| `ber3.ff` | 4216 | exact |
| `ber3_load.ff` | 5 | exact |
| `ber3b.ff` | 4344 | exact |
| `ber3b_load.ff` | 5 | exact |
| `code_post_gfx.ff` | 3037 | exact |
| `code_post_gfx_mp.ff` | 335 | exact |
| `common.ff` | 7011 | exact (end 43 852 190) |
| `common_ignore.ff` | 1 | exact |
| `common_mp.ff` | 4238 | exact |
| `mak.ff` | 5368 | exact |
| `pel2.ff` | 5394 | exact |
| `ui.ff` | 2402 | exact |
| `ui_mp.ff` | 91 | exact |
| *(all remaining `localized_*`, `mp_*`, `pel*`, `pet*`, `sniper*`, …)* | | exact |

Additional cross-checks:

* `fftool.py info` on `code_post_gfx_mp.ff` ends at 1 443 871 = payload length,
  and lists 28 rawfiles whose names are the expected `.cfg`/`.gsc` names.
* `fftool.py extract ber1.ff` produces 56 `.gsc` files with valid C-like
  script text (`#include maps\_utility;`, `main() {`, …).
* Field-level spot checks against hand-derived ground truth: `ber1.ff`
  `GfxWorld` planes = 32 467 exactly unit-length `cplane_s`, cells = 27 × 56 B,
  probes = 78 × 16 B; `clipMap_t::mapEnts::entityString` = the `"worldspawn"`
  entity text of the map.
* Pointer census over `common.ff` / `mak.ff` / `ber1.ff`: of 333 882 alias
  pointers, 98 % decode to block 4 (`VIRTUAL`) — consistent with a single
  content stream plus shared/aliased sub-data (section 5).

---

## 11. Open questions

1. **`xblockSize[7]` semantics.**  The game's loader uses them to `Alloc()` the
   seven in-memory XFile arenas (`StepAllocXBlocks`), but they are not offsets,
   not lengths of contiguous payload regions and do not sum to the payload size.
   Whatever accounting the writer used (the shipped files are *linked* zones) is
   not reproducible from the payload alone.  Nothing in the reader needs them
   (the parser never uses them), so this is unresolved but harmless.
2. **`externalSize`** is much larger than the payload for map files and much
   smaller for `_load` files; it is presumably the size of the unlinked zone
   including externally streamed assets.  Not needed to parse the file.
3. **Asset types never present in the shipped files** (`xmodelpieces`,
   `loadedsound`, `mapents`, `uimap`, `menu`, `aitype`, `mptype`, `character`,
   `xmodelalias`, `packindex`) have no entry in `structs.json`, so `fftool.py`
   reports them as unsupported instead of guessing.  OAT's
   `ContentLoaderT4.cpp` likewise `SKIP_ASSET(...)`s `XMODELPIECES`, `UI_MAP`,
   `AITYPE`, `MPTYPE`, `CHARACTER` and `XMODELALIAS`, so no reference
   implementation of those exists either.
4. **Explicit alignment padding** was never observed between blobs; if a file
   with a differently packed struct (e.g. a 16-byte-aligned element whose size
   is not a multiple of 16) appeared, the walker would have to insert the
   padding that `ZoneOutputStream::Align` performs.
5. **`cbrush_t` `set reusable` members** are read as single pointees, which is
   consistent with the exact end-to-end walks but has not been verified against
   a dump of an actual brush (the data is normally an alias into the
   `clipMap_t` arrays).

---

## 12. Tooling

`fftool.py` — single-file, Python 3.6, standard library only.

```
python3.6 fftool.py info    <file.ff>
python3.6 fftool.py extract <file.ff> [-o DIR] [--ext .gsc .cfg .txt] [--all]
```

* `info` prints the container header, the asset census and one row per asset
  with its name and `[start..end)` range inside the payload.
* `extract` walks with the same engine, additionally records every `bytes`
  pointee, and writes the `RawFile` payloads (default filter: `.gsc`, `.cfg`,
  `.txt`; `--all` disables the filter).  Directory components inside a rawfile
  name are preserved.

Internals: `T` (embedded `structs.json`: 244 structs, sizes/offsets/pointer
flags) + `ANN` (annotation table: per-struct member → count/string/bytes/union
rule) + `RUNTIME`/`ORDER`/`ORDER_V2` + the `Walker`.
