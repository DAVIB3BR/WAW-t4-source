#!/bin/sh
# Build do T4Port para macOS — DEVE rodar num Mac (ferramentas da Apple).
set -eu
HERE=$(cd "$(dirname "$0")" && pwd)
ROOT=$(cd "$HERE/.." && pwd)

case "$(uname -s)" in
    Darwin) ;;
    *) echo "erro: este build precisa de macOS (uname=$(uname -s))." >&2
       echo "      copie a pasta port/ para o Mac e rode de novo la." >&2
       exit 1 ;;
esac

if ! command -v xcode-select >/dev/null 2>&1 || ! xcode-select -p >/dev/null 2>&1
then
    echo "instale as ferramentas primeiro: xcode-select --install" >&2
    exit 1
fi

exec "$ROOT/packaging/macos/build_macos.sh" "$@"
