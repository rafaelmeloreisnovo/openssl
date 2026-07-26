# Copyright 2026 Rafael Melo Reis.
# Target-scoped optimization policy inspired by the BLAKE3 fork build model:
# architecture capability, source selection and compiler policy stay separate.

include(CheckIPOSupported)

set(RMR_ARCH_PROFILE "auto" CACHE STRING
    "Optimization profile: auto, portable, host-native, x86_64-v3, aarch64-crypto, armv7-neon")
set_property(CACHE RMR_ARCH_PROFILE PROPERTY STRINGS
    auto portable host-native x86_64-v3 aarch64-crypto armv7-neon)

set(RMR_CPU_TUNE "" CACHE STRING
    "Optional -mtune value. Empty keeps the profile portable across CPUs sharing the ISA.")
option(RMR_ENABLE_IPO "Enable interprocedural optimization when supported" ON)
option(RMR_ENABLE_ASM "Link the architecture-specific ASM backend" ON)
option(RMR_STRICT_WARNINGS "Treat compiler warnings as errors" ON)
option(RMR_OMIT_FRAME_POINTER "Omit frame pointer in optimized builds" ON)
option(RMR_NO_SEMANTIC_INTERPOSITION
       "Allow stronger optimization for hidden, non-preemptible ELF symbols" ON)

function(rmr_resolve_profile out_profile)
  if(NOT RMR_ARCH_PROFILE STREQUAL "auto")
    set(${out_profile} "${RMR_ARCH_PROFILE}" PARENT_SCOPE)
    return()
  endif()

  string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" _rmr_processor)
  if(ANDROID_ABI STREQUAL "armeabi-v7a" OR _rmr_processor MATCHES "armv7|armv8l")
    set(${out_profile} "armv7-neon" PARENT_SCOPE)
  elseif(ANDROID_ABI STREQUAL "arm64-v8a" OR _rmr_processor MATCHES "aarch64|arm64")
    set(${out_profile} "aarch64-crypto" PARENT_SCOPE)
  elseif(_rmr_processor MATCHES "x86_64|amd64")
    set(${out_profile} "x86_64-v3" PARENT_SCOPE)
  else()
    set(${out_profile} "portable" PARENT_SCOPE)
  endif()
endfunction()

function(rmr_apply_optimization target)
  rmr_resolve_profile(_rmr_profile)
  set(RMR_RESOLVED_ARCH_PROFILE "${_rmr_profile}" CACHE INTERNAL
      "Resolved RMR architecture profile")

  target_compile_options(${target} PRIVATE
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-O3>
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-fvisibility=hidden>
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-ffunction-sections>
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-fdata-sections>
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-fno-plt>
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-fno-asynchronous-unwind-tables>
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-fno-unwind-tables>
  )

  if(RMR_OMIT_FRAME_POINTER)
    target_compile_options(${target} PRIVATE
      $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-fomit-frame-pointer>)
  endif()

  if(RMR_NO_SEMANTIC_INTERPOSITION AND UNIX AND NOT APPLE)
    target_compile_options(${target} PRIVATE
      $<$<COMPILE_LANG_AND_ID:C,GNU,Clang>:-fno-semantic-interposition>)
  endif()

  if(RMR_STRICT_WARNINGS)
    target_compile_options(${target} PRIVATE
      $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Wall>
      $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Wextra>
      $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Wpedantic>
      $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Werror>
    )
  endif()

  if(_rmr_profile STREQUAL "host-native")
    target_compile_options(${target} PRIVATE
      $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-march=native>
      $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-mtune=native>
      $<$<COMPILE_LANGUAGE:ASM>:-march=native>)
  elseif(_rmr_profile STREQUAL "x86_64-v3")
    target_compile_options(${target} PRIVATE
      $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-march=x86-64-v3>
      $<$<COMPILE_LANGUAGE:ASM>:-march=x86-64-v3>)
  elseif(_rmr_profile STREQUAL "aarch64-crypto")
    target_compile_options(${target} PRIVATE
      $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-march=armv8-a+crypto+crc+simd>
      $<$<COMPILE_LANGUAGE:ASM>:-march=armv8-a+crypto+crc+simd>)
  elseif(_rmr_profile STREQUAL "armv7-neon")
    target_compile_options(${target} PRIVATE
      $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-march=armv7-a>
      $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-mfpu=neon-vfpv4>
      $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-mfloat-abi=softfp>
      $<$<COMPILE_LANGUAGE:ASM>:-march=armv7-a>
      $<$<COMPILE_LANGUAGE:ASM>:-mfpu=neon-vfpv4>
      $<$<COMPILE_LANGUAGE:ASM>:-mfloat-abi=softfp>)
  elseif(NOT _rmr_profile STREQUAL "portable")
    message(FATAL_ERROR "Unknown RMR_ARCH_PROFILE='${_rmr_profile}'")
  endif()

  if(NOT RMR_CPU_TUNE STREQUAL "")
    target_compile_options(${target} PRIVATE
      $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-mtune=${RMR_CPU_TUNE}>
      $<$<COMPILE_LANGUAGE:ASM>:-mtune=${RMR_CPU_TUNE}>)
  endif()

  if(RMR_ENABLE_IPO)
    check_ipo_supported(RESULT _rmr_ipo_supported OUTPUT _rmr_ipo_error
                        LANGUAGES C)
    if(_rmr_ipo_supported)
      set_property(TARGET ${target} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
      message(STATUS "RMR IPO unavailable: ${_rmr_ipo_error}")
    endif()
  endif()
endfunction()

function(rmr_apply_link_optimization target)
  if(UNIX AND NOT APPLE)
    target_link_options(${target} PRIVATE
      -Wl,--gc-sections
      -Wl,--build-id=none)
  endif()
endfunction()
