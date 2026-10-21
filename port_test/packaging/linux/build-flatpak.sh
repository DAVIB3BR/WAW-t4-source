#!/usr/bin/env bash
#
# build-flatpak.sh - constroi e opcionalmente instala o Flatpak do t4port.
#
# Nao exige root: usa flatpak-builder instalado como usuario (se o binario nao
# existir, instala o app "org.flatpak.Builder" do Flathub como usuario).
#
# Uso:
#   ./build-flatpak.sh                 build (incremental)
#   ./build-flatpak.sh --clean         build do zero (apaga cache)
#   ./build-flatpak.sh --install       build + install --user
#   ./build-flatpak.sh --run [args...] build + executa
#   ./build-flatpak.sh --manifest      so valida o manifesto
#
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
MANIFEST="$SCRIPT_DIR/com.t4port.engine.yml"
APP_ID="com.t4port.engine"
BUILD_DIR="${T4PORT_BUILD_DIR:-$SCRIPT_DIR/.flatpak-build}"
REPO_DIR="${T4PORT_REPO_DIR:-$SCRIPT_DIR/.flatpak-repo}"
STATE_DIR="$HOME/.local/share/t4port-flatpak"

EXTRA_ARGS=()
DO_INSTALL=0
DO_RUN=0
MANIFEST_ONLY=0
RUN_ARGS=()

while [ $# -gt 0 ]; do
    case "$1" in
        --clean)    EXTRA_ARGS+=(--force-clean) ;;
        --install)  DO_INSTALL=1 ;;
        --run)       DO_RUN=1; shift; RUN_ARGS=("$@"); break ;;
        --manifest)  MANIFEST_ONLY=1 ;;
        *) echo "opcao desconhecida: $1" >&2; exit 2 ;;
    esac
    shift
done

echo "== verificando ferramentas =="
if ! command -v flatpak >/dev/null 2>&1; then
    echo "flatpak ausente. openSUSE: sudo zypper in flatpak" >&2
    exit 1
fi

BUILDER_CMD=()
if command -v flatpak-builder >/dev/null 2>&1; then
    BUILDER_CMD=(flatpak-builder)
    echo "flatpak-builder: $(command -v flatpak-builder)"
else
    echo "flatpak-builder do sistema ausente -> usando org.flatpak.Builder (Flathub, usuario)"
    if ! flatpak info org.flatpak.Builder >/dev/null 2>&1; then
        echo "instalando org.flatpak.Builder (sem root)..."
        flatpak remote-add --user --if-not-exists flathub \
            https://dl.flathub.org/repo/flathub.flatpakrepo
        flatpak install --user -y flathub org.flatpak.Builder
    fi
    BUILDER_CMD=(flatpak run org.flatpak.Builder)
fi

# runtime/sdk 25.08 e o escolhido no manifesto por ja estar disponivel
if ! flatpak info org.freedesktop.Sdk//25.08 >/dev/null 2>&1 && \
   ! flatpak info --system org.freedesktop.Sdk//25.08 >/dev/null 2>&1; then
    echo "baixando org.freedesktop.Sdk//25.08 (~1,5 GB)..."
    flatpak install --user -y flathub org.freedesktop.Sdk//25.08 || \
        flatpak install --system -y flathub org.freedesktop.Sdk//25.08 || true
fi

if [ "$MANIFEST_ONLY" -eq 1 ]; then
    echo "== validando manifesto =="
    "${BUILDER_CMD[@]}" --show-manifest "$MANIFEST"
    exit 0
fi

echo "== build =="
echo "manifesto : $MANIFEST"
echo "diretorio : $BUILD_DIR"
echo "repo      : $REPO_DIR"
echo "atencao: o modulo gamedata copia 8,2 GB - leve minutos."
mkdir -p "$BUILD_DIR" "$REPO_DIR" "$STATE_DIR"

# O flatpak-builder recusa reutilizar um diretorio de app deixado por um build
# que falhou. O carimbo marca que o diretorio contem um build valido.
STAMP="$BUILD_DIR/.t4port-ok"
if [ -d "$BUILD_DIR" ] && [ -n "$(ls -A "$BUILD_DIR" 2>/dev/null)" ] && [ ! -f "$STAMP" ]; then
    echo "diretorio de app de build anterior incompleto -> limpando: $BUILD_DIR"
    rm -rf "$BUILD_DIR"
    mkdir -p "$BUILD_DIR"
fi
rm -f "$STAMP"

"${BUILDER_CMD[@]}" "${EXTRA_ARGS[@]}" \
    --repo="$REPO_DIR" \
    "$BUILD_DIR" "$MANIFEST"

touch "$STAMP"

if [ "$DO_INSTALL" -eq 1 ]; then
    echo "== instalando (usuario) =="
    flatpak --user remote-add --if-not-exists t4port "$REPO_DIR" --no-gpg-verify 2>/dev/null || true
    flatpak --user install -y t4port "$APP_ID"
    # BUG CORRIGIDO: "install" reporta "ja instalado" e NAO troca o commit
    # quando o app ja existe com um build antigo -> o porte velho ficava
    # rodando. "update" garante que o instalado e o recem-exportado.
    flatpak --user update -y "$APP_ID" 2>/dev/null || true
    echo "pronto: flatpak run $APP_ID"
fi

if [ "$DO_RUN" -eq 1 ]; then
    flatpak run "$APP_ID" "${RUN_ARGS[@]}"
fi

echo "== artefatos =="
du -sh "$BUILD_DIR" "$REPO_DIR" 2>/dev/null || true
echo "para instalar depois: flatpak --user remote-add --if-not-exists t4port $REPO_DIR --no-gpg-verify && flatpak --user install -y t4port $APP_ID"
