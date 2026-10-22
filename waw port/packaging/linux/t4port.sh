#!/usr/bin/env bash
#
# t4port.sh - launcher do porte nativo de Call of Duty: World at War (motor T4).
#
# Procura o jogo e o binario nativo em varios locais, permitindo mover/reinstalar
# o jogo sem reconfigurar nada. Roda em Linux (bash) e tambem dentro do Flatpak.
#
# Uso:
#   ./t4port.sh                     INICIA O JOGO (padrao sem argumentos)
#   ./t4port.sh menu                opcoes de video (Options > Graphics, em ingles)
#   ./t4port.sh info                inventario + status
#   ./t4port.sh zip <a.iwd> [filtro]
#   ./t4port.sh ff <a.ff>
#   ./t4port.sh iwi <a.iwi> -o x.png
#   ./t4port.sh view <a.iwi>        abre janela com a textura
#   ./t4port.sh extract <a.iwd> <entrada> -o <saida>
#   ./t4port.sh play                abre o jogo retail sob Wine (compat)
#   ./t4port.sh --wine              roda o cliente original sob Wine (compat)
#   ./t4port.sh --check             so diagnostica o ambiente
#
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
SELF_NAME="$(basename -- "$0")"

# ---------------------------------------------------------------- localizar
find_game_dir() {
    local candidates=(
        "${T4_GAME_DIR:-}"
        "/app/share/t4port/game"
        "$SCRIPT_DIR/../../../Call of Duty World At War"   # copia classica completa
        "$SCRIPT_DIR/../../../pluto_t4_full_game"     # layout do repositorio (fallback)
        "$SCRIPT_DIR/../pluto_t4_full_game"
        "$HOME/Plutonium/Call of Duty World At War"
        "$HOME/Plutonium/pluto_t4_full_game"
        "$HOME/.local/share/t4port/game"
        "/opt/t4port/game"
    )
    local c
    for c in "${candidates[@]}"; do
        [ -n "$c" ] || continue
        if [ -d "$c/main" ] && [ -d "$c/zone" ]; then
            printf '%s' "$c"
            return 0
        fi
    done
    return 1
}

find_engine() {
    local candidates=(
        "${T4PORT_BIN:-}"
        "$SCRIPT_DIR/t4port"
        "$SCRIPT_DIR/build/t4port"
        "$SCRIPT_DIR/../../build/t4port"
        "$SCRIPT_DIR/../build/t4port"
        "$SCRIPT_DIR/../../../build/t4port"
        "/app/bin/t4port"
        "$HOME/.local/bin/t4port"
    )
    local c
    for c in "${candidates[@]}"; do
        [ -n "$c" ] || continue
        if [ -x "$c" ]; then
            printf '%s' "$c"
            return 0
        fi
    done
    if command -v t4port >/dev/null 2>&1; then
        command -v t4port
        return 0
    fi
    return 1
}

GAME_DIR=""
ENGINE=""
GAME_DIR="$(find_game_dir)" || true
ENGINE="$(find_engine)" || true

# ------------------------------------------------------------------ check
if [ "${1:-}" = "--check" ]; then
    echo "== diagnostico t4port =="
    echo "launcher      : $SCRIPT_DIR/$SELF_NAME"
    if [ -n "$ENGINE" ]; then
        echo "motor nativo  : $ENGINE  [ok]"
    else
        echo "motor nativo  : NAO ENCONTRADO (compile com: cmake -S . -B build && cmake --build build)"
    fi
    if [ -n "$GAME_DIR" ]; then
        echo "dados do jogo : $GAME_DIR  [ok]"
        echo "  .iwd        : $(find "$GAME_DIR/main" -maxdepth 1 -name '*.iwd' 2>/dev/null | wc -l)"
        echo "  fastfiles   : $(find "$GAME_DIR/zone" -name '*.ff' 2>/dev/null | wc -l)"
    else
        echo "dados do jogo : NAO ENCONTRADOS (exporte T4_GAME_DIR=/caminho/para/pluto_t4_full_game)"
    fi
    for tool in wine wine64 ffmpeg cmake; do
        if command -v "$tool" >/dev/null 2>&1; then
            echo "$tool: $(command -v "$tool")"
        else
            echo "$tool: ausente (opcional)"
        fi
    done
    exit 0
fi

