#!/usr/bin/env bash
SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
DESTPATH="$SCRIPTPATH"/results

DATE=$(date '+%Y%m%d')

benchmark() {
    mkdir -p "$DESTPATH"
    NAME=$(echo "$1" | rev | cut -d '/' -f1 | cut -c5- | rev )
    "$SCRIPTPATH"/bin/benchmark/fibonacci/simple                        "$1" --benchmark_format=csv > "$DESTPATH/fibonacci.$NAME.simple.$DATE.csv"
    "$SCRIPTPATH"/bin/benchmark/fibonacci/enhanced                      "$1" --benchmark_format=csv > "$DESTPATH/fibonacci.$NAME.enhanced.$DATE.csv"
    "$SCRIPTPATH"/bin/benchmark/fibonacci/memcmp-zuffix-wyhash          "$1" --benchmark_format=csv > "$DESTPATH/fibonacci.$NAME.memcmp-zuffix-wyhash.$DATE.csv"
    "$SCRIPTPATH"/bin/benchmark/fibonacci/memcmp-zuffix-xxh3            "$1" --benchmark_format=csv > "$DESTPATH/fibonacci.$NAME.memcmp-zuffix-xxh3.$DATE.csv"
    "$SCRIPTPATH"/bin/benchmark/fibonacci/memcmp-zuffix-crc32cfolly     "$1" --benchmark_format=csv > "$DESTPATH/fibonacci.$NAME.memcmp-zuffix-crc32cfolly.$DATE.csv"
    "$SCRIPTPATH"/bin/benchmark/fibonacci/memcmp-zuffix-crc32zlib       "$1" --benchmark_format=csv > "$DESTPATH/fibonacci.$NAME.memcmp-zuffix-crc32zlib.$DATE.csv"
    "$SCRIPTPATH"/bin/benchmark/fibonacci/memcmp-zuffix-crc32+crc32c    "$1" --benchmark_format=csv > "$DESTPATH/fibonacci.$NAME.memcmp-zuffix-crc32+crc32c.$DATE.csv"
    "$SCRIPTPATH"/bin/benchmark/fibonacci/signature-zuffix-crc32cfolly  "$1" --benchmark_format=csv > "$DESTPATH/fibonacci.$NAME.signature-zuffix-crc32cfolly.$DATE.csv"
    "$SCRIPTPATH"/bin/benchmark/fibonacci/signature-zuffix-crc32zlib    "$1" --benchmark_format=csv > "$DESTPATH/fibonacci.$NAME.signature-zuffix-crc32zlib.$DATE.csv"
    "$SCRIPTPATH"/bin/benchmark/fibonacci/signature-zuffix-crc32+crc32c "$1" --benchmark_format=csv > "$DESTPATH/fibonacci.$NAME.signature-zuffix-crc32+crc32c.$DATE.csv"
    "$SCRIPTPATH"/bin/benchmark/fibonacci/nothing-zuffix-wyhash         "$1" --benchmark_format=csv > "$DESTPATH/fibonacci.$NAME.nothing-zuffix-wyhash.$DATE.csv"
    "$SCRIPTPATH"/bin/benchmark/fibonacci/nothing-zuffix-xxh3           "$1" --benchmark_format=csv > "$DESTPATH/fibonacci.$NAME.nothing-zuffix-xxh3.$DATE.csv"
    "$SCRIPTPATH"/bin/benchmark/fibonacci/nothing-zuffix-crc32cfolly    "$1" --benchmark_format=csv > "$DESTPATH/fibonacci.$NAME.nothing-zuffix-crc32cfolly.$DATE.csv"
    "$SCRIPTPATH"/bin/benchmark/fibonacci/nothing-zuffix-crc32zlib      "$1" --benchmark_format=csv > "$DESTPATH/fibonacci.$NAME.nothing-zuffix-crc32zlib.$DATE.csv"
    "$SCRIPTPATH"/bin/benchmark/fibonacci/nothing-zuffix-crc32+crc32c   "$1" --benchmark_format=csv > "$DESTPATH/fibonacci.$NAME.nothing-zuffix-crc32+crc32c.$DATE.csv"
}

benchmark "$1"
