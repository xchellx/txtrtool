#!/usr/bin/env bash
dp0="$(dirname $(readlink -m $BASH_SOURCE))"

EXEC="$dp0/build/txtrtool"
[ "$(expr substr $(uname -s) 1 10)" == "MINGW32_NT" ] || [ "$(expr substr $(uname -s) 1 10)" == "MINGW64_NT" ] && EXEC="$EXEC.exe"

[ -n "$1" ] && [ -d "$1" ] && indir="$1" || indir="$dp0/.local/testing"
[ -n "$2" ] && [ -d "$2" ] && outdir="$2" || outdir="$dp0/.local/testing/out"

function test-start {
    echo "________________________________________________________________________________"
    echo "$1"
}

function test-end {
    if [ $1 -ne 0 ]; then echo "__________________ ERROR: returned non-zero status of $1  __________________" ; fi
    echo "________________________________________________________________________________"
}

[ -n "$IFS" ] && OLDIFS="$IFS"
IFS=$'\n'
for F in `find "$indir" -maxdepth 1 -type f -name "*.TXTR"`; do
    if [ -n "$F" ] && [ -f "$F" ] ; then
        test-start "\"$EXEC\" decode -my -a_ \"$F\" \"$outdir\""
        "$EXEC" decode -my -a_ "$F" "$outdir"
        test-end $?
    fi
done
[ -n "$OLDIFS" ] && IFS="$OLDIFS" || unset IFS
