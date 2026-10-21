#!/bin/sh
# T4Port — build para FreeBSD (roda no FreeBSD).
set -eu
HERE=$(cd "$(dirname "$0")" && pwd)
ROOT=$(cd "$HERE/../.." && pwd)

case "$(uname -s)" in
    FreeBSD|DragonFly) ;;
    *) echo "aviso: este script e para FreeBSD (uname=$(uname -s))." >&2 ;;
esac

for tool in cmake ninja; do
    command -v "$tool" >/dev/null 2>&1 || {
        echo "falta $tool -> pkg install cmake ninja" >&2
        exit 1
    }
done

if ! echo '#include <zlib.h>' | cc -E - >/dev/null 2>&1; then
    echo "faltam os headers do zlib -> pkg install zlib" >&2
    exit 1
fi

GEN=""
command -v ninja >/dev/null 2>&1 && GEN="-G Ninja"

echo "== cmake =="
cmake -S "$ROOT" -B "$ROOT/build-freebsd" $GEN -DCMAKE_BUILD_TYPE=Release
cmake --build "$ROOT/build-freebsd"

echo "== smoke =="
"$ROOT/build-freebsd/t4port" --check
"$ROOT/build-freebsd/t4port" info "${T4_GAME_DIR:-$HOME/Plutonium/pluto_t4_full_game}" || true

echo "binario: $ROOT/build-freebsd/t4port"
