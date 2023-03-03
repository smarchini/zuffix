#!/usr/bin/env bash
SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")

git clone git@github.com:facebook/folly.git
git clone git@github.com:madler/zlib.git
git clone git@github.com:Cyan4973/xxHash.git
git clone git@github.com:y-256/libdivsufsort.git
git clone git@github.com:vigna/sux.git
git clone git@github.com:IlyaGrebnov/libsais.git
git clone git@github.com:wangyi-fudan/wyhash.git

# libdivsufsort
mkdir -p "$SCRIPTPATH"/libdivsufsort/build
cd "$SCRIPTPATH"/libdivsufsort/build || exit
cmake -DBUILD_DIVSUFSORT64=ON -DBUILD_SHARED_LIBS=OFF -DBUILD_EXAMPLES=OFF -DUSE_OPENMP=ON ..
make CFLAGS="-march=native -O3 "

# zlib
mkdir -p "$SCRIPTPATH"/zlib/build
cd "$SCRIPTPATH"/zlib || exit
./configure
prefix="$SCRIPTPATH"/zlib/build CFLAGS="-march=native -O3" ./configure --static
make install

# xxhash
mkdir -p "$SCRIPTPATH"/xxHash/build
cd "$SCRIPTPATH"/xxHash || exit
prefix="$SCRIPTPATH"/xxHash/build CFLAGS="-march=native -O3" make install

# libsais
mkdir -p "$SCRIPTPATH"/libsais/build
cd "$SCRIPTPATH"/libsais || exit
sed -i "s/PROJECT=sais/PROJECT?=sais/" Makefile
PREFIX="$SCRIPTPATH"/libsais/build CFLAGS="-march=native -O3 -fopenmp" PROJECT=sais64 make all
PREFIX="$SCRIPTPATH"/libsais/build CFLAGS="-march=native -O3 -fopenmp" PROJECT=sais64 make install
PREFIX="$SCRIPTPATH"/libsais/build CFLAGS="-march=native -O3 -fopenmp" PROJECT=sais make all
PREFIX="$SCRIPTPATH"/libsais/build CFLAGS="-march=native -O3 -fopenmp" PROJECT=sais make install
