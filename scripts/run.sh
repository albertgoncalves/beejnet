#!/usr/bin/env bash

set -eu

if [ ! -d "$WD/bin" ]; then
    mkdir "$WD/bin"
fi

flags=(
    -D_GNU_SOURCE
    "-ferror-limit=1"
    -fshort-enums
    "-march=native"
    -O1
    "-std=c99"
    -Werror
    -Weverything
    -Wno-cast-align
    -Wno-declaration-after-statement
    -Wno-disabled-macro-expansion
    -Wno-reserved-macro-identifier
)

path_source="$1"
path_bin="${WD}/bin/$(basename "$path_source" ".c")"

clang-format -i "$WD/src/"*
mold -run clang "${flags[@]}" -o "$path_bin" "$path_source"
"$path_bin"
