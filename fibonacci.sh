#!/usr/bin/env bash
SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
DESTPATH="$SCRIPTPATH"/results

DATE=$(date '+%Y%m%d')

benchmark() {
    mkdir -p "$DESTPATH"
    "$SCRIPTPATH"/bin/benchmark/fibonacci/simple                        --benchmark_format=csv > "$DESTPATH/fibonacci.simple.$DATE.csv"                        2>  "$DESTPATH/fibonacci.simple.$DATE.info"
    "$SCRIPTPATH"/bin/benchmark/fibonacci/enhanced                      --benchmark_format=csv > "$DESTPATH/fibonacci.enhanced.$DATE.csv"                      2>  "$DESTPATH/fibonacci.enhanced.$DATE.info"
    "$SCRIPTPATH"/bin/benchmark/fibonacci/memcmp-zuffix-wyhash          --benchmark_format=csv > "$DESTPATH/fibonacci.memcmp-zuffix-wyhash.$DATE.csv"          2>  "$DESTPATH/fibonacci.memcmp-zuffix-wyhash.$DATE.info"
    "$SCRIPTPATH"/bin/benchmark/fibonacci/memcmp-zuffix-xxh3            --benchmark_format=csv > "$DESTPATH/fibonacci.memcmp-zuffix-xxh3.$DATE.csv"            2>  "$DESTPATH/fibonacci.memcmp-zuffix-xxh3.$DATE.info"
    "$SCRIPTPATH"/bin/benchmark/fibonacci/memcmp-zuffix-crc32cfolly     --benchmark_format=csv > "$DESTPATH/fibonacci.memcmp-zuffix-crc32cfolly.$DATE.csv"     2>  "$DESTPATH/fibonacci.memcmp-zuffix-crc32cfolly.$DATE.info"
    "$SCRIPTPATH"/bin/benchmark/fibonacci/memcmp-zuffix-crc32zlib       --benchmark_format=csv > "$DESTPATH/fibonacci.memcmp-zuffix-crc32zlib.$DATE.csv"       2>  "$DESTPATH/fibonacci.memcmp-zuffix-crc32zlib.$DATE.info"
    "$SCRIPTPATH"/bin/benchmark/fibonacci/signature-zuffix-crc32cfolly  --benchmark_format=csv > "$DESTPATH/fibonacci.signature-zuffix-crc32cfolly.$DATE.csv"  2>  "$DESTPATH/fibonacci.signature-zuffix-crc32cfolly.$DATE.info"
    "$SCRIPTPATH"/bin/benchmark/fibonacci/signature-zuffix-crc32zlib    --benchmark_format=csv > "$DESTPATH/fibonacci.signature-zuffix-crc32zlib.$DATE.csv"    2>  "$DESTPATH/fibonacci.signature-zuffix-crc32zlib.$DATE.info"
}

benchmark "$1"
