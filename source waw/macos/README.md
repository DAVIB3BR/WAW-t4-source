# T4Port — macOS

## Build (runs on a Mac)

```sh
./build.sh                    # T4Port.app into ../../dist/
./build.sh --with-data /path/pluto_t4_full_game
./build.sh --link-data /path/pluto_t4_full_game   # symlink, instant
```

The script delegates to `../../packaging/macos/build_macos.sh`, which produces
`dist/T4Port.app` (Info.plist, ad-hoc signature, SDL2 in
`Contents/Frameworks` in dlopen mode, data in `Contents/Resources/game` — the
binary locates it by itself when opened from the Finder).

## Requirements

- Xcode Command Line Tools (`xcode-select --install`)
- `cmake` (`brew install cmake ninja`)
- zlib (ships with the SDK); SDL2 optional (`brew install sdl2` — without it
  the app uses dlopen and bundles the framework into the package)

## OS-specific code

`src/platform.c` uses `_NSGetExecutablePath` — already implemented. The rest of
the core is pure POSIX (same as FreeBSD/Linux).

## Status

⚠️ **Not buildable on this Linux machine** — requires a real Mac. The script and
the code are ready; the first build there is what will surface any adjustments.

<p align="center">
  <img src="../../doc/images/loadscreen_mp_bgate.jpg" width="420"
       alt="Call of Duty: World at War — Battle of the Bulge load screen">
  <br>
  <sub><i>Call of Duty: World at War</i> — “Battle of the Bulge” load screen
  (© Activision/Treyarch), decoded with this port's IWI decoder;
  local documentation use only.</sub>
</p>
