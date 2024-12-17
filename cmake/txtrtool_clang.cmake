# cmake list preconfig for clang
cmake_minimum_required(VERSION 3.28, FATAL_ERROR)

if(NOT ${CMAKE_C_COMPILER_ID} MATCHES "Clang")
    message(FATAL_ERROR "Please use clang for this preconfig.")
endif()

include(txtrtool_any)

default_to_release()

ensure_debug_release()

erase_default_options()

macro(add_global_flags)
    # all
    set(TXTRTOOL_C_COMPILE_FLAGS
        SHELL:"-Werror=vla" SHELL:"-Werror=missing-prototypes" SHELL:"-Werror=aggregate-return" SHELL:"-Werror=switch" SHELL:"-Werror=uninitialized" SHELL:"-Werror=varargs" SHELL:"-Werror=shift-count-overflow" SHELL:"-Werror=gnu-zero-variadic-macro-arguments" SHELL:"-Werror=c23-extensions" SHELL:"-Wall" SHELL:"-Wextra" SHELL:"-pedantic-errors"
        $<$<CONFIG:Release>:SHELL:"-O2" SHELL:"-DNDEBUG">
        $<$<CONFIG:Debug>:SHELL:"-O0" SHELL:"-g3" SHELL:"-ggdb">)
    add_compile_options("$<$<COMPILE_LANGUAGE:C>:${TXTRTOOL_C_COMPILE_FLAGS}>")
endmacro()

macro(add_txtrtool_flags noasan)
    # txtrtool
    target_compile_options(txtrtool
        PUBLIC
            $<$<AND:$<CONFIG:Debug>,$<NOT:$<BOOL:${WIN32}>>,$<NOT:$<BOOL:${noasan}>>>:SHELL:"-fsanitize=address" SHELL:"-fsanitize=undefined" SHELL:"-fno-sanitize-recover" SHELL:"-fstack-protector-strong">)
    target_link_libraries(txtrtool
        PUBLIC 
            $<$<AND:$<CONFIG:Debug>,$<NOT:$<BOOL:${WIN32}>>,$<NOT:$<BOOL:${noasan}>>>:-lasan -lubsan>)
endmacro()

macro(set_stdext_extraopts)
    # stdext
    set(STDEXT_CSLPRINTF_H_INLINE_CLANG ON)
endmacro()

macro(add_optparse99_flags)
    # optparse99
    target_compile_options(optparse99
        PRIVATE
            SHELL:"-Wno-vla")
endmacro()

macro(add_global_sse_flags)
    # all
    set(TXTRTOOL_C_COMPILE_SSE_FLAGS
        "-msse")
    add_compile_options("$<$<COMPILE_LANGUAGE:C>:${TXTRTOOL_C_COMPILE_SSE_FLAGS}>")
endmacro()

macro(add_global_sse2_flags)
    # all
    set(TXTRTOOL_C_COMPILE_SSE2_FLAGS
        "-msse2")
    add_compile_options("$<$<COMPILE_LANGUAGE:C>:${TXTRTOOL_C_COMPILE_SSE2_FLAGS}>")
endmacro()

macro(add_global_avx_flags)
    # all
    set(TXTRTOOL_C_COMPILE_AVX_FLAGS
        "-mavx")
    add_compile_options("$<$<COMPILE_LANGUAGE:C>:${TXTRTOOL_C_COMPILE_AVX_FLAGS}>")
endmacro()

macro(add_global_avx2_flags)
    # all
    set(TXTRTOOL_C_COMPILE_AVX2_FLAGS
        "-mavx2")
    add_compile_options("$<$<COMPILE_LANGUAGE:C>:${TXTRTOOL_C_COMPILE_AVX2_FLAGS}>")
endmacro()

macro(add_global_vec_flags)
    # all
    set(TXTRTOOL_C_COMPILE_VEC_FLAGS
        SHELL:"-mfpmath=sse" SHELL:"-ffp-contract=off")
    add_compile_options("$<$<COMPILE_LANGUAGE:C>:${TXTRTOOL_C_COMPILE_VEC_FLAGS}>")
endmacro()
