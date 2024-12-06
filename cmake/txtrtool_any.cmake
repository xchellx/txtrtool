# cmake list preconfig for any
cmake_minimum_required(VERSION 3.28, FATAL_ERROR)

function(default_to_release)
    # default to Release configuration especially in newer cmake releases where there is no default
    if (NOT GENERATOR_IS_MULTI_CONFIG AND (NOT CMAKE_BUILD_TYPE OR NOT CMAKE_BUILD_TYPE MATCHES "Debug|Release"))
        unset(CMAKE_BUILD_TYPE CACHE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
    endif()
endfunction()

function(ensure_debug_release)
    # ensure only Debug and Release configurations
    unset(CMAKE_CONFIGURATION_TYPES CACHE)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
endfunction()

function(erase_default_options)
    # erase default compiler options
    unset(CMAKE_C_FLAGS CACHE)
    set(CMAKE_C_FLAGS "" CACHE STRING "" FORCE)
    unset(CMAKE_C_FLAGS_DEBUG CACHE)
    set(CMAKE_C_FLAGS_DEBUG "" CACHE STRING "" FORCE)
    unset(CMAKE_C_FLAGS_RELEASE CACHE)
    set(CMAKE_C_FLAGS_RELEASE "" CACHE STRING "" FORCE)
    unset(CMAKE_C_FLAGS_INIT CACHE)
    set(CMAKE_C_FLAGS_INIT "" CACHE STRING "" FORCE)
    unset(CMAKE_C_FLAGS_DEBUG_INIT CACHE)
    set(CMAKE_C_FLAGS_DEBUG_INIT "" CACHE STRING "" FORCE)
    unset(CMAKE_C_FLAGS_RELEASE_INIT CACHE)
    set(CMAKE_C_FLAGS_RELEASE_INIT "" CACHE STRING "" FORCE)
endfunction()
