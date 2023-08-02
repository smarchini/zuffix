#!/usr/bin/env bash
SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
DESTPATH="$SCRIPTPATH"/results

DATE=$(date '+%Y%m%d')

benchmark() {
    mkdir -p "$DESTPATH"
    echo "$1"
    echo wyhash
    "$SCRIPTPATH"/bin/benchmark/build/memcmp-zuffix-wyhash "$1"
    echo xxh3
    "$SCRIPTPATH"/bin/benchmark/build/memcmp-zuffix-xxh3 "$1"
    echo folly
    "$SCRIPTPATH"/bin/benchmark/build/memcmp-zuffix-crc32cfolly "$1"
    echo zlib
    "$SCRIPTPATH"/bin/benchmark/build/memcmp-zuffix-crc32zlib "$1"
}

benchmark "$1"
