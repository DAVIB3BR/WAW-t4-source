#!/bin/sh
# Root shortcut: points to the official launcher in packaging/linux/.
# NO ARGUMENTS = IT STARTS THE GAME (retail client, offline).
# Diagnostics: ./t4port.sh --check    Tools: ./t4port.sh info | gpu | menu ...
exec "$(cd -- "$(dirname -- "$0")" && pwd)/packaging/linux/t4port.sh" "$@"
