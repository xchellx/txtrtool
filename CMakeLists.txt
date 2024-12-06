# txtrtool cmake list
cmake_minimum_required(VERSION 3.28, FATAL_ERROR)

# TODO: AppleClang

project(txtrtool LANGUAGES C)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" "${PROJECT_SOURCE_DIR}/extern/gxtexture_base/extern/cmake-modules")

if(NOT ${CMAKE_C_COMPILER_ID} MATCHES "GNU|Clang")
    message(FATAL_ERROR "Unsupported compiler. If using MVSC, use MSYS2 MINGW or MSYS2 CLANG64 instead.")
elseif(${CMAKE_C_COMPILER_ID} MATCHES "GNU")
    include(txtrtool_gcc)
elseif(${CMAKE_C_COMPILER_ID} MATCHES "Clang")
    include(txtrtool_clang)
endif()

add_global_flags()

include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)
git_describe(GIT_TAG --tags)
git_local_changes(GIT_LOCAL_CHANGES)

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/include/configure/txtrtool_version.h")
    configure_file("${PROJECT_SOURCE_DIR}/include/configure/txtrtool_version.h.in" "${PROJECT_SOURCE_DIR}/include/configure/txtrtool_version.h" @ONLY)
endif()

option(TXTRTOOL_INCLUDE_DECODE "Include decoding capabilities within txtrool." ON)
option(TXTRTOOL_INCLUDE_ENCODE "Include encoding capabilities within txtrool." ON)
option(TXTRTOOL_INCLUDE_MISC "Include miscellaneous capabilities within txtrool." ON)
if(WIN32)
    set(TXTRTOOL_NOASAN ON)
    set(MSYS_PATH "/c/msys64/clang64/bin" CACHE STRING "Path to MSYS2's bin directory (either clang64 or mingw64) for copying libraries at postbuild step in Windows builds")
else()
    option(TXTRTOOL_NOASAN "Do NOT include asan (address sanitizer) on Linux Debug builds." OFF)
endif()

set(TXTRTOOL_REALLY_INCLUDE_TXTR_DECODE ${TXTRTOOL_INCLUDE_DECODE})
if(NOT TXTRTOOL_REALLY_INCLUDE_TXTR_DECODE AND TXTRTOOL_INCLUDE_MISC)
    set(TXTRTOOL_REALLY_INCLUDE_TXTR_DECODE ON)
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/include/configure/txtrtool_settings.h")
    configure_file("${PROJECT_SOURCE_DIR}/include/configure/txtrtool_settings.h.in" "${PROJECT_SOURCE_DIR}/include/configure/txtrtool_settings.h" @ONLY)
endif()

if(STBIR_SSE2 OR BUILD_SQUISH_WITH_SSE2)
    add_global_sse2_flags()
endif()
if(STBIR_AVX)
    add_global_avx_flags()
endif()
if(STBIR_AVX2)
    add_global_avx2_flags()
endif()
add_global_vec_flags()

add_executable(txtrtool
    ${PROJECT_SOURCE_DIR}/include/txtrtool.h
    
    ${PROJECT_SOURCE_DIR}/src/txtrtool.c
    
    ${PROJECT_SOURCE_DIR}/extern/gxtexture_base/extern/stb_helpers/stb_image_resize2_impl.c
    ${PROJECT_SOURCE_DIR}/extern/gxtexture_base/extern/stb_helpers/stb_ds_impl.c)

add_txtrtool_flags("${TXTRTOOL_NOASAN}")

target_include_directories(txtrtool
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include)

target_compile_features(txtrtool
    PRIVATE
        c_std_99
        c_function_prototypes
        c_variadic_macros)

set_target_properties(txtrtool
    PROPERTIES
        C_STANDARD 99
        C_STANDARD_REQUIRED ON)

target_precompile_headers(txtrtool
    PUBLIC
        "$<$<COMPILE_LANGUAGE:C>:${PROJECT_SOURCE_DIR}/include/configure/txtrtool_version.h>"
        "$<$<COMPILE_LANGUAGE:C>:${PROJECT_SOURCE_DIR}/include/configure/txtrtool_settings.h>")

set(STB_IMAGE_RESIZE_IMPLEMENTED ON)
set(STB_DS_IMPLEMENTED ON)

# optparse99
set(OPT_OPTPARSE_HELP_MAX_DIVIDER_WIDTH "42")
set(OPT_OPTPARSE_HELP_MAX_LINE_WIDTH "80")
set(OPT_OPTPARSE_HIDDEN_OPTIONS OFF)
set(OPT_OPTPARSE_HELP_USAGE_STYLE 1)
set(OPT_OPTPARSE_HELP_USAGE_OPTIONS_STRING "options")
if (NOT TARGET optparse99)
    add_subdirectory(${PROJECT_SOURCE_DIR}/extern/optparse99 CMAKE/txtrtool/extern/optparse99)
endif()
add_optparse99_flags()
target_link_libraries(txtrtool PUBLIC optparse99)

# stdext
set(STDEXT_INCLUDE_ERROR_STRINGS $<IF:$<CONFIG:Debug>,ON,OFF>)
set(STDEXT_TEST_ALIGNMENT $<IF:$<CONFIG:Debug>,ON,OFF>)
set_stdext_extraopts()
if (NOT TARGET stdext)
    add_subdirectory(${PROJECT_SOURCE_DIR}/extern/gxtexture_base/extern/stdext CMAKE/txtrtool/extern/gxtexture_base/extern/stdext)
endif()
target_link_libraries(txtrtool PUBLIC stdext)

# txtr
set(TXTR_INCLUDE_ERROR_STRINGS $<IF:$<CONFIG:Debug>,ON,OFF>)
set(TXTR_INCLUDE_DECODE ${TXTRTOOL_REALLY_INCLUDE_TXTR_DECODE})
set(TXTR_INCLUDE_ENCODE ${TXTRTOOL_INCLUDE_ENCODE})
if (NOT TARGET txtr)
    add_subdirectory(${PROJECT_SOURCE_DIR}/extern/gxtexture_base/txtr CMAKE/extern/gxtexture_base/txtr)
endif()
target_link_libraries(txtrtool PUBLIC txtr)

# tga
set(TGA_INCLUDE_ERROR_STRINGS $<IF:$<CONFIG:Debug>,ON,OFF>)
set(TGA_INCLUDE_DECODE ${TXTRTOOL_INCLUDE_DECODE})
set(TGA_INCLUDE_ENCODE ${TXTRTOOL_INCLUDE_ENCODE})
if (NOT TARGET tga)
    add_subdirectory(${PROJECT_SOURCE_DIR}/extern/gxtexture_base/tga CMAKE/extern/gxtexture_base/tga)
endif()
target_link_libraries(txtrtool PUBLIC tga)

install(TARGETS txtrtool
    RUNTIME DESTINATION DESTINATION "${CMAKE_INSTALL_BINDIR}"
    INCLUDES DESTINATION DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
