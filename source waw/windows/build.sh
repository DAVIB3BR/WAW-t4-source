#!/bin/sh
# T4Port — build de t4port.exe (Windows PE32+ x86-64).
#
# Roda em qualquer SO: Linux, macOS, MSYS2/Windows ou FreeBSD. Ferramentas,
# na ordem de preferencia:
#   1) zig   (traz o toolchain mingw embutido - o caminho testado)
#   2) x86_64-w64-mingw32-gcc (mingw-w64 instalado pelo sistema)
#   3) clang --target=x86_64-windows-gnu
#
# O zlib NAO e opcional (inflate dos .ff + compress2 do PNG): se o toolchain
# nao tiver um -lz de Windows, o script baixa o fonte do zlib e compila
# uma copia estatica em .zlib/ (cache local).
set -eu

HERE=$(cd "$(dirname "$0")" && pwd)
ROOT=$(cd "$HERE/.." && pwd)
OUT="$ROOT/dist/windows"
ZLIBDIR="$HERE/.zlib"
mkdir -p "$OUT"

SRC="$ROOT/src/main.c $ROOT/src/iwd.c $ROOT/src/ff.c $ROOT/src/iwi.c \
$ROOT/src/gfx.c $ROOT/src/gfx_gl.c $ROOT/src/gfx_vk.c \
$ROOT/src/gsc.c $ROOT/src/mods.c $ROOT/src/net.c $ROOT/src/platform.c \
$ROOT/src/png.c $ROOT/src/sdl2_dyn.c $ROOT/src/zonewalk.c \
$ROOT/src/world.c $ROOT/src/gapi.c $ROOT/src/menu.c"
CFLAGS="-std=c11 -O2 -Wall -Wextra -I$ROOT/src"
ZLIB_VER=1.3.1

# ---------------------------------------------------------------- ferramenta
if [ -n "${ZIG:-}" ]; then
    T=zig; ZIGBIN=$ZIG
elif command -v zig >/dev/null 2>&1; then
    T=zig; ZIGBIN=$(command -v zig)
elif command -v x86_64-w64-mingw32-gcc >/dev/null 2>&1; then
    T=mingw
elif command -v clang >/dev/null 2>&1; then
    T=clang
else
    echo "erro: nenhuma ferramenta para Windows encontrada." >&2
    echo "      instale o zig (https://ziglang.org) ou mingw-w64" >&2
    exit 1
fi
echo "ferramenta: $T"

cc_win() {  # cc_win <saida> <flag de -c?> <fontes...>
    case $T in
        zig)   "$ZIGBIN" cc -target x86_64-windows-gnu "$@" ;;
        mingw) x86_64-w64-mingw32-gcc "$@" ;;
        clang) clang --target=x86_64-windows-gnu "$@" ;;
    esac
}

# --------------------------------------------------------------------- zlib
have_system_zlib=0
if [ "$T" != "zig" ]; then
    if echo 'int main(){return 0;}' | cc_win -x c - -o "${TMPDIR:-/tmp}/t4pwz.exe" -lz 2>/dev/null
    then have_system_zlib=1; fi
fi

ZLIB_LIB=""
if [ "$have_system_zlib" = "1" ]; then
    ZLIB_LIB="-lz"
    echo "zlib: do sistema"
else
    if [ ! -f "$ZLIBDIR/libz.a" ]; then
        echo "zlib: baixando fonte v$ZLIB_VER (cache em $ZLIBDIR)"
        mkdir -p "$ZLIBDIR"
        if [ ! -f "$ZLIBDIR/src.tar.gz" ]; then
            curl -fsSL -o "$ZLIBDIR/src.tar.gz" \
                "https://github.com/madler/zlib/archive/refs/tags/v$ZLIB_VER.tar.gz" \
                || wget -q -O "$ZLIBDIR/src.tar.gz" \
                "https://github.com/madler/zlib/archive/refs/tags/v$ZLIB_VER.tar.gz"
        fi
        rm -rf "$ZLIBDIR/src"; mkdir -p "$ZLIBDIR/src"
        tar xzf "$ZLIBDIR/src.tar.gz" -C "$ZLIBDIR/src" --strip-components=1
        [ -f "$ZLIBDIR/src/zconf.h" ] || cp "$ZLIBDIR/src/zconf.h.in" "$ZLIBDIR/src/zconf.h"
        mkdir -p "$ZLIBDIR/obj"
        # um arquivo por invocacao (o zig recusa -c multiplo em COFF)
        for f in adler32 crc32 deflate infback inffast inflate inftrees \
                 trees zutil compress uncompr; do
            cc_win -O2 -w -c "$ZLIBDIR/src/$f.c" -o "$ZLIBDIR/obj/$f.obj"
        done
        case $T in
            zig)   "$ZIGBIN" ar rcs "$ZLIBDIR/libz.a" "$ZLIBDIR"/obj/*.obj ;;
            mingw) x86_64-w64-mingw32-ar rcs "$ZLIBDIR/libz.a" "$ZLIBDIR"/obj/*.obj ;;
            clang) llvm-ar rcs "$ZLIBDIR/libz.a" "$ZLIBDIR"/obj/*.obj ;;
        esac
    fi
    ZLIB_LIB="$ZLIBDIR/libz.a -I$ZLIBDIR/src"
    echo "zlib: estatica proprio ($ZLIBDIR/libz.a)"
fi

# ------------------------------------------------------------------- build
echo "== compilando t4port.exe =="
# shellcheck disable=SC2086
cc_win $CFLAGS $ZLIB_LIB $SRC -o "$OUT/t4port.exe" -lws2_32
echo "ok: $OUT/t4port.exe"
file "$OUT/t4port.exe" 2>/dev/null || true

echo
echo "Testar a partir do Linux/macOS (com wine):"
echo "  wine64 $OUT/t4port.exe ff <jogo>/zone/english/<mapa>.ff"
echo "No Windows: copie o jogo e rode t4port.exe pelo prompt."
