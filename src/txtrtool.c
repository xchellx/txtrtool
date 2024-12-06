/*
 * MIT License
 * 
 * Copyright (c) 2024 Yonder
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <txtrtool.h>

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <signal.h>
#include <float.h>

#include <stdext.h>
#include <optparse99.h>
#include <tga.h>
#include <txtr.h>

#if (defined(TXTRTOOL_INCLUDE_DECODE) || defined(TXTRTOOL_INCLUDE_MISC)) && !defined(TXTR_INCLUDE_DECODE)
#error TXTR decoding capabilities are required
#endif

#if defined(TXTRTOOL_INCLUDE_ENCODE) && !defined(TXTR_INCLUDE_ENCODE)
#error TXTR encoding capabilities are required
#endif

#if defined(TXTRTOOL_INCLUDE_DECODE) && !defined(TGA_INCLUDE_DECODE)
#error TGA decoding capabilities are required
#endif

#if defined(TXTRTOOL_INCLUDE_ENCODE) && !defined(TGA_INCLUDE_ENCODE)
#error TGA encoding capabilities are required
#endif

// TODO: handle premultiplied alpha
// TODO: add better support for TGA format (convert to and from 32bit BGRA, support footer, etc.)
// TODO: add option to intake TGA's palette (and convert it to BGRA) which will make the quantizer not run and instead
// all colors will be collected for the palette instead.

typedef enum TTMode {
    TTM_SZ_MIN = SIG_ATOMIC_MIN,
    TTM_NONE = 0,
    TTM_DECODE,
    TTM_ENCODE,
    TTM_PRINT,
    TTM_SZ_MAX = SIG_ATOMIC_MAX
} PACK TTMode_t;

// A safe single thread volatile variable modified by only a single instruction so therefore it is re-entrant as well.
static volatile sig_atomic_t ttMode = TTM_NONE;

// Subcommand options types
#ifdef TXTRTOOL_INCLUDE_DECODE
typedef struct TTDecodeOptions {
    int noOutp;
    int noErrp;
    int yes;
    int no;
    int mipmaps;
    char *prefix;
    char *suffix;
} TTDecodeOptions_t;
#endif

#ifdef TXTRTOOL_INCLUDE_ENCODE
typedef struct TTEncodeOptions {
    int noOutp;
    int noErrp;
    int yes;
    int no;
    char *texFmt;
    TXTRFormat_t texFmtDec;
    char *palFmt;
    TXTRPaletteFormat_t palFmtDec;
    uint8_t mipLimit;
    uint16_t widthLimit;
    uint16_t heightLimit;
    char *avgType;
    GXAvgType_t avgTypeDec;
    char *stbirEdge;
    stbir_edge stbirEdgeDec;
    char *stbirFilter;
    stbir_filter stbirFilterDec;
    char *ditherType; 
    GXDitherType_t ditherTypeDec;
    float squishMetric[3];
    float *squishMetricPtr;
    size_t squishMetricSz;
    bool squishMetricValid;
    int squishFlags;
    int squishAlphaWeight;
    int squishClusterFit;
    int squishRangeFit;
    int squishIterClusterFit;
} TTEncodeOptions_t;
#endif

#ifdef TXTRTOOL_INCLUDE_MISC
typedef struct TTPrintOptions {
    int noOutp;
    int noErrp;
    int json;
} TTPrintOptions_t;
#endif

// TXTR_TTF_* <-> string and TOSTR(*) where * is of TXTR_TTF_* all without an intermediate type.
#define __txtrtool_fmtstrs_egrp__(e) e = TXTR_TTF_ ## e
enum __txtrtool_fmtstrs__ {
    __txtrtool_fmtstrs_egrp__(I4),
    __txtrtool_fmtstrs_egrp__(I8),
    __txtrtool_fmtstrs_egrp__(IA4),
    __txtrtool_fmtstrs_egrp__(IA8),
    __txtrtool_fmtstrs_egrp__(CI4),
    __txtrtool_fmtstrs_egrp__(CI8),
    __txtrtool_fmtstrs_egrp__(CI14X2),
    __txtrtool_fmtstrs_egrp__(R5G6B5),
    __txtrtool_fmtstrs_egrp__(RGB5A3),
    __txtrtool_fmtstrs_egrp__(RGBA8),
    __txtrtool_fmtstrs_egrp__(CMP)
};

#define __txtrtool_Tex2Str_agrp__(e) [TXTR_TTF_ ## e ] = TOSTR(e)
static char *_Tex2Str[12] = {
    __txtrtool_Tex2Str_agrp__(I4),
    __txtrtool_Tex2Str_agrp__(I8),
    __txtrtool_Tex2Str_agrp__(IA4),
    __txtrtool_Tex2Str_agrp__(IA8),
    __txtrtool_Tex2Str_agrp__(CI4),
    __txtrtool_Tex2Str_agrp__(CI8),
    __txtrtool_Tex2Str_agrp__(CI14X2),
    __txtrtool_Tex2Str_agrp__(R5G6B5),
    __txtrtool_Tex2Str_agrp__(RGB5A3),
    __txtrtool_Tex2Str_agrp__(RGBA8),
    __txtrtool_Tex2Str_agrp__(CMP)
};

FORCE_INLINE char *Tex2Str(TXTRFormat_t t) {
    return t >= TXTR_TTF_I4 && t <= TXTR_TTF_CMP ? _Tex2Str[t] : "INVALID";
}

FORCE_INLINE TXTRFormat_t Str2Tex(char *str) {
    for (TXTRFormat_t t = TXTR_TTF_I4; t <= TXTR_TTF_CMP; t++)
        if (!strcmp(str, Tex2Str(t)))
            return t;
    return TXTR_TTF_INVALID;
}

#define TexList(d) \
    TOSTR(I4) d \
    TOSTR(I8) d \
    TOSTR(IA4) d \
    TOSTR(IA8) d \
    TOSTR(CI4) d \
    TOSTR(CI8) d \
    TOSTR(CI14X2) d \
    TOSTR(R5G6B5) d \
    TOSTR(RGB5A3) d \
    TOSTR(RGBA8) d \
    TOSTR(CMP)

// TXTR_TPF_* <-> string and TOSTR(*) where * is of TXTR_TPF_* all without an intermediate type.
#define __txtrtool_Pal2Str_agrp__(e) [TXTR_TPF_ ## e ] = TOSTR(e)
static char *_Pal2Str[4] = {
    __txtrtool_Pal2Str_agrp__(IA8),
    __txtrtool_Pal2Str_agrp__(R5G6B5),
    __txtrtool_Pal2Str_agrp__(RGB5A3)
};

FORCE_INLINE char *Pal2Str(TXTRPaletteFormat_t p) {
    return p >= TXTR_TPF_IA8 && p <= TXTR_TPF_RGB5A3 ? _Pal2Str[p] : "INVALID";
}

FORCE_INLINE TXTRPaletteFormat_t Str2Pal(char *str) {
    for (TXTRPaletteFormat_t p = TXTR_TPF_IA8; p <= TXTR_TPF_RGB5A3; p++)
        if (!strcmp(str, Pal2Str(p)))
            return p;
    return TXTR_TPF_INVALID;
}

#define PalList(d) \
    TOSTR(IA8) d \
    TOSTR(R5G6B5) d \
    TOSTR(RGB5A3)

// GX_AT_* <-> string and TOSTR(*) where * is of GX_AT_* all without an intermediate type.
#define __txtrtool_avgtypstrs_egrp__(e) e = GX_AT_ ## e
enum __txtrtool_avgtypstrs__ {
    __txtrtool_avgtypstrs_egrp__(AVERAGE),
    __txtrtool_avgtypstrs_egrp__(SQUARED),
    __txtrtool_avgtypstrs_egrp__(W3C),
    __txtrtool_avgtypstrs_egrp__(SRGB)
};

#define __txtrtool_AvgTyp2Str_agrp__(e) [GX_AT_ ## e ] = TOSTR(e)
static char *_AvgTyp2Str[5] = {
    __txtrtool_AvgTyp2Str_agrp__(AVERAGE),
    __txtrtool_AvgTyp2Str_agrp__(SQUARED),
    __txtrtool_AvgTyp2Str_agrp__(W3C),
    __txtrtool_AvgTyp2Str_agrp__(SRGB)
};

FORCE_INLINE char *AvgTyp2Str(GXAvgType_t at) {
    return at >= GX_AT_MIN && at <= GX_AT_MAX ? _AvgTyp2Str[at] : "INVALID";
}

FORCE_INLINE GXAvgType_t Str2AvgTyp(char *str) {
    for (GXAvgType_t at = GX_AT_MIN; at <= GX_AT_MAX; at++)
        if (!strcmp(str, AvgTyp2Str(at)))
            return at;
    return GX_AT_INVALID;
}

#define AvgTypList(d) \
    TOSTR(AVERAGE) d \
    TOSTR(SQUARED) d \
    TOSTR(W3C) d \
    TOSTR(SRGB)

// STBIR_EDGE_* <-> string and TOSTR(*) where * is of STBIR_EDGE_* all without an intermediate type.
#define __txtrtool_edgestrs_egrp__(e) e = STBIR_EDGE_ ## e
enum __txtrtool_edgestrs__ {
    __txtrtool_edgestrs_egrp__(CLAMP),
    __txtrtool_edgestrs_egrp__(REFLECT),
    __txtrtool_edgestrs_egrp__(WRAP),
    __txtrtool_edgestrs_egrp__(ZERO)
};

#define __txtrtool_Edge2Str_agrp__(e) [STBIR_EDGE_ ## e ] = TOSTR(e)
static char *_Edge2Str[5] = {
    __txtrtool_Edge2Str_agrp__(CLAMP),
    __txtrtool_Edge2Str_agrp__(REFLECT),
    __txtrtool_Edge2Str_agrp__(WRAP),
    __txtrtool_Edge2Str_agrp__(ZERO)
};

FORCE_INLINE char *Edge2Str(stbir_edge e) {
    return e >= STBIR_EDGE_CLAMP && e <= STBIR_EDGE_ZERO ? _Edge2Str[e] : "INVALID";
}

FORCE_INLINE stbir_edge Str2Edge(char *str) {
    for (stbir_edge e = STBIR_EDGE_CLAMP; e <= STBIR_EDGE_ZERO; e++)
        if (!strcmp(str, Edge2Str(e)))
            return e;
    return -1;
}

#define EdgeList(d) \
    TOSTR(CLAMP) d \
    TOSTR(REFLECT) d \
    TOSTR(WRAP) d \
    TOSTR(ZERO)

// STBIR_FILTER_* <-> string and TOSTR(*) where * is of STBIR_FILTER_* all without an intermediate type.
#define __txtrtool_filterstrs_egrp__(e) e = STBIR_FILTER_ ## e
enum __txtrtool_filterstrs__ {
    __txtrtool_filterstrs_egrp__(DEFAULT),
    __txtrtool_filterstrs_egrp__(BOX),
    __txtrtool_filterstrs_egrp__(TRIANGLE),
    __txtrtool_filterstrs_egrp__(CUBICBSPLINE),
    __txtrtool_filterstrs_egrp__(CATMULLROM),
    __txtrtool_filterstrs_egrp__(MITCHELL),
    __txtrtool_filterstrs_egrp__(POINT_SAMPLE)
};

#define __txtrtool_Filter2Str_agrp__(e) [STBIR_FILTER_ ## e ] = TOSTR(e)
static char *_Filter2Str[8] = {
    __txtrtool_Filter2Str_agrp__(DEFAULT),
    __txtrtool_Filter2Str_agrp__(BOX),
    __txtrtool_Filter2Str_agrp__(TRIANGLE),
    __txtrtool_Filter2Str_agrp__(CUBICBSPLINE),
    __txtrtool_Filter2Str_agrp__(CATMULLROM),
    __txtrtool_Filter2Str_agrp__(MITCHELL),
    __txtrtool_Filter2Str_agrp__(POINT_SAMPLE)
};

FORCE_INLINE char *Filter2Str(stbir_filter f) {
    return f >= STBIR_FILTER_DEFAULT && f <= STBIR_FILTER_POINT_SAMPLE ? _Filter2Str[f] : "INVALID";
}

FORCE_INLINE stbir_filter Str2Filter(char *str) {
    for (stbir_filter f = STBIR_FILTER_DEFAULT; f <= STBIR_FILTER_POINT_SAMPLE; f++)
        if (!strcmp(str, Filter2Str(f)))
            return f;
    return -1;
}

#define FilterList(d) \
    TOSTR(DEFAULT) d \
    TOSTR(BOX) d \
    TOSTR(TRIANGLE) d \
    TOSTR(CUBICBSPLINE) d \
    TOSTR(CATMULLROM) d \
    TOSTR(MITCHELL) d \
    TOSTR(POINT_SAMPLE)

// GX_DT_* <-> string and TOSTR(*) where * is of GX_DT_* all without an intermediate type.
#define __txtrtool_dithertypestrs_egrp__(e) e = GX_DT_ ## e
enum __txtrtool_dithertypestrs__ {
    __txtrtool_dithertypestrs_egrp__(THRESHOLD),
    __txtrtool_dithertypestrs_egrp__(FLOYD_STEINBERG),
    __txtrtool_dithertypestrs_egrp__(ATKINSON),
    __txtrtool_dithertypestrs_egrp__(JARVIS_JUDICE_NINKE),
    __txtrtool_dithertypestrs_egrp__(STUCKI),
    __txtrtool_dithertypestrs_egrp__(BURKES),
    __txtrtool_dithertypestrs_egrp__(TWO_ROW_SIERRA),
    __txtrtool_dithertypestrs_egrp__(SIERRA),
    __txtrtool_dithertypestrs_egrp__(SIERRA_LITE)
};

#define __txtrtool_DitherType2Str_agrp__(e) [GX_DT_ ## e ] = TOSTR(e)
static char *_DitherType2Str[9] = {
    __txtrtool_DitherType2Str_agrp__(THRESHOLD),
    __txtrtool_DitherType2Str_agrp__(FLOYD_STEINBERG),
    __txtrtool_DitherType2Str_agrp__(ATKINSON),
    __txtrtool_DitherType2Str_agrp__(JARVIS_JUDICE_NINKE),
    __txtrtool_DitherType2Str_agrp__(STUCKI),
    __txtrtool_DitherType2Str_agrp__(BURKES),
    __txtrtool_DitherType2Str_agrp__(TWO_ROW_SIERRA),
    __txtrtool_DitherType2Str_agrp__(SIERRA),
    __txtrtool_DitherType2Str_agrp__(SIERRA_LITE)
};

FORCE_INLINE char *DitherType2Str(GXDitherType_t at) {
    return at >= GX_DT_MIN && at <= GX_DT_MAX ? _DitherType2Str[at] : "INVALID";
}

FORCE_INLINE GXDitherType_t Str2DitherType(char *str) {
    for (GXDitherType_t at = GX_DT_MIN; at <= GX_DT_MAX; at++)
        if (!strcmp(str, DitherType2Str(at)))
            return at;
    return GX_DT_INVALID;
}

#define DitherTypeList(d) \
    TOSTR(THRESHOLD) d \
    TOSTR(FLOYD_STEINBERG) d \
    TOSTR(ATKINSON) d \
    TOSTR(JARVIS_JUDICE_NINKE) \
    TOSTR(STUCKI) \
    TOSTR(BURKES) \
    TOSTR(TWO_ROW_SIERRA) \
    TOSTR(SIERRA) \
    TOSTR(SIERRA_LITE)

// Read file tasks
#if defined(TXTRTOOL_INCLUDE_DECODE) || defined(TXTRTOOL_INCLUDE_ENCODE) || defined(TXTRTOOL_INCLUDE_MISC)
static TTStatus_t readFile(bool noErrp, char *input, size_t *outDataSz, uint8_t **outData) {
    FILE *file;
    if (cfopen(input, "rb", &file)) {
        sleprintf(noErrp, "ERROR: Failed to open input file \"%s\": %s\n", input, strerror(errno));
        return TTS_IOERROR;
    }
    
    size_t fileSz = 0;
    if (cfsize(&fileSz, file)) {
        sleprintf(noErrp, "ERROR: Failed to get size of input file \"%s\": %s\n", input, strerror(errno));
        if (cfclose(file))
            sleprintf(noErrp, "WARN: Failed to close input file \"%s\": %s\n", input, strerror(errno));
        return TTS_IOERROR;
    } else if (!fileSz) {
        sleprintf(noErrp, "ERROR: Input file \"%s\" is empty\n", input);
        if (cfclose(file))
            sleprintf(noErrp, "WARN: Failed to close input file \"%s\": %s\n", input, strerror(errno));
        return TTS_ARGERROR;
    }
    
    uint8_t *fileData = malloc(fileSz);
    if (!fileData) {
        sleprintf(noErrp, "ERROR: Failed to allocate memory for TXTR data\n");
        if (cfclose(file))
            sleprintf(noErrp, "WARN: Failed to close input file \"%s\": %s\n", input, strerror(errno));
        return TTS_MEMERROR;
    }
    if (cfread(fileData, sizeof(uint8_t), fileSz, file)) {
        sleprintf(noErrp, "ERROR: Failed to read input file \"%s\": %s\n", input, strerror(errno));
        free(fileData);
        return TTS_IOERROR;
    }
    if (cfclose(file))
        sleprintf(noErrp, "WARN: Failed to close input file \"%s\": %s\n", input, strerror(errno));
    
    *outData = fileData;
    *outDataSz = fileSz;
    return TTS_SUCCESS;
}
#endif

#if defined(TXTRTOOL_INCLUDE_DECODE) || defined(TXTRTOOL_INCLUDE_MISC)
static TTStatus_t readTXTR(bool noErrp, char *input, TXTR_t *txtr) {
    uint8_t *txtrData = NULL;
    size_t txtrDataSz = 0;
    TTStatus_t rfe = readFile(noErrp, input, &txtrDataSz, &txtrData);
    if (rfe)
        return rfe;
    
    TXTRReadError_t tre = TXTR_Read(txtr, txtrDataSz, txtrData);
    if (tre) {
        sleprintf(noErrp, "ERROR: Failed to read TXTR data: %s\n", TXTRReadError_ToStr(tre));
        free(txtrData);
        
        switch (tre) {
            case TXTR_RE_INVLDTEXFMT:
            case TXTR_RE_INVLDTEXWIDTH:
            case TXTR_RE_INVLDTEXHEIGHT:
            case TXTR_RE_INVLDMIPCNT:
            case TXTR_RE_INVLDPALFMT:
            case TXTR_RE_INVLDPALWIDTH:
            case TXTR_RE_INVLDPALHEIGHT:
            case TXTR_RE_INVLDPALSZ:
                return TTS_FMTERROR;
            case TXTR_RE_MEMFAILPAL:
            case TXTR_RE_MEMFAILMIPS:
                return TTS_MEMERROR;
            case TXTR_RE_INVLDPARAMS:
                return TTS_ARGERROR;
            default:
                return TTS_PROGERROR;
        }
    }
    free(txtrData);
    
    return TTS_SUCCESS;
}
#endif

#ifdef TXTRTOOL_INCLUDE_ENCODE
static TTStatus_t readTGA(bool noErrp, char *input, TGA_t *tga) {
    uint8_t *tgaData = NULL;
    size_t tgaDataSz = 0;
    TTStatus_t rfe = readFile(noErrp, input, &tgaDataSz, &tgaData);
    if (rfe)
        return rfe;
    
    TGAReadError_t tre = TGA_Read(tga, tgaDataSz, tgaData);
    if (tre) {
        sleprintf(noErrp, "ERROR: Failed to read TGA data: %s\n", TGAReadError_ToStr(tre));
        free(tgaData);
        
        switch (tre) {
            case TGA_RE_CLRMAPPRESENT:
            case TGA_RE_NOTACOLORTGA:
            case TGA_RE_INVLDXORIGIN:
            case TGA_RE_INVLDYORIGIN:
            case TGA_RE_INVLDWIDTH:
            case TGA_RE_INVLDHEIGHT:
            case TGA_RE_INVLDPXLDEP:
            case TGA_RE_INVLDALPHBITSZ:
                return TTS_FMTERROR;
            case TGA_RE_MEMFAILID:
            case TGA_RE_MEMFAILDATA:
                return TTS_MEMERROR;
            case TGA_RE_INVLDPARAMS:
                return TTS_ARGERROR;
            default:
                return TTS_PROGERROR;
        }
    }
    free(tgaData);
    
    // TODO: Remove this when support is added
    if (tga->isNewFmt)
        sleprintf(noErrp, "WARN: Input file \"%s\" is of the \"New TGA Format\". Information in the file's footer will"
            " be ignored. This may produce incorrect results.\n", input);
    
    return TTS_SUCCESS;
}
#endif

// Write file tasks
#if defined(TXTRTOOL_INCLUDE_DECODE) || defined(TXTRTOOL_INCLUDE_ENCODE)
static TTStatus_t writeFile(bool noOutp, bool noErrp, bool yes, bool no, char *output, size_t fileDataSz,
uint8_t *fileData) {
    bool outputIsDir = false;
    bool outputExists = !cfexists(output, &outputIsDir);
    if (outputExists) {
        if (outputIsDir) {
            sleprintf(noErrp, "ERROR: Output file \"%s\" is a directory; cannot overwrite\n", output);
            return TTS_PROGERROR;
        } else {
            sloprintf(no || yes || noOutp, "Do you want to overwrite output file \"%s\"? (y,Y/ANY) ", output);
            if (no || askYN(yes, noOutp)) {
                sleprintf(noErrp, "ERROR: Not overwriting output file \"%s\"\n", output);
                return TTS_PROGERROR;
            }
        }
    }
    
    FILE *file;
    if (cfopen(output, "wb", &file)) {
        sleprintf(noErrp, "ERROR: Failed to open output file \"%s\": %s\n", output, strerror(errno));
        return TTS_IOERROR;
    }
    
    if (cfwrite(fileData, sizeof(uint8_t), fileDataSz, file)) {
        sleprintf(noErrp, "ERROR: Failed to write output file \"%s\": %s\n", output, strerror(errno));
        return TTS_IOERROR;
    }
    if (cfclose(file))
        sleprintf(noErrp, "WARN: Failed to close output file \"%s\": %s\n", output, strerror(errno));
    
    return TTS_SUCCESS;
}
#endif

#ifdef TXTRTOOL_INCLUDE_ENCODE
static TTStatus_t writeTXTR(bool noOutp, bool noErrp, bool yes, bool no, char *output, TXTR_t *txtr,
TXTRRawMipmap_t mips[11]) {
    size_t txtrDataSz = 0;
    uint8_t *txtrData = NULL;
    TXTRWriteError_t twe = TXTR_Write(txtr, mips, &txtrDataSz, &txtrData);
    if (twe) {
        sleprintf(noErrp, "ERROR: Failed to write TXTR data: %s\n", TXTRWriteError_ToStr(twe));
        
        switch (twe) {
            case TXTR_WE_INVLDTEXFMT:
            case TXTR_WE_INVLDTEXWIDTH:
            case TXTR_WE_INVLDTEXHEIGHT:
            case TXTR_WE_INVLDMIPCNT:
            case TXTR_WE_INVLDPALFMT:
            case TXTR_WE_INVLDPALWIDTH:
            case TXTR_WE_INVLDPALHEIGHT:
            case TXTR_WE_INVLDPALSZ:
                return TTS_FMTERROR;
            case TXTR_WE_INVLDTEXPAL:
            case TXTR_WE_INVLDTEXMIPS:
            case TXTR_WE_MEMFAILMIPS:
                return TTS_MEMERROR;
            case TXTR_WE_INVLDPARAMS:
                return TTS_ARGERROR;
            case TXTR_WE_INTERRUPTED:
            default:
                return TTS_PROGERROR;
        }
    }
    
    TTStatus_t fwe = writeFile(noOutp, noErrp, yes, no, output, txtrDataSz, txtrData);
    if (fwe) {
        free(txtrData);
        return fwe;
    }
    free(txtrData);
    
    return TTS_SUCCESS;
}
#endif

#ifdef TXTRTOOL_INCLUDE_DECODE
static TTStatus_t writeTGA(bool noOutp, bool noErrp, bool yes, bool no, char *output, char *id, TXTRMipmap_t *mip) {
    TGA_t tga = {
        .hdr = {
            .idLength = strlen(id),
            .imageType = TGA_IMT_COLOR,
            .colorMapType = TGA_CMT_NOCOLORMAP,
            .colorMapSpec = {
                .firstEntryIndex = 0,
                .colorMapLength = 0,
                .colorMapEntrySize = 0
            },
            .imageSpec = {
                .xOrigin = 0,
                .yOrigin = 0,
                .width = mip->width,
                .height = mip->height,
                .pixelDepth = 32,
                .imageDesc = TGAImageDescriptor(8, false, false, 0)
            }
        },
        .id = id,
        .dataSz = mip->size,
        .data = mip->data,
        .isNewFmt = true,
        .ftr = {
            .extAreaOffs = 0,
            .devAreaOffs = 0,
            .signature = TGA_FOOTERSIG
        }
    };
    
    size_t tgaDataSz = 0;
    uint8_t *tgaData = NULL;
    TGAWriteError_t twe = TGA_Write(&tga, &tgaDataSz, &tgaData);
    if (twe) {
        sleprintf(noErrp, "ERROR: Failed to write TGA data: %s\n", TGAWriteError_ToStr(twe));
        
        switch (twe) {
            case TGA_WE_MEMFAILDATA:
                return TTS_MEMERROR;
            case TGA_WE_CLRMAPPRESENT:
            case TGA_WE_NOTACOLORTGA:
            case TGA_WE_INVLDXORIGIN:
            case TGA_WE_INVLDYORIGIN:
            case TGA_WE_INVLDWIDTH:
            case TGA_WE_INVLDHEIGHT:
            case TGA_WE_INVLDPXLDEP:
            case TGA_WE_INVLDALPHBITSZ:
            case TGA_WE_INVLDPARAMS:
            case TGA_WE_INVLDDATA:
            case TGA_WE_INVLDID:
            case TGA_WE_INVLDSIGNATURE:
                return TTS_ARGERROR;
            default:
                return TTS_PROGERROR;
        }
    }
    
    TTStatus_t fwe = writeFile(noOutp, noErrp, yes, no, output, tgaDataSz, tgaData);
    if (fwe) {
        free(tgaData);
        return  fwe;
    }
    free(tgaData);
    
    return TTS_SUCCESS;
}
#endif

// Subcommand tasks
#ifdef TXTRTOOL_INCLUDE_DECODE
static TTStatus_t decode(TTDecodeOptions_t *opts, char *input, char *output) {
    char *mipFile = NULL, *mipFileEnd = NULL;
    bool outputIsDir = false;
    bool outputExists = !cfexists(output, &outputIsDir);
    if (opts->mipmaps) {
        if (outputExists && !outputIsDir) {
            sleprintf(opts->noErrp, "ERROR: Output directory \"%s\" must be a directory\n", output);
            return TTS_PROGERROR;
        } else if (!outputExists) {
            sloprintf(opts->no || opts->yes || opts->noOutp,
                "Output directory \"%s\" does not exist. Create it? (y,Y/ANY) ", output);
            if (opts->no || askYN(opts->yes, opts->noOutp)) {
                sleprintf(opts->noErrp, "ERROR: Not creating output directory \"%s\"\n", output);
                return TTS_PROGERROR;
            } else {
                if (cmkdir(output)) {
                    sleprintf(opts->noErrp, "ERROR: Failed to create output directory \"%s\": %s\n", output,
                        strerror(errno));
                    return TTS_IOERROR;
                }
            }
        }
        
        char *cfnPtr = NULL;
        char *cfn = cfilename(input, &cfnPtr);
        if (cfnPtr) {
            if (output[strlen(output) - 1] != '/' || output[strlen(output) - 1] != '\\')
                mipFile = csprintf_s("%s/%s%s%s00.tga", output, opts->prefix, cfn, opts->suffix);
            else
                mipFile = csprintf_s("%s%s%s%s00.tga", output, opts->prefix, cfn, opts->suffix);
            
            free(cfnPtr);
        } else {
            sleprintf(opts->noErrp, "ERROR: Failed to get file name from input file path \"%s\"\n", input);
            return TTS_MEMERROR;
        }
        
        if (!mipFile) {
            sleprintf(opts->noErrp, "ERROR: Failed to setup output file path from input file path \"%s\"\n", input);
            return TTS_MEMERROR;
        }
        
        mipFileEnd = cextnameraw(mipFile);
        if (!mipFileEnd)
            mipFileEnd = (mipFile + strlen(mipFile)) - 2;
        else
            mipFileEnd -= 2;
    } else {
        if (outputExists && outputIsDir) {
            sleprintf(opts->noErrp, "ERROR: Output file \"%s\" must be a file\n", output);
            return TTS_PROGERROR;
        }
        
        mipFile = csprintf_s("%s", output);
        if (!mipFile) {
            sleprintf(opts->noErrp, "ERROR: Failed to set output file path \"%s\"\n", output);
            return TTS_MEMERROR;
        }
    }
    
    sloprintf(opts->noOutp, "Reading input TXTR \"%s\"...\n", input);
    
    TXTR_t txtr;
    TTStatus_t tre = readTXTR(opts->noErrp, input, &txtr);
    if (tre) {
        free(mipFile);
        return tre;
    }
    
    sloprintf(opts->noOutp, "Decoding TXTR...\n");
    
    TXTRMipmap_t mips[11];
    size_t mipsCount;
    TXTRDecodeOptions_t texOpts = {
        .flipX = false,
        .flipY = true,
        .decAllMips = opts->mipmaps
    };
    TXTRDecodeError_t tde = TXTR_Decode(&txtr, mips, &mipsCount, &texOpts);
    if (tde) {
        sleprintf(opts->noErrp, "ERROR: Failed to decode TXTR data: %s\n", TXTRDecodeError_ToStr(tde));
        free(mipFile);
        TXTR_free(&txtr);
        
        switch (tde) {
            case TXTR_DE_INVLDTEXFMT:
            case TXTR_DE_INVLDPALFMT:
            case TXTR_DE_INVLDTEXWIDTH:
            case TXTR_DE_INVLDTEXHEIGHT:
            case TXTR_DE_INVLDTEXMIPCNT:
                return TTS_FMTERROR;
            case TXTR_DE_MEMFAILPAL:
            case TXTR_DE_MEMFAILMIP:
            case TXTR_DE_INVLDTEXPAL:
            case TXTR_DE_INVLDTEXMIPS:
                return TTS_MEMERROR;
            case TXTR_DE_INVLDPARAMS:
                return TTS_ARGERROR;
            case TXTR_DE_INTERRUPTED:
            case TXTR_DE_FAILDECPAL:
            default:
                return TTS_PROGERROR;
        }
    }
    TXTR_free(&txtr);
    
    char *iToC = /* major */ "00000000011" /* minor */ "12345678901";
    for (size_t m = 0; catexit_loopSafety && m < mipsCount; m++) {
        if (opts->mipmaps) {
            mipFileEnd[0] = iToC[m];
            mipFileEnd[1] = iToC[11 + m];
        }
        
        sloprintf(opts->noOutp, "Writing mipmap %zu to output TGA \"%s\"\n", m + 1, mipFile);
        
        TTStatus_t twe = writeTGA(opts->noOutp, opts->noErrp, opts->yes, opts->no, mipFile, TT_TITLE, &mips[m]);
        if (twe) {
            free(mipFile);
            for (size_t m = 0; m < mipsCount; m++)
                TXTRMipmap_free(&mips[m]);
            return twe;
        }
    }
    free(mipFile);
    for (size_t m = 0; m < mipsCount; m++)
        TXTRMipmap_free(&mips[m]);
    
    return TTS_SUCCESS;
}
#endif

