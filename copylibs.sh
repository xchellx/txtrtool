#!/usr/bin/env bash
dp0="$(dirname $(readlink -m $BASH_SOURCE))"
[ -n "$1" ] && [ -d "$1" ] && outdir="$1" || outdir="$dp0/build"
[ -n "$2" ] && [ -d "$2" ] && _MSYS_PATH="$2" || _MSYS_PATH="/c/msys64/clang64/bin"
echo "Copying $_MSYS_PATH/libc++.dll to $outdir/libc++.dll..."
cp "$_MSYS_PATH/libc++.dll" "$outdir/libc++.dll"
