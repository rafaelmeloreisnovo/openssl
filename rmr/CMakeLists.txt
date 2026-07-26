cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(rmr_openssl_silicon
  VERSION 1.1.0
  DESCRIPTION "RMR OpenSSL bit/flags/linker/silicon contract"
  LANGUAGES C ASM)

include(CTest)
include(cmake/RMROptimization.cmake)

option(RMR_BUILD_EVP_TOOLS "Build hosted OpenSSL EVP KAT and benchmark" ON)
option(RMR_BUILD_BENCHMARK "Build the percentile EVP benchmark" ON)

set(RMR_COMMON_DEFINITIONS
  RMR_NO_HEAP=1
  RMR_FUNCTION_SECTIONS=1
  RMR_DATA_SECTIONS=1
  RMR_HIDDEN_VISIBILITY=1
  RMR_GC_SECTIONS=1
  RMR_BUILD_ID_NONE=1)

function(rmr_configure_core target use_asm freestanding)
  add_library(${target} STATIC core/rmr_silicon.c)
  target_include_directories(${target} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
  target_compile_features(${target} PUBLIC c_std_11)
  target_compile_definitions(${target} PRIVATE ${RMR_COMMON_DEFINITIONS})
  set_target_properties(${target} PROPERTIES
    C_EXTENSIONS OFF
    C_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
    POSITION_INDEPENDENT_CODE OFF)

  if(freestanding)
    target_compile_definitions(${target} PRIVATE
      RMR_NO_LIBC=1
      RMR_NO_BUILTIN=1
      RMR_NO_STACK_PROTECTOR=1)
    target_compile_options(${target} PRIVATE
      $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-ffreestanding>
      $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-fno-builtin>
      $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-fno-stack-protector>)
  endif()

  rmr_apply_optimization(${target})
  rmr_resolve_profile(_profile)

  if(use_asm AND RMR_ENABLE_ASM)
    string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" _processor)
    if(_profile STREQUAL "x86_64-v3" OR
       (_profile STREQUAL "host-native" AND _processor MATCHES "x86_64|amd64"))
      target_sources(${target} PRIVATE arch/x86_64/rmr_mix32.S)
      target_compile_definitions(${target} PRIVATE RMR_USE_ARCH_ASM=1)
    elseif(_profile STREQUAL "aarch64-crypto" OR
           (_profile STREQUAL "host-native" AND _processor MATCHES "aarch64|arm64"))
      target_sources(${target} PRIVATE arch/aarch64/rmr_mix32.S)
      target_compile_definitions(${target} PRIVATE RMR_USE_ARCH_ASM=1)
    elseif(_profile STREQUAL "armv7-neon" OR
           (_profile STREQUAL "host-native" AND _processor MATCHES "armv7|armv8l"))
      target_sources(${target} PRIVATE arch/armv7/rmr_mix32.S)
      target_compile_definitions(${target} PRIVATE RMR_USE_ARCH_ASM=1)
    else()
      message(STATUS "RMR ASM disabled for profile '${_profile}' on '${CMAKE_SYSTEM_PROCESSOR}'")
    endif()
  endif()
endfunction()

# Two explicit variants preserve the distinction between reference C and ASM.
rmr_configure_core(rmr_silicon_c FALSE TRUE)
rmr_configure_core(rmr_silicon TRUE TRUE)
add_library(RMR::silicon ALIAS rmr_silicon)
add_library(RMR::silicon_c ALIAS rmr_silicon_c)

if(BUILD_TESTING)
  add_executable(rmr_kat_c tests/kat.c)
  target_link_libraries(rmr_kat_c PRIVATE RMR::silicon_c)
  rmr_apply_optimization(rmr_kat_c)
  rmr_apply_link_optimization(rmr_kat_c)

  add_executable(rmr_kat tests/kat.c)
  target_link_libraries(rmr_kat PRIVATE RMR::silicon)
  rmr_apply_optimization(rmr_kat)
  rmr_apply_link_optimization(rmr_kat)

  if(UNIX AND NOT APPLE)
    target_link_options(rmr_kat_c PRIVATE
      -Wl,-T,${CMAKE_CURRENT_SOURCE_DIR}/linker/rmr_sections.ld)
    target_link_options(rmr_kat PRIVATE
      -Wl,-T,${CMAKE_CURRENT_SOURCE_DIR}/linker/rmr_sections.ld)
  endif()

  add_test(NAME rmr.kat.c COMMAND rmr_kat_c)
  add_test(NAME rmr.kat.arch COMMAND rmr_kat)
endif()

if(RMR_BUILD_EVP_TOOLS)
  find_package(OpenSSL 3 QUIET COMPONENTS Crypto)
  if(OpenSSL_FOUND)
    add_executable(rmr_openssl_probe tools/openssl_probe.c)
    target_link_libraries(rmr_openssl_probe PRIVATE RMR::silicon_c OpenSSL::Crypto)
    rmr_apply_optimization(rmr_openssl_probe)
    rmr_apply_link_optimization(rmr_openssl_probe)

    add_executable(rmr_openssl_evp_kat tools/openssl_evp_kat.c)
    target_link_libraries(rmr_openssl_evp_kat PRIVATE RMR::silicon_c OpenSSL::Crypto)
    rmr_apply_optimization(rmr_openssl_evp_kat)
    rmr_apply_link_optimization(rmr_openssl_evp_kat)

    if(BUILD_TESTING)
      add_test(NAME rmr.openssl.evp.kat COMMAND rmr_openssl_evp_kat)
    endif()

    if(RMR_BUILD_BENCHMARK)
      add_executable(rmr_openssl_evp_bench tools/openssl_evp_bench.c)
      target_link_libraries(rmr_openssl_evp_bench PRIVATE RMR::silicon_c OpenSSL::Crypto)
      rmr_apply_optimization(rmr_openssl_evp_bench)
      rmr_apply_link_optimization(rmr_openssl_evp_bench)
    endif()
  else()
    message(STATUS "OpenSSL::Crypto not found: hosted EVP tools remain TOKEN_VAZIO")
  endif()
endif()

rmr_resolve_profile(RMR_FINAL_PROFILE)
file(GENERATE
  OUTPUT "${CMAKE_BINARY_DIR}/rmr-cmake-flags-$<CONFIG>.txt"
  CONTENT
"project=${PROJECT_NAME}\nversion=${PROJECT_VERSION}\ncompiler=${CMAKE_C_COMPILER}\ncompiler_id=${CMAKE_C_COMPILER_ID}\ncompiler_version=${CMAKE_C_COMPILER_VERSION}\nsystem=${CMAKE_SYSTEM_NAME}\nprocessor=${CMAKE_SYSTEM_PROCESSOR}\nandroid_abi=${ANDROID_ABI}\nbuild_type=${CMAKE_BUILD_TYPE}\nprofile=${RMR_FINAL_PROFILE}\ncpu_tune=${RMR_CPU_TUNE}\nipo=${RMR_ENABLE_IPO}\nasm=${RMR_ENABLE_ASM}\ncore_compile_options=$<JOIN:$<TARGET_PROPERTY:rmr_silicon,COMPILE_OPTIONS>, >\ncore_definitions=$<JOIN:$<TARGET_PROPERTY:rmr_silicon,COMPILE_DEFINITIONS>, >\n")

message(STATUS "RMR CMake profile: ${RMR_FINAL_PROFILE}")
message(STATUS "RMR CPU tune: '${RMR_CPU_TUNE}'")
message(STATUS "RMR IPO requested: ${RMR_ENABLE_IPO}")
message(STATUS "RMR ASM requested: ${RMR_ENABLE_ASM}")