#ifdef TXTRTOOL_INCLUDE_ENCODE
static TTStatus_t encode(TTEncodeOptions_t *opts, char *input, char *output) {
    bool inputIsDir = false;
    bool inputExists = !cfexists(input, &inputIsDir);
    if (inputExists && inputIsDir) {
        sleprintf(opts->noErrp, "ERROR: Input file \"%s\" must be a file\n", input);
        return TTS_PROGERROR;
    }
    
    char *cdn = NULL;
    char *outputDir = cdirname(output, &cdn);
    if (cdn) {
        bool outputDirIsDir = false;
        bool outputDirExists = !cfexists(outputDir, &outputDirIsDir);
        if (!outputDirExists) {
            sloprintf(opts->no || opts->yes || opts->noOutp, "Output file's directory \"%s\" does not exist. Create "
                "it? (y,Y/ANY) ", outputDir);
            if (opts->no || askYN(opts->yes, opts->noOutp)) {
                sleprintf(opts->noErrp, "ERROR: Not creating output file's directory \"%s\"\n", outputDir);
                free(cdn);
                return TTS_PROGERROR;
            } else {
                if (cmkdir(outputDir)) {
                    sleprintf(opts->noErrp, "ERROR: Failed to create output file's directory \"%s\": %s\n", outputDir,
                        strerror(errno));
                    free(cdn);
                    return TTS_IOERROR;
                }
            }
        }
        free(cdn);
    }
    
    sloprintf(opts->noOutp, "Reading input TGA \"%s\"...\n", input);
    
    TGA_t tga;
    TTStatus_t tre = readTGA(opts->noErrp, input, &tga);
    if (tre)
        return tre;
    
    sloprintf(opts->noOutp, "Encoding TXTR...\n");
    
    TXTR_t txtr;
    TXTRRawMipmap_t txtrMips[11];
    TXTREncodeOptions_t texOpts = {
        .flipX = false,
        .flipY = false,
        .mipLimit = opts->mipLimit,
        .widthLimit = opts->widthLimit,
        .heightLimit = opts->heightLimit,
        .avgType = opts->avgTypeDec,
        .squishFlags = opts->squishFlags,
        .squishMetricSz = opts->squishMetricSz,
        .squishMetric = opts->squishMetricPtr,
        .stbirEdge = opts->stbirEdgeDec,
        .stbirFilter = opts->stbirFilterDec,
        .ditherType = opts->ditherTypeDec
    };
    TXTREncodeError_t tee = TXTR_Encode(opts->texFmtDec, opts->palFmtDec, tga.hdr.imageSpec.width,
        tga.hdr.imageSpec.height, tga.dataSz, tga.data, &txtr, txtrMips, &texOpts);
    if (tee) {
        sleprintf(opts->noErrp, "ERROR: Failed to encode TXTR data: %s\n", TXTREncodeError_ToStr(tee));
        TGA_free(&tga);
        
        switch (tee) {
            case TXTR_EE_MEMFAILSRCPXS:
            case TXTR_EE_MEMFAILMIP:
            case TXTR_EE_MEMFAILPAL:
                return TTS_MEMERROR;
            case TXTR_EE_INVLDPARAMS:
            case TXTR_EE_INVLDTEXFMT:
            case TXTR_EE_INVLDPALFMT:
            case TXTR_EE_INVLDTEXWIDTH:
            case TXTR_EE_INVLDTEXHEIGHT:
            case TXTR_EE_INVLDTEXMIPLMT:
            case TXTR_EE_INVLDTEXWIDTHLMT:
            case TXTR_EE_INVLDTEXHEIGHTLMT:
            case TXTR_EE_INVLDGXAVGTYPE:
            case TXTR_EE_TRYMIPPALFMT:
            case TXTR_EE_INVLDSTBIREDGEMODE:
            case TXTR_EE_INVLDSTBIRFILTER:
            case TXTR_EE_INVLDSQUISHMETRICSZ:
            case TXTR_EE_INVLDGXDITHERTYPE:
                return TTS_ARGERROR;
            case TXTR_EE_FAILBUILDPAL:
            case TXTR_EE_RESIZEFAIL:
            case TXTR_EE_INTERRUPTED:
            case TXTR_EE_FAILENCPAL:
            default:
                return TTS_PROGERROR;
        }
    }
    TGA_free(&tga);
    
    sloprintf(opts->noOutp, "Writing %u mipmap%s to output TXTR \"%s\"...\n", txtr.hdr.mipCount,
        txtr.hdr.mipCount != 1 ? "s" : "", output);
    
    TTStatus_t twe = writeTXTR(opts->noOutp, opts->noErrp, opts->yes, opts->no, output, &txtr, txtrMips);
    if (twe) {
        TXTR_free(&txtr);
        for (size_t m = 0; m < txtr.hdr.mipCount; m++)
            TXTRRawMipmap_free(&txtrMips[m]);
        return twe;
    }
    TXTR_free(&txtr);
    for (size_t m = 0; m < txtr.hdr.mipCount; m++)
        TXTRRawMipmap_free(&txtrMips[m]);
    
    return TTS_SUCCESS;
}
#endif

