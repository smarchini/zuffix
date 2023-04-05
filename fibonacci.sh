#!/usr/bin/env bash
SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
DESTPATH="$SCRIPTPATH"/results

DATE=$(date '+%Y%m%d')

benchmark() {
    mkdir -p "$DESTPATH"
    "$SCRIPTPATH"/bin/benchmark/fibonacci/simple                        --benchmark_format=csv > "$DESTPATH/fibonacci.simple.$DATE.csv"
    "$SCRIPTPATH"/bin/benchmark/fibonacci/enhanced                      --benchmark_format=csv > "$DESTPATH/fibonacci.enhanced.$DATE.csv"
    "$SCRIPTPATH"/bin/benchmark/fibonacci/memcmp-zuffix-wyhash          --benchmark_format=csv > "$DESTPATH/fibonacci.memcmp-zuffix-wyhash.$DATE.csv"
    "$SCRIPTPATH"/bin/benchmark/fibonacci/memcmp-zuffix-xxh3            --benchmark_format=csv > "$DESTPATH/fibonacci.memcmp-zuffix-xxh3.$DATE.csv"
    "$SCRIPTPATH"/bin/benchmark/fibonacci/memcmp-zuffix-crc32cfolly     --benchmark_format=csv > "$DESTPATH/fibonacci.memcmp-zuffix-crc32cfolly.$DATE.csv"
    "$SCRIPTPATH"/bin/benchmark/fibonacci/memcmp-zuffix-crc32zlib       --benchmark_format=csv > "$DESTPATH/fibonacci.memcmp-zuffix-crc32zlib.$DATE.csv"
    "$SCRIPTPATH"/bin/benchmark/fibonacci/memcmp-zuffix-crc32+crc32c    --benchmark_format=csv > "$DESTPATH/fibonacci.memcmp-zuffix-crc32+crc32c.$DATE.csv"
    "$SCRIPTPATH"/bin/benchmark/fibonacci/signature-zuffix-crc32cfolly  --benchmark_format=csv > "$DESTPATH/fibonacci.signature-zuffix-crc32cfolly.$DATE.csv"
    "$SCRIPTPATH"/bin/benchmark/fibonacci/signature-zuffix-crc32zlib    --benchmark_format=csv > "$DESTPATH/fibonacci.signature-zuffix-crc32zlib.$DATE.csv"
    "$SCRIPTPATH"/bin/benchmark/fibonacci/signature-zuffix-crc32+crc32c --benchmark_format=csv > "$DESTPATH/fibonacci.signature-zuffix-crc32+crc32c.$DATE.csv"
}

benchmark "$1"
