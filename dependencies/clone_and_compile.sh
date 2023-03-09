#!/usr/bin/env bash
SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")

# git clone git@github.com:facebook/folly.git
# git clone git@github.com:madler/zlib.git
# git clone git@github.com:Cyan4973/xxHash.git
# git clone git@github.com:y-256/libdivsufsort.git
# git clone git@github.com:vigna/sux.git
# git clone git@github.com:IlyaGrebnov/libsais.git
# git clone git@github.com:wangyi-fudan/wyhash.git
# git clone git@github.com:google/benchmark.git
#
# # libdivsufsort
# mkdir -p "$SCRIPTPATH"/libdivsufsort/build
# cd "$SCRIPTPATH"/libdivsufsort/build || exit
# cmake -DBUILD_DIVSUFSORT64=ON -DBUILD_SHARED_LIBS=OFF -DBUILD_EXAMPLES=OFF -DUSE_OPENMP=ON ..
# make CFLAGS="-march=native -O3 "
#
# # zlib
# mkdir -p "$SCRIPTPATH"/zlib/build
# cd "$SCRIPTPATH"/zlib || exit
# ./configure
# prefix="$SCRIPTPATH"/zlib/build CFLAGS="-march=native -O3" ./configure --static
# make install
#
# # xxhash
# mkdir -p "$SCRIPTPATH"/xxHash/build
# cd "$SCRIPTPATH"/xxHash || exit
# prefix="$SCRIPTPATH"/xxHash/build CFLAGS="-march=native -O3" make install

# libsais
mkdir -p "$SCRIPTPATH"/libsais/build
cd "$SCRIPTPATH"/libsais || exit
sed -i "s/PROJECT=sais/PROJECT?=sais/" Makefile
PREFIX="$SCRIPTPATH"/libsais/build CFLAGS="-march=native -O3 -fopenmp -fPIC" PROJECT=sais64 make -B all
PREFIX="$SCRIPTPATH"/libsais/build CFLAGS="-march=native -O3 -fopenmp -fPIC" PROJECT=sais64 make -B install
PREFIX="$SCRIPTPATH"/libsais/build CFLAGS="-march=native -O3 -fopenmp -fPIC" PROJECT=sais make -B all
PREFIX="$SCRIPTPATH"/libsais/build CFLAGS="-march=native -O3 -fopenmp -fPIC" PROJECT=sais make -B install

# benchmark
mkdir -p "$SCRIPTPATH"/benchmark/installed
cd "$SCRIPTPATH"/benchmark || exit
cmake -E make_directory "build"
cmake -E chdir "build" cmake -DBENCHMARK_DOWNLOAD_DEPENDENCIES=on -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$SCRIPTPATH"/benchmark/installed ../
cmake --build "build" --config Release --target install

# # folly
# mkdir -p "$SCRIPTPATH"/folly/installed
# cd "$SCRIPTPATH"/folly || exit
# python3 ./build/fbcode_builder/getdeps.py --allow-system-packages build --extra-cmake-defines '{"CMAKE_CXX_FLAGS": "-march=native", "CMAKE_C_FLAGS": "-march=native"}'
# cp -r /tmp/fbcode_builder_getdep*/installed/fmt-* "$SCRIPTPATH"/folly/installed/fmt
# cp -r /tmp/fbcode_builder_getdep*/installed/googletest-* "$SCRIPTPATH"/folly/installed/googletest
# mkdir -p "$SCRIPTPATH"/folly/installed/folly/lib
# mkdir -p "$SCRIPTPATH"/folly/installed/folly/include
# cp /tmp/fbcode_builder_getdep*/build/folly/*.a "$SCRIPTPATH"/folly/installed/folly/lib/
# mv /tmp/
# cd "$SCRIPTPATH"/folly/build || exit
# cmake -DCMAKE_CXX_FLAGS="-march=native -O3" ../