#ifdef TXTRTOOL_INCLUDE_MISC
static TTStatus_t print(TTPrintOptions_t *opts, char *input) {
    sleprintf(opts->noErrp, "Reading input TXTR \"%s\"...\n", input);
    
    TXTR_t txtr;
    TTStatus_t tre = readTXTR(opts->noErrp, input, &txtr);
    if (tre)
        return tre;
    
    if (opts->json) {
        sloprintf(opts->noOutp,
            "{\n"
            "    \"texture_format\": \"%s\",\n"
            "    \"texture_width\": %u,\n"
            "    \"texture_height\": %u,\n"
            "    \"texture_mipmap_count\": %u,\n"
            "    \"palette_format\": \"%s\",\n"
            "    \"palette_width\": %u,\n"
            "    \"palette_height\": %u\n"
            "}\n",
            Tex2Str(txtr.hdr.format),
            txtr.hdr.width,
            txtr.hdr.height,
            txtr.hdr.mipCount,
            txtr.isIndexed ? Pal2Str(txtr.palHdr.format) : "",
            txtr.isIndexed ? txtr.palHdr.width : 0,
            txtr.isIndexed ? txtr.palHdr.height : 0
        );
    } else {
        sloprintf(opts->noOutp, "Texture format: %s\nTexture dimensions: %ux%u\nTexture mipmaps: %u\n",
            Tex2Str(txtr.hdr.format), txtr.hdr.width, txtr.hdr.height, txtr.hdr.mipCount);
        if (txtr.isIndexed)
            sloprintf(opts->noOutp, "Palette format: %s\nPalette dimensions: %ux%u\n", Pal2Str(txtr.palHdr.format),
                txtr.palHdr.width, txtr.palHdr.height);
    }
    
    TXTR_free(&txtr);
    
    return TTS_SUCCESS;
}
#endif

