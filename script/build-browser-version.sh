#!/usr/bin/env bash

mkdir -p build

cd build

emcc                                         \
  -O1                                        \
  -I ../vendor/pcre/10.23/src                   \
  -I ../vendor/pcre/include                     \
  -D HAVE_CONFIG_H                           \
  -D PCRE2_CODE_UNIT_WIDTH=16                \
  -c                                         \
  ../vendor/pcre/pcre2_chartables.c             \
  ../vendor/pcre/10.23/src/pcre2_auto_possess.c \
  ../vendor/pcre/10.23/src/pcre2_compile.c      \
  ../vendor/pcre/10.23/src/pcre2_config.c       \
  ../vendor/pcre/10.23/src/pcre2_context.c      \
  ../vendor/pcre/10.23/src/pcre2_dfa_match.c    \
  ../vendor/pcre/10.23/src/pcre2_error.c        \
  ../vendor/pcre/10.23/src/pcre2_find_bracket.c \
  ../vendor/pcre/10.23/src/pcre2_jit_compile.c  \
  ../vendor/pcre/10.23/src/pcre2_maketables.c   \
  ../vendor/pcre/10.23/src/pcre2_match.c        \
  ../vendor/pcre/10.23/src/pcre2_match_data.c   \
  ../vendor/pcre/10.23/src/pcre2_newline.c      \
  ../vendor/pcre/10.23/src/pcre2_ord2utf.c      \
  ../vendor/pcre/10.23/src/pcre2_pattern_info.c \
  ../vendor/pcre/10.23/src/pcre2_serialize.c    \
  ../vendor/pcre/10.23/src/pcre2_string_utils.c \
  ../vendor/pcre/10.23/src/pcre2_study.c        \
  ../vendor/pcre/10.23/src/pcre2_substitute.c   \
  ../vendor/pcre/10.23/src/pcre2_substring.c    \
  ../vendor/pcre/10.23/src/pcre2_tables.c       \
  ../vendor/pcre/10.23/src/pcre2_ucd.c          \
  ../vendor/pcre/10.23/src/pcre2_valid_utf.c    \
  ../vendor/pcre/10.23/src/pcre2_xclass.c

em++                                    \
  --bind \
  -s ASYNCIFY=1 \
  -s NO_EXIT_RUNTIME=1 \
  -s ALLOW_MEMORY_GROWTH=1 \
  -s EXTRA_EXPORTED_RUNTIME_METHODS="['ccall','cwrap']" \
  -s EXPORT_ALL=1 \
  -s MODULARIZE=1 \
  -O1                                   \
  -o browser.js                         \
  -s LINKABLE=1 -s EXPORT_ALL=1 \
  -I ../src/bindings/em                    \
  -I ../src/core                           \
  -I ../vendor/libcxx                      \
  -I ../vendor/pcre/include                \
  -D PCRE2_CODE_UNIT_WIDTH=16           \
  ../src/core/*.cc                         \
  ../src/bindings/em/*.cc \
  *.o
