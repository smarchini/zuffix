#!/usr/bin/env bash
SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
DESTPATH="$SCRIPTPATH"/results

DATE=$(date '+%Y%m%d')

benchmark() {
    mkdir -p "$DESTPATH"
    NAME=$(echo "$1" | rev | cut -d '/' -f1 | cut -c5- | rev )
    "$SCRIPTPATH"/bin/benchmark/nofindrandom/simple                        "$1" --benchmark_format=csv > "$DESTPATH/nofindrandom.$NAME.simple.$DATE.csv"
    "$SCRIPTPATH"/bin/benchmark/nofindrandom/enhanced                      "$1" --benchmark_format=csv > "$DESTPATH/nofindrandom.$NAME.enhanced.$DATE.csv"
    "$SCRIPTPATH"/bin/benchmark/nofindrandom/memcmp-zuffix-wyhash          "$1" --benchmark_format=csv > "$DESTPATH/nofindrandom.$NAME.memcmp-zuffix-wyhash.$DATE.csv"
    "$SCRIPTPATH"/bin/benchmark/nofindrandom/memcmp-zuffix-xxh3            "$1" --benchmark_format=csv > "$DESTPATH/nofindrandom.$NAME.memcmp-zuffix-xxh3.$DATE.csv"
    "$SCRIPTPATH"/bin/benchmark/nofindrandom/memcmp-zuffix-crc32cfolly     "$1" --benchmark_format=csv > "$DESTPATH/nofindrandom.$NAME.memcmp-zuffix-crc32cfolly.$DATE.csv"
    "$SCRIPTPATH"/bin/benchmark/nofindrandom/memcmp-zuffix-crc32zlib       "$1" --benchmark_format=csv > "$DESTPATH/nofindrandom.$NAME.memcmp-zuffix-crc32zlib.$DATE.csv"
    "$SCRIPTPATH"/bin/benchmark/nofindrandom/signature-zuffix-crc32cfolly  "$1" --benchmark_format=csv > "$DESTPATH/nofindrandom.$NAME.signature-zuffix-crc32cfolly.$DATE.csv"
    "$SCRIPTPATH"/bin/benchmark/nofindrandom/signature-zuffix-crc32zlib    "$1" --benchmark_format=csv > "$DESTPATH/nofindrandom.$NAME.signature-zuffix-crc32zlib.$DATE.csv"
}

benchmark "$1"
