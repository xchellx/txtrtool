#!/usr/bin/env bash
dp0="$(dirname $(readlink -m $BASH_SOURCE))"
echo "Deleting all _version.h and _setings.h configured files from $dp0"
find "$dp0" \( -wholename '**/configure/*_version.h' -o -wholename '**/configure/*_settings.h' \) -delete
