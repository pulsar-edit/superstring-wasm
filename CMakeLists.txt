project(superstring)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_C_COMPILER emcc)
set(CMAKE_CXX_COMPILER em++)

file(GLOB CORE_SOURCES "src/core/*.cc")
file(GLOB EM_CORE_SOURCES "src/bindings/em/*.cc")

add_executable(
  browser
  vendor/pcre/pcre2_chartables.c
  vendor/pcre/10.23/src/pcre2_auto_possess.c
  vendor/pcre/10.23/src/pcre2_compile.c
  vendor/pcre/10.23/src/pcre2_config.c
  vendor/pcre/10.23/src/pcre2_context.c
  vendor/pcre/10.23/src/pcre2_dfa_match.c
  vendor/pcre/10.23/src/pcre2_error.c
  vendor/pcre/10.23/src/pcre2_find_bracket.c
  vendor/pcre/10.23/src/pcre2_jit_compile.c
  vendor/pcre/10.23/src/pcre2_maketables.c
  vendor/pcre/10.23/src/pcre2_match.c
  vendor/pcre/10.23/src/pcre2_match_data.c
  vendor/pcre/10.23/src/pcre2_newline.c
  vendor/pcre/10.23/src/pcre2_ord2utf.c
  vendor/pcre/10.23/src/pcre2_pattern_info.c
  vendor/pcre/10.23/src/pcre2_serialize.c
  vendor/pcre/10.23/src/pcre2_string_utils.c
  vendor/pcre/10.23/src/pcre2_study.c
  vendor/pcre/10.23/src/pcre2_substitute.c
  vendor/pcre/10.23/src/pcre2_substring.c
  vendor/pcre/10.23/src/pcre2_tables.c
  vendor/pcre/10.23/src/pcre2_ucd.c
  vendor/pcre/10.23/src/pcre2_valid_utf.c
  vendor/pcre/10.23/src/pcre2_xclass.c
  ${CORE_SOURCES}
  ${EM_CORE_SOURCES}
)

set(EMSCRIPTEN_INCLUDES  $ENV{EMSDK}/upstream/emscripten/system/include)

target_compile_options(
  browser
  PUBLIC
  -c
  -I${EMSCRIPTEN_INCLUDES}
  -Ivendor/pcre/10.23/src
  -Ivendor/pcre/include
  -O3
  -Isrc/bindings/em
  -Isrc/core
  -Ivendor/libcxx
  -DHAVE_CONFIG_H
  -DPCRE2_CODE_UNIT_WIDTH=16
)

target_link_options(
  browser
  PUBLIC
  --bind
  -O3
  -sASYNCIFY=1
  -sNO_EXIT_RUNTIME=1
  -sALLOW_MEMORY_GROWTH=1
  -sEXTRA_EXPORTED_RUNTIME_METHODS=['ccall','cwrap']
  -sEXPORT_ALL=1
  -sMODULARIZE=1
  -sLINKABLE=1
  -sEXPORT_ALL=1
)

set_target_properties(
  browser
  PROPERTIES
  OUTPUT_NAME "browser.js"
)