# Sem argumentos INICIA O JOGO (cliente retail classico - e o que se espera
# ao clicar no .sh/.desktop). As opcoes de video do porte ficam em 'menu'
# (Options > Graphics, em ingles); ferramentas em 'info'/'--check'.
if [ $# -eq 0 ] || [ "${1:-}" = "play" ]; then
    set -- --wine
fi

# ------------------------------------------------------------------- wine
if [ "${1:-}" = "--wine" ]; then
    shift
    # Dentro do Flatpak nao existe wine no sandbox (o jogo e o Wine ficam
    # no HOST): a manifest exporta T4_HOST_LAUNCHER e a permissao de talk
    # com org.freedesktop.Flatpak, entao o clique no .desktop delega pro
    # launcher do host e o jogo abre na sessao do usuario.
    if ! command -v wine >/dev/null 2>&1 && ! command -v wine64 >/dev/null 2>&1 \
        && [ -f /.flatpak-info ] && command -v flatpak-spawn >/dev/null 2>&1 \
        && [ -n "${T4_HOST_LAUNCHER:-}" ]; then
        echo "$SELF_NAME: sem wine no sandbox; delegando ao host: $T4_HOST_LAUNCHER"
        if flatpak-spawn --host "$T4_HOST_LAUNCHER" play; then
            exit 0
        fi
        echo "$SELF_NAME: ponte host falhou; rode ./t4port.sh play fora do Flatpak" >&2
        exit 1
    fi
    WINE_BIN="$(command -v wine || command -v wine64 || true)"
    if [ -z "$WINE_BIN" ]; then
        echo "$SELF_NAME: wine nao instalado. Instale com:" >&2
        echo "  openSUSE:  sudo zypper in wine" >&2
        echo "  Debian:    sudo apt install wine" >&2
        echo "  Flatpak:   flatpak install org.winehq.Wine" >&2
        exit 1
    fi
    # Executavel: argumento explicito > cliente ORIGINAL (copia completa,
    # campanha/zombies/LAN offline) > cliente MP. O launcher da Plutonium
    # (plutonium.exe, servico online deles) NUNCA e escolhido automaticamente:
    # o modo classico padrao e o jogo retail puro; online deles roda pelo
    # launcher deles, fora daqui. Roda no proprio diretorio do exe, entao o
    # jogo le os dados ao lado dele.
    EXE=""
    if [ -n "${1:-}" ] && [ -f "${1:-}" ]; then
        EXE="$(cd -- "$(dirname -- "$1")" && pwd)/$(basename -- "$1")"
        shift
    else
        # preferencia por EXECUTAVEL primeiro (original > mp), varrendo as
        # pastas em cada nivel - assim o CoDWaW.exe da copia completa vence
        # qualquer outro candidato
        for e in CoDWaW.exe CoDWaWmp.exe; do
            for d in "$GAME_DIR" \
                     "$HOME/Plutonium/Call of Duty World At War"; do
                [ -n "$d" ] && [ -d "$d" ] || continue
                if [ -f "$d/$e" ]; then EXE="$d/$e"; break; fi
            done
            [ -n "$EXE" ] && break
        done
    fi
    if [ -z "$EXE" ]; then
        echo "$SELF_NAME: nenhum executavel original (CoDWaW.exe/CoDWaWmp.exe) encontrado" >&2
        exit 1
    fi
    # prefixo dedicado do jogo; WINEDLLOVERRIDES pula os dialogs mono/gecko
    export WINEPREFIX="${WINEPREFIX:-$HOME/.wine-waw}"
    export WINEDLLOVERRIDES="${WINEDLLOVERRIDES:-mscoree,mshtml=}"
    export WINEDEBUG="${WINEDEBUG:--all}"
    # limpa o marcador de "sessao anterior nao fechou limpo" (__CoDWaW): sob
    # Wine a tela de bloqueio mata o jogo sem clean exit e na proxima vez o
    # motor para num dialogo "Run In Safe Mode?" invisivel - removendo, abre
    # direto no modo classico normal
    for mk in "$WINEPREFIX"/drive_c/users/*/AppData/Local/Activision/*/__CoDWaW; do
        if [ -f "$mk" ]; then rm -f "$mk"; echo "marcador de sessao anterior removido: $mk"; fi
    done
    echo "executando sob Wine (prefixo $WINEPREFIX): $EXE"
    cd -- "$(dirname -- "$EXE")"
    exec "$WINE_BIN" "$(basename -- "$EXE")" "$@"
fi

# ------------------------------------------------------------- sem engine
if [ -z "$ENGINE" ]; then
    echo "$SELF_NAME: motor nativo nao encontrado." >&2
    echo "  build:  cmake -S <este_repositorio> -B build && cmake --build build" >&2
    echo "  ou     flatpak-builder build (ver packaging/linux/)" >&2
    exit 1
fi

# --------------------------------------------------------------- execucao
if [ -z "$GAME_DIR" ]; then
    # subcomandos que nao precisam dos dados do jogo continuam funcionando
    # (menu de video, ferramentas, --check etc); os que precisam (`world`)
    # mostram o proprio erro com a dica de T4_GAME_DIR
    exec "$ENGINE" "$@"
fi

export T4_GAME_DIR="$GAME_DIR"

exec "$ENGINE" "$@"
