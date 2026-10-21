#!/usr/bin/env bash
#
# build_macos.sh - constroi o bundle .app do t4port em macOS.
#
# ESTE SCRIPT RODA NO MAC (nao e cross-compile): ele precisa de Xcode Command
# Line Tools, cmake e (recomendado) SDL2 via Homebrew.
#
# Uso:
#   ./build_macos.sh                          # so o .app (sem os dados do jogo)
#   ./build_macos.sh --with-data <dir>         # copia os 8,2 GB para Contents/Resources/game
#   ./build_macos.sh --link-data <dir>         # cria symlink dos dados (rapido, 0 bytes)
#   ./build_macos.sh --install                 # copia o .app para /Applications
#   ./build_macos.sh --clean                   # rebuild do zero
#
# Requisitos:
#   xcode-select --install        (compilador)
#   brew install cmake sdl2       (ou use o dlopen, se a SDL2 nao existir)
#
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PORT_DIR="$(cd "$SCRIPT_DIR/../.." && pwd)"
BUILD_DIR="$PORT_DIR/build-macos"
DIST_DIR="$PORT_DIR/dist"
APP="$DIST_DIR/T4Port.app"
APP_ID="com.t4port.engine"

DATA_SRC=""
DO_LINK=0
DO_INSTALL=0
CLEAN=0

while [ $# -gt 0 ]; do
    case "$1" in
        --with-data) DATA_SRC="${2:?dir obrigatorio}"; DO_LINK=0; shift ;;
        --link-data) DATA_SRC="${2:?dir obrigatorio}"; DO_LINK=1; shift ;;
        --install)   DO_INSTALL=1 ;;
        --clean)     CLEAN=1 ;;
        *) echo "opcao desconhecida: $1" >&2; exit 2 ;;
    esac
    shift
done

die() { echo "ERRO: $*" >&2; exit 1; }

echo "== verificando o ambiente do macOS =="
[ "$(uname -s)" = "Darwin" ] || die "este script precisa rodar em macOS (uname=$(uname -s))"
command -v cc >/dev/null || die "compilador ausente: rode 'xcode-select --install'"
command -v cmake >/dev/null || die "cmake ausente: brew install cmake"

if pkg-config --exists sdl2 2>/dev/null; then
    echo "SDL2 (desenvolvimento): $(pkg-config --modversion sdl2) -> linkada estaticamente no bundle"
    SDL2_MODE=linked
elif [ -f /opt/homebrew/lib/libSDL2-2.0.dylib ] || [ -f /usr/local/lib/libSDL2-2.0.dylib ]; then
    echo "SDL2 (runtime) encontrada -> dlopen + rpath do Homebrew"
    SDL2_MODE=dynamic
else
    echo "SDL2 ausente -> o viewer nao vai abrir (o resto funciona)."
    echo "  recomendado: brew install sdl2"
    SDL2_MODE=dynamic
fi

[ "$CLEAN" -eq 1 ] && rm -rf "$BUILD_DIR"

echo "== compilando =="
cmake -S "$PORT_DIR" -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE=Release \
    -DT4PORT_SDL2="$SDL2_MODE"
cmake --build "$BUILD_DIR" -j"$(sysctl -n hw.ncpu 2>/dev/null || echo 4)"

[ -x "$BUILD_DIR/t4port" ] || die "build nao gerou o binario"

echo "== montando o bundle =="
rm -rf "$APP"
mkdir -p "$APP/Contents/MacOS" "$APP/Contents/Resources" "$APP/Contents/Frameworks"
cp "$BUILD_DIR/t4port" "$APP/Contents/MacOS/t4port"
cp "$SCRIPT_DIR/Info.plist" "$APP/Contents/Info.plist"

# --- SDL2 em modo dlopen: garante que o dlopen ache a dylib -------------
DYLIB=""
for c in /opt/homebrew/lib/libSDL2-2.0.dylib /usr/local/lib/libSDL2-2.0.dylib; do
    [ -f "$c" ] && DYLIB="$c" && break
done
if [ "$SDL2_MODE" = "dynamic" ] && [ -n "$DYLIB" ]; then
    cp -f "$DYLIB" "$APP/Contents/Frameworks/libSDL2-2.0.dylib"
    chmod 755 "$APP/Contents/Frameworks/libSDL2-2.0.dylib"
    install_name_tool -id "@executable_path/../Frameworks/libSDL2-2.0.dylib" \
        "$APP/Contents/Frameworks/libSDL2-2.0.dylib" 2>/dev/null || true
    # corrige as dependencias internas da propria dylib
    while IFS= read -r dep; do
        base="$(basename "$dep")"
        if [ -f "$APP/Contents/Frameworks/$base" ]; then
            install_name_tool -change "$dep" "@executable_path/../Frameworks/$base" \
                "$APP/Contents/Frameworks/$base" 2>/dev/null || true
        fi
    done < <(otool -L "$APP/Contents/Frameworks/libSDL2-2.0.dylib" | tail -n +2 | awk '{print $1}')
    install_name_tool -add_rpath "@executable_path/../Frameworks" \
        "$APP/Contents/MacOS/t4port" 2>/dev/null || true
    echo "  SDL2 copiada para Contents/Frameworks"
fi

# --- dados do jogo ------------------------------------------------------
if [ -n "$DATA_SRC" ]; then
    [ -d "$DATA_SRC" ] || die "diretorio de dados nao encontrado: $DATA_SRC"
    [ -d "$DATA_SRC/main" ] || die "$DATA_SRC nao parece ser o jogo (falta main/)"
    if [ "$DO_LINK" -eq 1 ]; then
        ln -sfn "$DATA_SRC" "$APP/Contents/Resources/game"
        echo "  dados: symlink -> $DATA_SRC"
    else
        echo "  dados: copiando 8,2 GB (minutos)..."
        mkdir -p "$APP/Contents/Resources/game"
        cp -a "$DATA_SRC/." "$APP/Contents/Resources/game/"
        echo "  dados: ok ($(du -sh "$APP/Contents/Resources/game" | awk '{print $1}'))"
    fi
fi

echo "== assinatura ad-hoc =="
codesign --force --deep --sign - "$APP" 2>/dev/null \
    || echo "  aviso: codesign falhou (o .app ainda roda; para distribuir, assine com ID)"
xattr -dr com.apple.quarantine "$APP" 2>/dev/null || true

if [ "$DO_INSTALL" -eq 1 ]; then
    echo "== instalando em /Applications =="
    rm -rf "/Applications/T4Port.app"
    cp -R "$APP" "/Applications/T4Port.app"
    echo "  /Applications/T4Port.app"
fi

echo
echo "== pronto =="
echo "  $APP"
echo "  abrir:   open \"$APP\""
echo "  terminal: \"$APP/Contents/MacOS/t4port\" --check"
echo "  sem dados embutidos, aponte para eles com:"
echo "    T4_GAME_DIR=/caminho/para/pluto_t4_full_game open \"$APP\""
