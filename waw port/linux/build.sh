#!/bin/sh
# Build do T4Port para Linux. Delega para o build da raiz (+ Flatpak opcional).
set -eu
HERE=$(cd "$(dirname "$0")" && pwd)
ROOT=$(cd "$HERE/.." && pwd)

FLATPAK=0
for a in "$@"; do [ "$a" = "--flatpak" ] && FLATPAK=1; done

if command -v ninja >/dev/null 2>&1; then GEN="-G Ninja"; else GEN=""; fi

echo "== cmake =="
cmake -S "$ROOT" -B "$ROOT/build" $GEN -DCMAKE_BUILD_TYPE=Release
cmake --build "$ROOT/build" -j"$(nproc 2>/dev/null || echo 2)"

echo "== smoke =="
"$ROOT/build/t4port" --check

if [ "$FLATPAK" = "1" ]; then
    echo "== flatpak =="
    "$ROOT/packaging/linux/build-flatpak.sh" --clean --install
fi

echo "binario: $ROOT/build/t4port"
