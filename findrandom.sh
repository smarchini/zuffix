#!/usr/bin/env bash
SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
DESTPATH="$SCRIPTPATH"/results

DATE=$(date '+%Y%m%d')

benchmark() {
    mkdir -p "$DESTPATH"
    NAME=$(echo "$1" | rev | cut -d '/' -f1 | cut -c5- | rev )
    "$SCRIPTPATH"/bin/benchmark/findrandom/simple                        "$1" --benchmark_format=csv > "$DESTPATH/findrandom.$NAME.simple.$DATE.csv"                        2> "$DESTPATH/findrandom.$NAME.simple.$DATE.info"
    "$SCRIPTPATH"/bin/benchmark/findrandom/enhanced                      "$1" --benchmark_format=csv > "$DESTPATH/findrandom.$NAME.enhanced.$DATE.csv"                      2> "$DESTPATH/findrandom.$NAME.enhanced.$DATE.info" 
    "$SCRIPTPATH"/bin/benchmark/findrandom/memcmp-zuffix-wyhash          "$1" --benchmark_format=csv > "$DESTPATH/findrandom.$NAME.memcmp-zuffix-wyhash.$DATE.csv"          2> "$DESTPATH/findrandom.$NAME.memcmp-zuffix-wyhash.$DATE.info"
    "$SCRIPTPATH"/bin/benchmark/findrandom/memcmp-zuffix-xxh3            "$1" --benchmark_format=csv > "$DESTPATH/findrandom.$NAME.memcmp-zuffix-xxh3.$DATE.csv"            2> "$DESTPATH/findrandom.$NAME.memcmp-zuffix-xxh3.$DATE.info"
    "$SCRIPTPATH"/bin/benchmark/findrandom/memcmp-zuffix-crc32cfolly     "$1" --benchmark_format=csv > "$DESTPATH/findrandom.$NAME.memcmp-zuffix-crc32cfolly.$DATE.csv"     2> "$DESTPATH/findrandom.$NAME.memcmp-zuffix-crc32cfolly.$DATE.info"
    "$SCRIPTPATH"/bin/benchmark/findrandom/memcmp-zuffix-crc32zlib       "$1" --benchmark_format=csv > "$DESTPATH/findrandom.$NAME.memcmp-zuffix-crc32zlib.$DATE.csv"       2> "$DESTPATH/findrandom.$NAME.memcmp-zuffix-crc32zlib.$DATE.info"
    "$SCRIPTPATH"/bin/benchmark/findrandom/memcmp-zuffix-crc32+crc32c    "$1" --benchmark_format=csv > "$DESTPATH/findrandom.$NAME.memcmp-zuffix-crc32+crc32c.$DATE.csv"    2> "$DESTPATH/findrandom.$NAME.memcmp-zuffix-crc32+crc32c.$DATE.info"
    "$SCRIPTPATH"/bin/benchmark/findrandom/signature-zuffix-crc32cfolly  "$1" --benchmark_format=csv > "$DESTPATH/findrandom.$NAME.signature-zuffix-crc32cfolly.$DATE.csv"  2> "$DESTPATH/findrandom.$NAME.signature-zuffix-crc32cfolly.$DATE.info"
    "$SCRIPTPATH"/bin/benchmark/findrandom/signature-zuffix-crc32zlib    "$1" --benchmark_format=csv > "$DESTPATH/findrandom.$NAME.signature-zuffix-crc32zlib.$DATE.csv"    2> "$DESTPATH/findrandom.$NAME.signature-zuffix-crc32zlib.$DATE.info"
    "$SCRIPTPATH"/bin/benchmark/findrandom/signature-zuffix-crc32+crc32c "$1" --benchmark_format=csv > "$DESTPATH/findrandom.$NAME.signature-zuffix-crc32+crc32c.$DATE.csv" 2> "$DESTPATH/findrandom.$NAME.signature-zuffix-crc32+crc32c.$DATE.info"
}

benchmark "$1"
