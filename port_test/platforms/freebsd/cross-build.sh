#!/bin/sh
# T4Port - cross-build do binario FreeBSD (amd64) a partir de Linux/macOS.
#
# Complementa o build.sh nativo (que roda NO FreeBSD): este aqui gera e
# valida o binario FreeBSD sem precisar de uma maquina FreeBSD, usando o
# zig como toolchain embutido (libc do FreeBSD incluida).
# No proprio FreeBSD, use ./build.sh.
set -eu
HERE=$(cd "$(dirname "$0")" && pwd)
ROOT=$(cd "$HERE/../.." && pwd)
OUT="$ROOT/dist/freebsd"
ZLIBDIR="$HERE/.zlib-freebsd"
mkdir -p "$OUT"

SRC="$ROOT/src/main.c $ROOT/src/iwd.c $ROOT/src/ff.c $ROOT/src/iwi.c \
$ROOT/src/gfx.c $ROOT/src/gfx_gl.c $ROOT/src/gfx_vk.c \
$ROOT/src/gsc.c $ROOT/src/mods.c $ROOT/src/net.c $ROOT/src/platform.c \
$ROOT/src/png.c $ROOT/src/sdl2_dyn.c $ROOT/src/zonewalk.c \
$ROOT/src/world.c $ROOT/src/gapi.c $ROOT/src/menu.c"
CFLAGS="-std=c11 -O2 -Wall -Wextra -Wno-missing-field-initializers -I$ROOT/src"
ZLIB_VER=1.3.1
TARGET=x86_64-freebsd
# sysroot FreeBSD real (base.txz extraido) para os headers: o zig embutido
# nao traz a arvore de includes do FreeBSD, mas o sysroot tem tudo (e o
# linker continua usando a libc do proprio zig - binario autocontido)
SYSROOT="$HERE/.sysroot"
SYSINC=""
if [ -d "$SYSROOT/usr/include" ]; then
    SYSINC="-isystem $SYSROOT/usr/include"
    echo "headers FreeBSD: $SYSROOT/usr/include"
else
    echo "aviso: .sysroot ausente - rode platforms/freebsd/build.sh no FreeBSD" >&2
fi

# ---------------------------------------------------------------- ferramenta
if [ -n "${ZIG:-}" ]; then
    ZIGBIN=$ZIG
elif command -v zig >/dev/null 2>&1; then
    ZIGBIN=$(command -v zig)
elif [ -x "$ROOT/tools/.zig/zig-linux-x86_64-0.13.0/zig" ]; then
    ZIGBIN="$ROOT/tools/.zig/zig-linux-x86_64-0.13.0/zig"
else
    echo "erro: zig nao encontrado (baixe em ziglang.org ou defina ZIG=...)" >&2
    exit 1
fi
echo "zig: $ZIGBIN"
cc_fb() { "$ZIGBIN" cc -target "$TARGET" "$@"; }

mkdir -p "$OUT"
printf '#include <sys/types.h>\nint main(void){return (int)sizeof(off_t)-8;}\n' \
    > "$OUT/.probe_fb.c"
if ! cc_fb -c $SYSINC -o /dev/null "$OUT/.probe_fb.c" 2>/dev/null; then
    echo "aviso: nem sysroot nem zig com headers FreeBSD - tentando mesmo assim..." >&2
fi
rm -f "$OUT/.probe_fb.c"

# --------------------------------------------------------------------- zlib
if [ -f "$ZLIBDIR/libz.a" ]; then
    ZLIB_INC="-I$ZLIBDIR/src"
    ZLIB_LINK="$ZLIBDIR/libz.a"
else
    echo "zlib: baixando fonte v$ZLIB_VER (cache em $ZLIBDIR)"
    mkdir -p "$ZLIBDIR"
    if [ ! -f "$ZLIBDIR/src.tar.gz" ]; then
        curl -fsSL -o "$ZLIBDIR/src.tar.gz" \
            "https://github.com/madler/zlib/archive/refs/tags/v$ZLIB_VER.tar.gz" \
            || wget -q -O "$ZLIBDIR/src.tar.gz" \
            "https://github.com/madler/zlib/archive/refs/tags/v$ZLIB_VER.tar.gz"
    fi
    rm -rf "$ZLIBDIR/src"
    mkdir -p "$ZLIBDIR/src"
    tar xzf "$ZLIBDIR/src.tar.gz" -C "$ZLIBDIR/src" --strip-components=1
    [ -f "$ZLIBDIR/src/zconf.h" ] || cp "$ZLIBDIR/src/zconf.h.in" \
        "$ZLIBDIR/src/zconf.h"
    mkdir -p "$ZLIBDIR/obj"
    for f in adler32 crc32 deflate infback inffast inflate inftrees \
             trees zutil compress uncompr; do
        cc_fb -O2 -w $SYSINC -c "$ZLIBDIR/src/$f.c" -o "$ZLIBDIR/obj/$f.o"
    done
    "$ZIGBIN" ar rcs "$ZLIBDIR/libz.a" "$ZLIBDIR"/obj/*.o
    ZLIB_INC="-I$ZLIBDIR/src"
    ZLIB_LINK="$ZLIBDIR/libz.a"
fi

echo "== compilando t4port (FreeBSD amd64) =="
OBJDIR="$OUT/obj"
mkdir -p "$OBJDIR"
OBJS=""
# shellcheck disable=SC2086
for s in $SRC; do
    o="$OBJDIR/$(basename "$s" .c).o"
    cc_fb $CFLAGS $SYSINC $ZLIB_INC -c "$s" -o "$o"
    OBJS="$OBJS $o"
done
# link dinamico com crt/libc REAIS do sysroot (o zig nao fornece libc
# FreeBSD para link). Os .so.7 entram DIRETO no comando: o linker script
# usr/lib/libc.so do base.txz usa caminhos absolutos (/lib/...) que nao
# existem no host - resolvendo via -lc pelo script, nada da libc era
# encontrado. O binario fica NEEDED libc.so.7/libm.so.5 e roda com a libc do
# proprio FreeBSD da maquina alvo - binario FreeBSD de verdade, sem nada
# emulado.
rm -f "$OUT/t4port"
# shellcheck disable=SC2086
if ! cc_fb -nostdlib \
    "$SYSROOT/usr/lib/crt1.o" "$SYSROOT/usr/lib/crti.o" $OBJS $ZLIB_LINK \
    "$SYSROOT/lib/libm.so.5" "$SYSROOT/lib/libc.so.7" \
    "$SYSROOT/usr/lib/libc_nonshared.a" \
    "$SYSROOT/usr/lib/crtn.o" -o "$OUT/t4port"; then
    # o zig 0.13 imprime "error: libc not available" e retorna 1 MESMO quando
    # o lld linkou tudo (nostdlib + libc explicita fora do gerenciamento do
    # zig): so e falha de verdade se o ELF nao saiu completo
    if [ ! -s "$OUT/t4port" ] || \
       ! file "$OUT/t4port" | grep -q "FreeBSD"; then
        echo "erro: link FreeBSD falhou de verdade" >&2
        exit 1
    fi
    echo "aviso: zig retornou 1 ('libc not available' spurio); ELF gerado e valido" >&2
fi
echo "ok: $OUT/t4port"
file "$OUT/t4port" 2>/dev/null || true
readelf -d "$OUT/t4port" 2>/dev/null | grep NEEDED || true

echo
echo "Nao roda no Linux (ELF FreeBSD): copie para uma maquina FreeBSD e:"
echo "  ./t4port --check"
echo "No FreeBSD o driver NVIDIA aparece em: ./t4port gpu"