// optparse99 tasks
static void printHelp(int argc, char **argv) {
    optparse_print_help_subcmd_noexit(argc, argv);
    exit(TTS_SUCCESS);
}

static void printVersion(int argc, char **argv) {
    FAKEREF(argc);
    FAKEREF(argv);
    oprintf("%s\n", TT_TITLE);
    exit(TTS_SUCCESS);
}

#ifdef TXTRTOOL_INCLUDE_DECODE
static void setDecodeMode(int argc, char **argv) {
    FAKEREF(argc);
    FAKEREF(argv);
    ttMode = TTM_DECODE;
}
#endif

#ifdef TXTRTOOL_INCLUDE_ENCODE
static void setEncodeMode(int argc, char **argv) {
    FAKEREF(argc);
    FAKEREF(argv);
    ttMode = TTM_ENCODE;
}
#endif

#ifdef TXTRTOOL_INCLUDE_MISC
static void setPrintMode(int argc, char **argv) {
    FAKEREF(argc);
    FAKEREF(argv);
    ttMode = TTM_PRINT;
}
#endif

// Main entry point
int main(int argc, char **argv) {
    if (!argc)
        return TTS_ERROR;
    
    // Prevent any interrupts messing up loops (all loops will exit gracefully)
    // However, safety cant be guaranteed for external libraries that dont implement catexit_loopSafety.
    // For such cases, catexit_loopSafety will have to be checked after the library has been used.
    // Any other side effects for the libraries not dealing with catexit_loopSafety are unavoidable.
    if (!catexit(catexit_defaultExitHandler))
        return TTS_ERROR;
    
#ifdef TXTRTOOL_INCLUDE_DECODE
    TTDecodeOptions_t decOpts = {
        .noOutp = (int) false,
        .noErrp = (int) false,
        .yes = (int) false,
        .no = (int) false,
        .mipmaps = (int) false,
        .prefix = "",
        .suffix = ""
    };
#endif
    
#ifdef TXTRTOOL_INCLUDE_ENCODE
    TTEncodeOptions_t encOpts = {
        .noOutp = (int) false,
        .noErrp = (int) false,
        .yes = (int) false,
        .no = (int) false,
        .texFmt = TOSTR(RGBA8),
        .texFmtDec = TXTR_TTF_RGBA8,
        .palFmt = TOSTR(RGB5A3),
        .palFmtDec = TXTR_TPF_RGB5A3,
        .mipLimit = 1,
        .widthLimit = 1,
        .heightLimit = 1,
        .avgType = TOSTR(AVERAGE),
        .avgTypeDec = GX_AT_AVERAGE,
        .stbirEdge = TOSTR(CLAMP),
        .stbirEdgeDec = STBIR_EDGE_CLAMP,
        .stbirFilter = TOSTR(DEFAULT),
        .stbirFilterDec = STBIR_FILTER_DEFAULT,
        .ditherType = TOSTR(THRESHOLD),
        .ditherTypeDec = GX_DT_THRESHOLD,
        .squishMetricPtr = NULL,
        .squishMetricSz = 0,
        .squishMetric = { 1.0f, 1.0f, 1.0f },
        .squishMetricValid = false,
        .squishAlphaWeight = (int) false,
        .squishClusterFit = (int) false,
        .squishRangeFit = (int) false,
        .squishIterClusterFit = (int) false,
        .squishFlags = 0
    };
#endif
    
#ifdef TXTRTOOL_INCLUDE_MISC
    TTPrintOptions_t prtOpts = {
        .noOutp = (int) false,
        .noErrp = (int) false,
        .json = (int) false
    };
#endif
    
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
    struct optparse_cmd mainCmd = {
        .name = "txtrtool",
        .about = TT_ABOUT,
        .description = TT_DESCRIPTION,
        .function = printHelp,
        .subcommands = (struct optparse_cmd[]) {
            {
                .name = "help",
                .about = "Print a subcommand's help information and quit.",
                .operands = "<command>",
                .function = printHelp
            },
            {
                .name = "version",
                .about = "Print the program version and quit.",
                .function = printVersion
            },
#ifdef TXTRTOOL_INCLUDE_DECODE
            {
                .name = "decode",
                .about = "Decode a TXTR to a TGA or a set of TGAs for every mipmap.",
                .operands = "<input txtr> <output tga/output directory>",
                .function = setDecodeMode,
                .options = (struct optparse_opt[]) {
                    {
                        .short_name = 's',
                        .long_name = "nooutp",
                        .flag = &decOpts.noOutp,
                        .flag_type = FLAG_TYPE_SET_TRUE,
                        .description = "Do not print output."
                    },
                    {
                        .short_name = 'e',
                        .long_name = "noerrp",
                        .flag = &decOpts.noErrp,
                        .flag_type = FLAG_TYPE_SET_TRUE,
                        .description = "Do not print errors."
                    },
                    {
                        .short_name = 'y',
                        .long_name = "yes",
                        .flag = &decOpts.yes,
                        .flag_type = FLAG_TYPE_SET_TRUE,
                        .description = "Assume yes to every prompt."
                    },
                    {
                        .short_name = 'n',
                        .long_name = "no",
                        .flag = &decOpts.no,
                        .flag_type = FLAG_TYPE_SET_TRUE,
                        .description = "Assume no to every prompt."
                    },
                    {
                        .short_name = 'm',
                        .long_name = "mipmaps",
                        .flag = &decOpts.mipmaps,
                        .flag_type = FLAG_TYPE_SET_TRUE,
                        .description = "Decode all mipmaps from the TXTR. Outputs to a directory instead."
                    },
                    {
                        .short_name = 'b',
                        .long_name = "prefix",
                        .arg_name = "string",
                        .arg_data_type = DATA_TYPE_STR,
                        .arg_storage = &decOpts.prefix,
                        .description = "Prefix for each mipmap file name. This only has effect if --mipmaps "
                            "specified. (Default: )"
                    },
                    {
                        .short_name = 'a',
                        .long_name = "suffix",
                        .arg_name = "string",
                        .arg_data_type = DATA_TYPE_STR,
                        .arg_storage = &decOpts.suffix,
                        .description = "Suffix for each mipmap file name. This only has effect if --mipmaps "
                            "specified. (Default: )"
                    },
                    { END_OF_OPTIONS }
                }
            },
#endif
#ifdef TXTRTOOL_INCLUDE_ENCODE
            {
                .name = "encode",
                .about = "Encode a TGA to a TXTR.",
                .operands = "<input tga> <output txtr>",
                .function = setEncodeMode,
                .options = (struct optparse_opt[]) {
                    {
                        .short_name = 's',
                        .long_name = "nooutp",
                        .flag = &encOpts.noOutp,
                        .flag_type = FLAG_TYPE_SET_TRUE,
                        .description = "Do not print output."
                    },
                    {
                        .short_name = 'e',
                        .long_name = "noerrp",
                        .flag = &encOpts.noErrp,
                        .flag_type = FLAG_TYPE_SET_TRUE,
                        .description = "Do not print errors."
                    },
                    {
                        .short_name = 'y',
                        .long_name = "yes",
                        .flag = &encOpts.yes,
                        .flag_type = FLAG_TYPE_SET_TRUE,
                        .description = "Assume yes to every prompt."
                    },
                    {
                        .short_name = 'n',
                        .long_name = "no",
                        .flag = &encOpts.no,
                        .flag_type = FLAG_TYPE_SET_TRUE,
                        .description = "Assume no to every prompt."
                    },
                    {
                        .short_name = 't',
                        .long_name = "texfmt",
                        .arg_name = "string",
                        .arg_data_type = DATA_TYPE_STR,
                        .arg_storage = &encOpts.texFmt,
                        .description = "The texture format to set for the output TXTR. Valid values: " TexList(", ")
                            " (Default: " TOSTR(RGBA8) ")"
                    },
                    {
                        .short_name = 'p',
                        .long_name = "palfmt",
                        .arg_name = "string",
                        .arg_data_type = DATA_TYPE_STR,
                        .arg_storage = &encOpts.palFmt,
                        .description = "For " TOSTR(CI4) ", " TOSTR(CI8) ", and " TOSTR(CI14X2) ": The palette format "
                            "to set for the output TXTR. Valid values: " PalList(", ") " (Default: " TOSTR(RGB5A3) ")"
                    },
                    {
                        .short_name = 'm',
                        .long_name = "miplimit",
                        .arg_name = "uint8",
                        .arg_data_type = DATA_TYPE_UINT8,
                        .arg_storage = &encOpts.mipLimit,
                        .description = "The maximum limit of mipmaps to encode. 0 means no limit ergo 11 mipmaps. "
                            "This must be greater than or equal to 0 and less than 12. For " TOSTR(CI4) ", " TOSTR(CI8)
                            ", and " TOSTR(CI14X2) ": this must be 1 or 0 and 0 means 1 instead of 11. (Default: 1)"
                    },
                    {
                        .short_name = 'w',
                        .long_name = "widthlimit",
                        .arg_name = "uint16",
                        .arg_data_type = DATA_TYPE_UINT16,
                        .arg_storage = &encOpts.widthLimit,
                        .description = "The minimum limit of mipmap width. Must be greater than 0. (Default: 1)"
                    },
                    {
                        .short_name = 'h',
                        .long_name = "heightlimit",
                        .arg_name = "uint16",
                        .arg_data_type = DATA_TYPE_UINT16,
                        .arg_storage = &encOpts.heightLimit,
                        .description = "The minimum limit of mipmap height. Must be greater than 0. (Default: 1)"
                    },
                    {
                        .long_name = "avgtype",
                        .arg_name = "string",
                        .arg_data_type = DATA_TYPE_STR,
                        .arg_storage = &encOpts.avgType,
                        .description = "For " TOSTR(I4) ", " TOSTR(I8) ", " TOSTR(IA4) ", and " TOSTR(IA8) ": The "
                            "formula to use for greyscaling. Valid values: " AvgTypList(", ") " (Default: "
                            TOSTR(AVERAGE) ")"
                    },
                    {
                        .long_name = "stbiredge",
                        .arg_name = "string",
                        .arg_data_type = DATA_TYPE_STR,
                        .arg_storage = &encOpts.stbirEdge,
                        .description = "The type of edge mode to use for mipmaps. Valid values: " EdgeList(", ")
                            " (Default: " TOSTR(CLAMP) ")"
                    },
                    {
                        .long_name = "stbirfilter",
                        .arg_name = "string",
                        .arg_data_type = DATA_TYPE_STR,
                        .arg_storage = &encOpts.stbirFilter,
                        .description = "The type of filter to use for mipmaps. Valid values: " FilterList(", ")
                            " (Default: " TOSTR(DEFAULT) ")"
                    },
                    {
                        .long_name = "dithertype",
                        .arg_name = "string",
                        .arg_data_type = DATA_TYPE_STR,
                        .arg_storage = &encOpts.ditherType,
                        .description = "For " TOSTR(CI4) ", " TOSTR(CI8) ", and " TOSTR(CI14X2) ": The type of dither "
                            "operation during quantization. Valid values: "
                            DitherTypeList(", ") " (Default: " TOSTR(THRESHOLD) ")"
                    },
                    {
                        .long_name = "squishmetric",
                        .arg_name = "float,float,float",
                        .arg_data_type = DATA_TYPE_FLT,
                        .arg_delim = ",",
                        .arg_storage = &encOpts.squishMetricPtr,
                        .arg_storage_size = &encOpts.squishMetricSz,
                        .description = "For " TOSTR(CMP) ": An optional perceptual metric used to weight the relative "
                            "importance of each colour channel. (Default: 1.0,1.0,1.0)"
                    },
                    {
                        .long_name = "squishalphaweight",
                        .flag = &encOpts.squishAlphaWeight,
                        .flag_type = FLAG_TYPE_SET_TRUE,
                        .description = "For " TOSTR(CMP) ": Weight the color by alpha during cluster fit. This has no "
                            "effect if --squishrangefit specified."
                    },
                    {
                        .long_name = "squishclusterfit",
                        .flag = &encOpts.squishClusterFit,
                        .flag_type = FLAG_TYPE_SET_TRUE,
                        .group = 1,
                        .description = "For " TOSTR(CMP) ": Use a slow but high quality compressor (Default)."
                    },
                    {
                        .long_name = "squishrangefit",
                        .flag = &encOpts.squishRangeFit,
                        .flag_type = FLAG_TYPE_SET_TRUE,
                        .group = 1,
                        .description = "For " TOSTR(CMP) ": Use a fast but low quality compressor."
                    },
                    {
                        .long_name = "squishiterclusterfit",
                        .flag = &encOpts.squishIterClusterFit,
                        .flag_type = FLAG_TYPE_SET_TRUE,
                        .group = 1,
                        .description = "For " TOSTR(CMP) ": Use a very slow but very high quality compressor."
                    },
                    { END_OF_OPTIONS }
                }
            },
#endif
#ifdef TXTRTOOL_INCLUDE_MISC
            {
                .name = "print",
                .about = "Print information of a TXTR such as its format, dimensions, etc.",
                .operands = "<input txtr>",
                .function = setPrintMode,
                .options = (struct optparse_opt[]) {
                    {
                        .short_name = 's',
                        .long_name = "nooutp",
                        .flag = &prtOpts.noOutp,
                        .flag_type = FLAG_TYPE_SET_TRUE,
                        .description = "Do not print output."
                    },
                    {
                        .short_name = 'e',
                        .long_name = "noerrp",
                        .flag = &prtOpts.noErrp,
                        .flag_type = FLAG_TYPE_SET_TRUE,
                        .description = "Do not print errors."
                    },
                    {
                        .short_name = 'j',
                        .long_name = "json",
                        .flag = &prtOpts.json,
                        .flag_type = FLAG_TYPE_SET_TRUE,
                        .description = "Print output to JSON formatted data."
                    },
                    { END_OF_OPTIONS }
                }
            },
#endif
            { END_OF_SUBCOMMANDS }
        }
    };
#pragma GCC diagnostic pop
    
    optparse_parse(&mainCmd, &argc, &argv);
    
    // Handle any list's memory ASAP, validation will happen later in logical order
    encOpts.squishMetricValid = !!encOpts.squishMetricPtr;
    if (encOpts.squishMetricValid) {
        if (encOpts.squishMetricSz == 3)
            memcpy((float *) encOpts.squishMetric, encOpts.squishMetricPtr, 3);
        free(encOpts.squishMetricPtr);
        encOpts.squishMetricPtr = encOpts.squishMetric;
    } else
        encOpts.squishMetricPtr = SQUISH_DEFAULT_METRIC;
    
    // if this isnt true, then the program should have already quit from an exit call before reaching here
    assert(ttMode >= TTM_DECODE && ttMode <= TTM_PRINT);
    
    argc--;
    argv++;
    
    // follow some of optparse99's logic at the argument validation level:
    // - errors should not be silenced (there would be no way to tell if they should be silenced beforehand)
    // - return status code should be of EXIT_FAILURE (which is what TTS_ERROR equates to) upon error
#ifdef TXTRTOOL_INCLUDE_DECODE
    if (ttMode == TTM_DECODE) {
        if (argc < 2 || !*(argv[0]) || !*(argv[1])) {
            eprintf("ERROR: At least two operands are required.\n");
            return TTS_ERROR;
        } else
            return decode(&decOpts, argv[0], argv[1]);
    }
#endif
#ifdef TXTRTOOL_INCLUDE_ENCODE
    if (ttMode == TTM_ENCODE) {
        if (argc < 2 || !*(argv[0]) || !*(argv[1])) {
            eprintf("ERROR: At least two operands are required.\n");
            return TTS_ERROR;
        } else {
            encOpts.texFmtDec = Str2Tex(encOpts.texFmt);
            if (encOpts.texFmtDec == TXTR_TTF_INVALID) {
                eprintf("ERROR: --texfmt: Invalid format \"%s\". Valid values: " TexList(", ") "\n", encOpts.texFmt);
                return TTS_ERROR;
            }
            
            encOpts.palFmtDec = Str2Pal(encOpts.palFmt);
            if (encOpts.palFmtDec == TXTR_TPF_INVALID) {
                eprintf("ERROR: --palfmt: Invalid format \"%s\". Valid values: " PalList(", ") "\n", encOpts.palFmt);
                return TTS_ERROR;
            }
            
            if (encOpts.mipLimit > 11) {
                eprintf("ERROR: --miplimit: Limit %i must be less than 12.\n", encOpts.mipLimit);
                return TTS_ERROR;
            } else if (TXTR_IsIndexed(encOpts.texFmtDec) && encOpts.mipLimit > 1) {
                eprintf("ERROR: --miplimit: Limit %i must be either 1 or 0 on indexed formats.\n", encOpts.mipLimit);
                return TTS_ERROR;
            }
            if (!encOpts.mipLimit)
                encOpts.mipLimit = !TXTR_IsIndexed(encOpts.texFmtDec) ? 11 : 1;
            
            if (!encOpts.widthLimit) {
                eprintf("ERROR: --widthlimit: Limit %u must be greater than 0.\n", encOpts.widthLimit);
                return TTS_ERROR;
            }
            // There is no way to check if its greater than image width at this point
            
            if (!encOpts.heightLimit) {
                eprintf("ERROR: --heightlimit: Limit %u must be greater than 0.\n", encOpts.heightLimit);
                return TTS_ERROR;
            }
            // There is no way to check if its greater than image height at this point
            
            encOpts.avgTypeDec = Str2AvgTyp(encOpts.avgType);
            if (encOpts.avgTypeDec == GX_AT_INVALID) {
                eprintf("ERROR: --avgtype: Invalid average type \"%s\". Valid values: " AvgTypList(", ") "\n",
                    encOpts.avgType);
                return TTS_ERROR;
            }
            
            encOpts.stbirEdgeDec = Str2Edge(encOpts.stbirEdge);
            if (encOpts.stbirEdgeDec < STBIR_EDGE_CLAMP || encOpts.stbirEdgeDec > STBIR_EDGE_ZERO) {
                eprintf("ERROR: --stbiredge: Invalid edge mode \"%s\". Valid values: " EdgeList(", ") "\n",
                    encOpts.stbirEdge);
                return TTS_ERROR;
            }
            
            encOpts.stbirFilterDec = Str2Filter(encOpts.stbirFilter);
            if (encOpts.stbirFilterDec < STBIR_FILTER_DEFAULT || encOpts.stbirFilterDec > STBIR_FILTER_POINT_SAMPLE) {
                eprintf("ERROR: --stbirfilter: Invalid filter mode \"%s\". Valid values: " FilterList(", ") "\n",
                    encOpts.stbirFilter);
                return TTS_ERROR;
            }
            
            encOpts.ditherTypeDec = Str2DitherType(encOpts.ditherType);
            if (encOpts.ditherTypeDec == GX_DT_INVALID) {
                eprintf("ERROR: --dithertype: Invalid dither type \"%s\". Valid values: " DitherTypeList(", ") "\n",
                    encOpts.ditherType);
                return TTS_ERROR;
            }
            
            if (encOpts.squishMetricValid) {
                if (encOpts.squishMetricSz != 3) {
                    eprintf("ERROR: --squishmetric: Metric of size %zu must be 3.\n", encOpts.squishMetricSz);
                    return TTS_ERROR;
                } else {
                    if (encOpts.squishMetricPtr[0] < 0.0f || encOpts.squishMetricPtr[0] > 1.0f) {
                        eprintf("ERROR: --squishmetric: Metric's first component must be between 0.0 and 1.0.\n");
                        return TTS_ERROR;
                    }
                    
                    if (encOpts.squishMetricPtr[1] < 0.0f || encOpts.squishMetricPtr[1] > 1.0f) {
                        eprintf("ERROR: --squishmetric: Metric's second component must be between 0.0 and 1.0.\n");
                        return TTS_ERROR;
                    }
                    
                    if (encOpts.squishMetricPtr[2] < 0.0f || encOpts.squishMetricPtr[2] > 1.0f) {
                        eprintf("ERROR: --squishmetric: Metric's third component must be between 0.0 and 1.0.\n");
                        return TTS_ERROR;
                    }
                }
            }
            
            
            if (!!encOpts.squishAlphaWeight)
                encOpts.squishFlags |= kWeightColourByAlpha;
            if (!!encOpts.squishClusterFit)
                encOpts.squishFlags |= kColourClusterFit;
            if (!!encOpts.squishRangeFit)
                encOpts.squishFlags |= kColourRangeFit;
            if (!!encOpts.squishIterClusterFit)
                encOpts.squishFlags |= kColourIterativeClusterFit;
            
            return encode(&encOpts, argv[0], argv[1]);
        }
    }
#endif
#ifdef TXTRTOOL_INCLUDE_MISC
    if (ttMode == TTM_PRINT) {
        if (argc < 1 || !*(argv[0])) {
            eprintf("ERROR: At least one operand is required.\n");
            return TTS_ERROR;
        } else
            return print(&prtOpts, argv[0]);
    }
#endif
    
    // As stated in the previous assert, this should never be reached but it is here just in case
    eprintf("ERROR: Unknown command mode specified: %i.\n", ttMode);
    return TTS_ERROR;
}
