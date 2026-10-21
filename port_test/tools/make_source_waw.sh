#!/bin/sh
# Monta a pasta "source waw" (fonte completa do porte para Windows, macOS,
# Linux e FreeBSD) a partir da arvore atual do port.
#
#   ./tools/make_source_waw.sh [destino]
#
# O destino padrao e "/home/davi/Plutonium/source waw". A arvore gerada e
# auto-suficiente: CMakeLists + src/ (inclui gapi.c/menu.c, a opcao de APIs
# graficas com deteccao do sistema), as pastas linux/ macos/ windows/
# freebsd/ com build.sh cada, packaging/ (Flatpak), doc/ (logo/imagens),
# re/ (ferramentas de RE), tools/, dist/ e o snapshot source_original/.
set -eu

SRC=$(cd "$(dirname "$0")/.." && pwd)
DST=${1:-"/home/davi/Plutonium/source waw"}

echo "origem : $SRC"
echo "destino: $DST"

mkdir -p "$DST"

# nucleo (README: preserva o ja customizado da pasta destino - secao do
# menu de video e caminhos das pastas de SO no topo)
cp "$SRC/CMakeLists.txt" "$DST/"
if [ ! -f "$DST/README.md" ]; then cp "$SRC/README.md" "$DST/"; fi

# codigo-fonte + materiais de apoio (docs, ferramentas, snapshot)
cp -r "$SRC/src" "$DST/"
cp -r "$SRC/doc" "$DST/"
cp -r "$SRC/re" "$DST/"
# tools via tar para deixar de fora o toolchain zig (.zig = ~380 MB)
rm -rf "$DST/tools"
mkdir -p "$DST/tools"
( cd "$SRC/tools" && tar cf - --exclude='.zig' . ) | \
( cd "$DST/tools" && tar xf - )
cp -r "$SRC/dist" "$DST/"
cp -r "$SRC/source_original" "$DST/"
cp -r "$SRC/platforms/README.md" "$DST/BUILDING.md"
# atalho na raiz: sem argumentos ele abre o jogo
cp "$SRC/t4port.sh" "$DST/t4port.sh"
chmod +x "$DST/t4port.sh"
cp "$SRC/PROJECT.md" "$DST/PROJECT.md"

# um diretorio por SO no topo (o que os build.sh chamam de ROOT sobe um nivel)
for os in linux macos windows freebsd; do
    rm -rf "$DST/$os"
    mkdir -p "$DST/$os"
    # tar em vez de cp -r: preserva os hardlinks do sysroot FreeBSD e
    # deixa fora os caches de cross (.sysroot ~800 MB, .zlib, base.txz)
    ( cd "$SRC/platforms/$os" && \
      tar cf - --exclude='.sysroot' --exclude='.zlib' \
               --exclude='.zlib-freebsd' --exclude='base.txz' . ) | \
    ( cd "$DST/$os" && tar xf - )
    # scripts usam $HERE/../.. (raiz = avo); aqui a raiz e o pai
    sed -i 's|\$HERE/\.\./\.\.|\$HERE/..|' "$DST/$os/build.sh"
done

# packaging (Flatpak etc.) sem os caches de build escondidos (8+ GB)
rm -rf "$DST/packaging"
mkdir -p "$DST/packaging"
( cd "$SRC/packaging" && \
  tar cf - --exclude='.flatpak-build' --exclude='.flatpak-repo' \
           --exclude='.flatpak-builder' \
           --exclude='.zig' --exclude='.sysroot' \
           --exclude='.zlib' --exclude='.zlib-freebsd' \
           --exclude='base.txz' . ) | \
( cd "$DST/packaging" && tar xf - )

# BUILDING.md: caminhos relativos agora sao em relacao a raiz
sed -i 's|\.\./src/|src/|g; s|\.\./doc/|doc/|g' "$DST/BUILDING.md"

# sanity check
for f in CMakeLists.txt README.md BUILDING.md src/main.c src/gapi.c \
         src/menu.c src/zonewalk.c src/world.c linux/build.sh \
         macos/build.sh windows/build.sh freebsd/build.sh \
         doc/images/cod_waw_logo.png packaging/linux/com.t4port.engine.yml \
         t4port.sh PROJECT.md; do
    if [ ! -f "$DST/$f" ]; then echo "FALTA: $f" >&2; exit 1; fi
done

nfiles=$(find "$DST" -type f | wc -l)
echo "OK: $nfiles arquivos em $DST"
grep -c 'HERE/..' "$DST/linux/build.sh" >/dev/null && \
    echo "OK: ROOT dos build.sh ajustado"
