#!/usr/bin/env bash
dp0="$(dirname $(readlink -m $BASH_SOURCE))"

EXEC="$dp0/build/txtrtool"
[ "$(expr substr $(uname -s) 1 10)" == "MINGW32_NT" ] || [ "$(expr substr $(uname -s) 1 10)" == "MINGW64_NT" ] && EXEC="$EXEC.exe"

[ -n "$1" ] && testtorun="$1" || testtorun="all"

function test-start {
    echo "________________________________________________________________________________"
    echo "$1"
}

function test-end {
    if [ $1 -ne 0 ]; then echo "__________________ ERROR: returned non-zero status of $1  __________________" ; fi
    echo "________________________________________________________________________________"
}

function test-run {
    if [ -n "$1" ] && [ -f "$1" ] && [ -n "$2" ] && [ -d "$2" ] && [ -n "$3" ] ; then
        infile="$1"
        infilename="${1%.*}"
        infilename="$(basename $infilename)"
        
        if [ "$3" == "all" ] || [ "$3" == "intensity" ] ; then
            # I4
            echo "Testing I4..."
            
            test-start "\"$EXEC\" encode -y -t I4 -m 1 --avgtype AVERAGE \"$infile\" \"$2/I4_AVERAGE_${infilename}.TXTR\""
            "$EXEC" encode -y -t I4 -m 1 --avgtype AVERAGE "$infile" "$2/I4_AVERAGE_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t I4 -m 0 --avgtype AVERAGE \"$infile\" \"$2/I4_AVERAGE_mips_${infilename}.TXTR\""
            "$EXEC" encode -y -t I4 -m 0 --avgtype AVERAGE "$infile" "$2/I4_AVERAGE_mips_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t I4 -m 1 --avgtype SQUARED \"$infile\" \"$2/I4_SQUARED_${infilename}.TXTR\""
            "$EXEC" encode -y -t I4 -m 1 --avgtype SQUARED "$infile" "$2/I4_SQUARED_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t I4 -m 0 --avgtype SQUARED \"$infile\" \"$2/I4_SQUARED_mips_${infilename}.TXTR\""
            "$EXEC" encode -y -t I4 -m 0 --avgtype SQUARED "$infile" "$2/I4_SQUARED_mips_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t I4 -m 1 --avgtype W3C \"$infile\" \"$2/I4_W3C_${infilename}.TXTR\""
            "$EXEC" encode -y -t I4 -m 1 --avgtype W3C "$infile" "$2/I4_W3C_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t I4 -m 0 --avgtype W3C \"$infile\" \"$2/I4_W3C_mips_${infilename}.TXTR\""
            "$EXEC" encode -y -t I4 -m 0 --avgtype W3C "$infile" "$2/I4_W3C_mips_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t I4 -m 1 --avgtype SRGB \"$infile\" \"$2/I4_SRGB_${infilename}.TXTR\""
            "$EXEC" encode -y -t I4 -m 1 --avgtype SRGB "$infile" "$2/I4_SRGB_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t I4 -m 0 --avgtype SRGB \"$infile\" \"$2/I4_SRGB_mips_${infilename}.TXTR\""
            "$EXEC" encode -y -t I4 -m 0 --avgtype SRGB "$infile" "$2/I4_SRGB_mips_${infilename}.TXTR"
            test-end $?
            
            # I8
            echo "Testing I8..."
            
            test-start "\"$EXEC\" encode -y -t I8 -m 1 --avgtype AVERAGE \"$infile\" \"$2/I8_AVERAGE_${infilename}.TXTR\""
            "$EXEC" encode -y -t I8 -m 1 --avgtype AVERAGE "$infile" "$2/I8_AVERAGE_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t I8 -m 0 --avgtype AVERAGE \"$infile\" \"$2/I8_AVERAGE_mips_${infilename}.TXTR\""
            "$EXEC" encode -y -t I8 -m 0 --avgtype AVERAGE "$infile" "$2/I8_AVERAGE_mips_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t I8 -m 1 --avgtype SQUARED \"$infile\" \"$2/I8_SQUARED_${infilename}.TXTR\""
            "$EXEC" encode -y -t I8 -m 1 --avgtype SQUARED "$infile" "$2/I8_SQUARED_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t I8 -m 0 --avgtype SQUARED \"$infile\" \"$2/I8_SQUARED_mips_${infilename}.TXTR\""
            "$EXEC" encode -y -t I8 -m 0 --avgtype SQUARED "$infile" "$2/I8_SQUARED_mips_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t I8 -m 1 --avgtype W3C \"$infile\" \"$2/I8_W3C_${infilename}.TXTR\""
            "$EXEC" encode -y -t I8 -m 1 --avgtype W3C "$infile" "$2/I8_W3C_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t I8 -m 0 --avgtype W3C \"$infile\" \"$2/I8_W3C_mips_${infilename}.TXTR\""
            "$EXEC" encode -y -t I8 -m 0 --avgtype W3C "$infile" "$2/I8_W3C_mips_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t I8 -m 1 --avgtype SRGB \"$infile\" \"$2/I8_SRGB_${infilename}.TXTR\""
            "$EXEC" encode -y -t I8 -m 1 --avgtype SRGB "$infile" "$2/I8_SRGB_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t I8 -m 0 --avgtype SRGB \"$infile\" \"$2/I8_SRGB_mips_${infilename}.TXTR\""
            "$EXEC" encode -y -t I8 -m 0 --avgtype SRGB "$infile" "$2/I8_SRGB_mips_${infilename}.TXTR"
            test-end $?
            
            # IA4
            echo "Testing IA4..."
            
            test-start "\"$EXEC\" encode -y -t IA4 -m 1 --avgtype AVERAGE \"$infile\" \"$2/IA4_AVERAGE_${infilename}.TXTR\""
            "$EXEC" encode -y -t IA4 -m 1 --avgtype AVERAGE "$infile" "$2/IA4_AVERAGE_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t IA4 -m 0 --avgtype AVERAGE \"$infile\" \"$2/IA4_AVERAGE_mips_${infilename}.TXTR\""
            "$EXEC" encode -y -t IA4 -m 0 --avgtype AVERAGE "$infile" "$2/IA4_AVERAGE_mips_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t IA4 -m 1 --avgtype SQUARED \"$infile\" \"$2/IA4_SQUARED_${infilename}.TXTR\""
            "$EXEC" encode -y -t IA4 -m 1 --avgtype SQUARED "$infile" "$2/IA4_SQUARED_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t IA4 -m 0 --avgtype SQUARED \"$infile\" \"$2/IA4_SQUARED_mips_${infilename}.TXTR\""
            "$EXEC" encode -y -t IA4 -m 0 --avgtype SQUARED "$infile" "$2/IA4_SQUARED_mips_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t IA4 -m 1 --avgtype W3C \"$infile\" \"$2/IA4_W3C_${infilename}.TXTR\""
            "$EXEC" encode -y -t IA4 -m 1 --avgtype W3C "$infile" "$2/IA4_W3C_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t IA4 -m 0 --avgtype W3C \"$infile\" \"$2/IA4_W3C_mips_${infilename}.TXTR\""
            "$EXEC" encode -y -t IA4 -m 0 --avgtype W3C "$infile" "$2/IA4_W3C_mips_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t IA4 -m 1 --avgtype SRGB \"$infile\" \"$2/IA4_SRGB_${infilename}.TXTR\""
            "$EXEC" encode -y -t IA4 -m 1 --avgtype SRGB "$infile" "$2/IA4_SRGB_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t IA4 -m 0 --avgtype SRGB \"$infile\" \"$2/IA4_SRGB_mips_${infilename}.TXTR\""
            "$EXEC" encode -y -t IA4 -m 0 --avgtype SRGB "$infile" "$2/IA4_SRGB_mips_${infilename}.TXTR"
            test-end $?
            
            # IA8
            echo "Testing IA8..."
            
            test-start "\"$EXEC\" encode -y -t IA8 -m 1 --avgtype AVERAGE \"$infile\" \"$2/IA8_AVERAGE_${infilename}.TXTR\""
            "$EXEC" encode -y -t IA8 -m 1 --avgtype AVERAGE "$infile" "$2/IA8_AVERAGE_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t IA8 -m 0 --avgtype AVERAGE \"$infile\" \"$2/IA8_AVERAGE_mips_${infilename}.TXTR\""
            "$EXEC" encode -y -t IA8 -m 0 --avgtype AVERAGE "$infile" "$2/IA8_AVERAGE_mips_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t IA8 -m 1 --avgtype SQUARED \"$infile\" \"$2/IA8_SQUARED_${infilename}.TXTR\""
            "$EXEC" encode -y -t IA8 -m 1 --avgtype SQUARED "$infile" "$2/IA8_SQUARED_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t IA8 -m 0 --avgtype SQUARED \"$infile\" \"$2/IA8_SQUARED_mips_${infilename}.TXTR\""
            "$EXEC" encode -y -t IA8 -m 0 --avgtype SQUARED "$infile" "$2/IA8_SQUARED_mips_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t IA8 -m 1 --avgtype W3C \"$infile\" \"$2/IA8_W3C_${infilename}.TXTR\""
            "$EXEC" encode -y -t IA8 -m 1 --avgtype W3C "$infile" "$2/IA8_W3C_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t IA8 -m 0 --avgtype W3C \"$infile\" \"$2/IA8_W3C_mips_${infilename}.TXTR\""
            "$EXEC" encode -y -t IA8 -m 0 --avgtype W3C "$infile" "$2/IA8_W3C_mips_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t IA8 -m 1 --avgtype SRGB \"$infile\" \"$2/IA8_SRGB_${infilename}.TXTR\""
            "$EXEC" encode -y -t IA8 -m 1 --avgtype SRGB "$infile" "$2/IA8_SRGB_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t IA8 -m 0 --avgtype SRGB \"$infile\" \"$2/IA8_SRGB_mips_${infilename}.TXTR\""
            "$EXEC" encode -y -t IA8 -m 0 --avgtype SRGB "$infile" "$2/IA8_SRGB_mips_${infilename}.TXTR"
            test-end $?
        fi
        
        if [ "$3" == "all" ] || [ "$3" == "color" ] ; then
            # R5G6B5
            echo "Testing R5G6B5..."
            
            test-start "\"$EXEC\" encode -y -t R5G6B5 -m 1 \"$infile\" \"$2/R5G6B5_${infilename}.TXTR\""
            "$EXEC" encode -y -t R5G6B5 -m 1 "$infile" "$2/R5G6B5_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t R5G6B5 -m 0 \"$infile\" \"$2/R5G6B5_mips_${infilename}.TXTR\""
            "$EXEC" encode -y -t R5G6B5 -m 0 "$infile" "$2/R5G6B5_mips_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t R5G6B5 -m 1 \"$infile\" \"$2/R5G6B5_${infilename}.TXTR\""
            "$EXEC" encode -y -t R5G6B5 -m 1 "$infile" "$2/R5G6B5_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t R5G6B5 -m 0 \"$infile\" \"$2/R5G6B5_mips_${infilename}.TXTR\""
            "$EXEC" encode -y -t R5G6B5 -m 0 "$infile" "$2/R5G6B5_mips_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t R5G6B5 -m 1 \"$infile\" \"$2/R5G6B5_${infilename}.TXTR\""
            "$EXEC" encode -y -t R5G6B5 -m 1 "$infile" "$2/R5G6B5_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t R5G6B5 -m 0 \"$infile\" \"$2/R5G6B5_mips_${infilename}.TXTR\""
            "$EXEC" encode -y -t R5G6B5 -m 0 "$infile" "$2/R5G6B5_mips_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t R5G6B5 -m 1 \"$infile\" \"$2/R5G6B5_${infilename}.TXTR\""
            "$EXEC" encode -y -t R5G6B5 -m 1 "$infile" "$2/R5G6B5_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t R5G6B5 -m 0 \"$infile\" \"$2/R5G6B5_mips_${infilename}.TXTR\""
            "$EXEC" encode -y -t R5G6B5 -m 0 "$infile" "$2/R5G6B5_mips_${infilename}.TXTR"
            test-end $?
            
            # RGB5A3
            echo "Testing RGB5A3..."
            
            test-start "\"$EXEC\" encode -y -t RGB5A3 -m 1 \"$infile\" \"$2/RGB5A3_${infilename}.TXTR\""
            "$EXEC" encode -y -t RGB5A3 -m 1 "$infile" "$2/RGB5A3_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t RGB5A3 -m 0 \"$infile\" \"$2/RGB5A3_mips_${infilename}.TXTR\""
            "$EXEC" encode -y -t RGB5A3 -m 0 "$infile" "$2/RGB5A3_mips_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t RGB5A3 -m 1 \"$infile\" \"$2/RGB5A3_${infilename}.TXTR\""
            "$EXEC" encode -y -t RGB5A3 -m 1 "$infile" "$2/RGB5A3_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t RGB5A3 -m 0 \"$infile\" \"$2/RGB5A3_mips_${infilename}.TXTR\""
            "$EXEC" encode -y -t RGB5A3 -m 0 "$infile" "$2/RGB5A3_mips_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t RGB5A3 -m 1 \"$infile\" \"$2/RGB5A3_${infilename}.TXTR\""
            "$EXEC" encode -y -t RGB5A3 -m 1 "$infile" "$2/RGB5A3_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t RGB5A3 -m 0 \"$infile\" \"$2/RGB5A3_mips_${infilename}.TXTR\""
            "$EXEC" encode -y -t RGB5A3 -m 0 "$infile" "$2/RGB5A3_mips_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t RGB5A3 -m 1 \"$infile\" \"$2/RGB5A3_${infilename}.TXTR\""
            "$EXEC" encode -y -t RGB5A3 -m 1 "$infile" "$2/RGB5A3_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t RGB5A3 -m 0 \"$infile\" \"$2/RGB5A3_mips_${infilename}.TXTR\""
            "$EXEC" encode -y -t RGB5A3 -m 0 "$infile" "$2/RGB5A3_mips_${infilename}.TXTR"
            test-end $?
            
            # RGBA8
            echo "Testing RGBA8..."
            
            test-start "\"$EXEC\" encode -y -t RGBA8 -m 1 \"$infile\" \"$2/RGBA8_${infilename}.TXTR\""
            "$EXEC" encode -y -t RGBA8 -m 1 "$infile" "$2/RGBA8_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t RGBA8 -m 0 \"$infile\" \"$2/RGBA8_mips_${infilename}.TXTR\""
            "$EXEC" encode -y -t RGBA8 -m 0 "$infile" "$2/RGBA8_mips_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t RGBA8 -m 1 \"$infile\" \"$2/RGBA8_${infilename}.TXTR\""
            "$EXEC" encode -y -t RGBA8 -m 1 "$infile" "$2/RGBA8_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t RGBA8 -m 0 \"$infile\" \"$2/RGBA8_mips_${infilename}.TXTR\""
            "$EXEC" encode -y -t RGBA8 -m 0 "$infile" "$2/RGBA8_mips_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t RGBA8 -m 1 \"$infile\" \"$2/RGBA8_${infilename}.TXTR\""
            "$EXEC" encode -y -t RGBA8 -m 1 "$infile" "$2/RGBA8_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t RGBA8 -m 0 \"$infile\" \"$2/RGBA8_mips_${infilename}.TXTR\""
            "$EXEC" encode -y -t RGBA8 -m 0 "$infile" "$2/RGBA8_mips_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t RGBA8 -m 1 \"$infile\" \"$2/RGBA8_${infilename}.TXTR\""
            "$EXEC" encode -y -t RGBA8 -m 1 "$infile" "$2/RGBA8_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t RGBA8 -m 0 \"$infile\" \"$2/RGBA8_mips_${infilename}.TXTR\""
            "$EXEC" encode -y -t RGBA8 -m 0 "$infile" "$2/RGBA8_mips_${infilename}.TXTR"
            test-end $?
        fi
        
        if [ "$3" == "all" ] || [ "$3" == "compressed" ] ; then
            # CMP
            echo "Testing CMP..."
            
            test-start "\"$EXEC\" encode -y -t CMP --squishrangefit -m 1 \"$infile\" \"$2/CMP_RANGEFIT_${infilename}.TXTR\""
            "$EXEC" encode -y -t CMP --squishrangefit -m 1 "$infile" "$2/CMP_RANGEFIT_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t CMP --squishrangefit -m 0 \"$infile\" \"$2/CMP_RANGEFIT_mips_${infilename}.TXTR\""
            "$EXEC" encode -y -t CMP --squishrangefit -m 0 "$infile" "$2/CMP_RANGEFIT_mips_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t CMP --squishclusterfit -m 1 \"$infile\" \"$2/CMP_CLUSTERFIT_${infilename}.TXTR\""
            "$EXEC" encode -y -t CMP --squishclusterfit -m 1 "$infile" "$2/CMP_CLUSTERFIT_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t CMP --squishclusterfit --squishalphaweight -m 1 \"$infile\" \"$2/CMP_CLUSTERFIT_WEIGHTALPHA_${infilename}.TXTR\""
            "$EXEC" encode -y -t CMP --squishclusterfit --squishalphaweight -m 1 "$infile" "$2/CMP_CLUSTERFIT_WEIGHTALPHA_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t CMP --squishclusterfit -m 0 \"$infile\" \"$2/CMP_CLUSTERFIT_mips_${infilename}.TXTR\""
            "$EXEC" encode -y -t CMP --squishclusterfit -m 0 "$infile" "$2/CMP_CLUSTERFIT_mips_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t CMP --squishclusterfit --squishalphaweight -m 0 \"$infile\" \"$2/CMP_CLUSTERFIT_WEIGHTALPHA_mips_${infilename}.TXTR\""
            "$EXEC" encode -y -t CMP --squishclusterfit --squishalphaweight -m 0 "$infile" "$2/CMP_CLUSTERFIT_WEIGHTALPHA_mips_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t CMP --squishiterclusterfit -m 1 \"$infile\" \"$2/CMP_ITERCLUSTERFIT_${infilename}.TXTR\""
            "$EXEC" encode -y -t CMP --squishiterclusterfit -m 1 "$infile" "$2/CMP_ITERCLUSTERFIT_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t CMP --squishiterclusterfit --squishalphaweight -m 1 \"$infile\" \"$2/CMP_ITERCLUSTERFIT_WEIGHTALPHA_${infilename}.TXTR\""
            "$EXEC" encode -y -t CMP --squishiterclusterfit --squishalphaweight -m 1 "$infile" "$2/CMP_ITERCLUSTERFIT_WEIGHTALPHA_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t CMP --squishiterclusterfit -m 0 \"$infile\" \"$2/CMP_ITERCLUSTERFIT_mips_${infilename}.TXTR\""
            "$EXEC" encode -y -t CMP --squishiterclusterfit -m 0 "$infile" "$2/CMP_ITERCLUSTERFIT_mips_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t CMP --squishiterclusterfit --squishalphaweight -m 0 \"$infile\" \"$2/CMP_ITERCLUSTERFIT_WEIGHTALPHA_mips_${infilename}.TXTR\""
            "$EXEC" encode -y -t CMP --squishiterclusterfit --squishalphaweight -m 0 "$infile" "$2/CMP_ITERCLUSTERFIT_WEIGHTALPHA_mips_${infilename}.TXTR"
            test-end $?
        fi
        
        if [ "$3" == "all" ] || [ "$3" == "indexed" ] ; then
            # CI4
            echo "Testing CI4..."
            
            test-start "\"$EXEC\" encode -y -t CI4 -p IA8 -m 1 \"$infile\" \"$2/CI4_IA8_${infilename}.TXTR\""
            "$EXEC" encode -y -t CI4 -p IA8 -m 1 "$infile" "$2/CI4_IA8_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t CI4 -p R5G6B5 -m 1 \"$infile\" \"$2/CI4_R5G6B5_${infilename}.TXTR\""
            "$EXEC" encode -y -t CI4 -p R5G6B5 -m 1 "$infile" "$2/CI4_R5G6B5_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t CI4 -p RGB5A3 -m 1 \"$infile\" \"$2/CI4_RGB5A3_${infilename}.TXTR\""
            "$EXEC" encode -y -t CI4 -p RGB5A3 -m 1 "$infile" "$2/CI4_RGB5A3_${infilename}.TXTR"
            test-end $?
            
            # CI8
            echo "Testing CI8..."
            
            test-start "\"$EXEC\" encode -y -t CI8 -p IA8 -m 1 \"$infile\" \"$2/CI8_IA8_${infilename}.TXTR\""
            "$EXEC" encode -y -t CI8 -p IA8 -m 1 "$infile" "$2/CI8_IA8_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t CI8 -p R5G6B5 -m 1 \"$infile\" \"$2/CI8_R5G6B5_${infilename}.TXTR\""
            "$EXEC" encode -y -t CI8 -p R5G6B5 -m 1 "$infile" "$2/CI8_R5G6B5_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t CI8 -p RGB5A3 -m 1 \"$infile\" \"$2/CI8_RGB5A3_${infilename}.TXTR\""
            "$EXEC" encode -y -t CI8 -p RGB5A3 -m 1 "$infile" "$2/CI8_RGB5A3_${infilename}.TXTR"
            test-end $?
            
            # CI14X2
            echo "Testing CI14X2..."
            
            test-start "\"$EXEC\" encode -y -t CI14X2 -p IA8 -m 1 \"$infile\" \"$2/CI14X2_IA8_${infilename}.TXTR\""
            "$EXEC" encode -y -t CI14X2 -p IA8 -m 1 "$infile" "$2/CI14X2_IA8_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t CI14X2 -p R5G6B5 -m 1 \"$infile\" \"$2/CI14X2_R5G6B5_${infilename}.TXTR\""
            "$EXEC" encode -y -t CI14X2 -p R5G6B5 -m 1 "$infile" "$2/CI14X2_R5G6B5_${infilename}.TXTR"
            test-end $?
            
            test-start "\"$EXEC\" encode -y -t CI14X2 -p RGB5A3 -m 1 \"$infile\" \"$2/CI14X2_RGB5A3_${infilename}.TXTR\""
            "$EXEC" encode -y -t CI14X2 -p RGB5A3 -m 1 "$infile" "$2/CI14X2_RGB5A3_${infilename}.TXTR"
            test-end $?
        fi
        
        if [ "$3" == "all" ] || [ "$3" == "dither" ] ; then
            # Dither (CI8)
            echo "Testing Dither (CI8)..."
            
            test-start "\"$EXEC\" encode -y -t CI8 -p RGB5A3 -m 1 --dithertype THRESHOLD \"$infile\" \"$2/CI8_RGB5A3_THRESHOLD_${infilename}.TXTR\""
            "$EXEC" encode -y -t CI8 -p RGB5A3 -m 1 --dithertype THRESHOLD "$infile" "$2/CI8_RGB5A3_THRESHOLD_${infilename}.TXTR"
            test-end $?

            test-start "\"$EXEC\" encode -y -t CI8 -p RGB5A3 -m 1 --dithertype FLOYD_STEINBERG \"$infile\" \"$2/CI8_RGB5A3_FLOYD_STEINBERG_${infilename}.TXTR\""
            "$EXEC" encode -y -t CI8 -p RGB5A3 -m 1 --dithertype FLOYD_STEINBERG "$infile" "$2/CI8_RGB5A3_FLOYD_STEINBERG_${infilename}.TXTR"
            test-end $?

            test-start "\"$EXEC\" encode -y -t CI8 -p RGB5A3 -m 1 --dithertype ATKINSON \"$infile\" \"$2/CI8_RGB5A3_ATKINSON_${infilename}.TXTR\""
            "$EXEC" encode -y -t CI8 -p RGB5A3 -m 1 --dithertype ATKINSON "$infile" "$2/CI8_RGB5A3_ATKINSON_${infilename}.TXTR"
            test-end $?

            test-start "\"$EXEC\" encode -y -t CI8 -p RGB5A3 -m 1 --dithertype JARVIS_JUDICE_NINKE \"$infile\" \"$2/CI8_RGB5A3_JARVIS_JUDICE_NINKE_${infilename}.TXTR\""
            "$EXEC" encode -y -t CI8 -p RGB5A3 -m 1 --dithertype JARVIS_JUDICE_NINKE "$infile" "$2/CI8_RGB5A3_JARVIS_JUDICE_NINKE_${infilename}.TXTR"
            test-end $?

            test-start "\"$EXEC\" encode -y -t CI8 -p RGB5A3 -m 1 --dithertype STUCKI \"$infile\" \"$2/CI8_RGB5A3_STUCKI_${infilename}.TXTR\""
            "$EXEC" encode -y -t CI8 -p RGB5A3 -m 1 --dithertype STUCKI "$infile" "$2/CI8_RGB5A3_STUCKI_${infilename}.TXTR"
            test-end $?

            test-start "\"$EXEC\" encode -y -t CI8 -p RGB5A3 -m 1 --dithertype BURKES \"$infile\" \"$2/CI8_RGB5A3_BURKES_${infilename}.TXTR\""
            "$EXEC" encode -y -t CI8 -p RGB5A3 -m 1 --dithertype BURKES "$infile" "$2/CI8_RGB5A3_BURKES_${infilename}.TXTR"
            test-end $?

            test-start "\"$EXEC\" encode -y -t CI8 -p RGB5A3 -m 1 --dithertype TWO_ROW_SIERRA \"$infile\" \"$2/CI8_RGB5A3_TWO_ROW_SIERRA_${infilename}.TXTR\""
            "$EXEC" encode -y -t CI8 -p RGB5A3 -m 1 --dithertype TWO_ROW_SIERRA "$infile" "$2/CI8_RGB5A3_TWO_ROW_SIERRA_${infilename}.TXTR"
            test-end $?

            test-start "\"$EXEC\" encode -y -t CI8 -p RGB5A3 -m 1 --dithertype SIERRA \"$infile\" \"$2/CI8_RGB5A3_SIERRA_${infilename}.TXTR\""
            "$EXEC" encode -y -t CI8 -p RGB5A3 -m 1 --dithertype SIERRA "$infile" "$2/CI8_RGB5A3_SIERRA_${infilename}.TXTR"
            test-end $?

            test-start "\"$EXEC\" encode -y -t CI8 -p RGB5A3 -m 1 --dithertype SIERRA_LITE \"$infile\" \"$2/CI8_RGB5A3_SIERRA_LITE_${infilename}.TXTR\""
            "$EXEC" encode -y -t CI8 -p RGB5A3 -m 1 --dithertype SIERRA_LITE "$infile" "$2/CI8_RGB5A3_SIERRA_LITE_${infilename}.TXTR"
            test-end $?
        fi
        
        # TODO: Mipmap tests
    fi
}

test-run "$dp0/.local/rgb8bit/nightshot_iso_100_small.tga" "$dp0/.local/export" "$testtorun"
test-run "$dp0/.local/allcolors/allcolors_alpha.tga" "$dp0/.local/export" "$testtorun"
test-run "$dp0/.local/other/32aeaaae.tga" "$dp0/.local/export" "$testtorun"
test-run "$dp0/.local/other/640x446-London_Telephone.tga" "$dp0/.local/export" "$testtorun"
test-run "$dp0/.local/other/colorsalpha.tga" "$dp0/.local/export" "$testtorun"

"$dp0/test_decode.sh" "$dp0/.local/export" "$dp0/.local/export/out"
