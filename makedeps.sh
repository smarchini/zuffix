#!/usr/bin/env bash
SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
DESTPATH="$SCRIPTPATH"/dependencies
 
# libdivsufsort
cd "$DESTPATH" || exit
git clone git@github.com:y-256/libdivsufsort.git
mkdir -p "$DESTPATH"/libdivsufsort/build
cd "$DESTPATH"/libdivsufsort/build || exit
cmake -DBUILD_DIVSUFSORT64=ON -DBUILD_SHARED_LIBS=OFF -DBUILD_EXAMPLES=OFF -DUSE_OPENMP=ON ..
make CFLAGS="-march=native -flto -O3"

# zlib
cd "$DESTPATH" || exit
git clone git@github.com:madler/zlib.git
mkdir -p "$DESTPATH"/zlib/build
cd "$DESTPATH"/zlib || exit
./configure
prefix="$DESTPATH"/zlib/build CFLAGS="-march=native -flto -O3" ./configure --static
make install

# wyhash
cd "$DESTPATH" || exit
git clone git@github.com:wangyi-fudan/wyhash.git

# xxhash
cd "$DESTPATH" || exit
git clone git@github.com:Cyan4973/xxHash.git
mkdir -p "$DESTPATH"/xxHash/build
cd "$DESTPATH"/xxHash || exit
prefix="$DESTPATH"/xxHash/build CFLAGS="-march=native -flto -O3" make install

# libsais
cd "$DESTPATH" || exit
git clone git@github.com:IlyaGrebnov/libsais.git
mkdir -p "$DESTPATH"/libsais/build
cd "$DESTPATH"/libsais || exit
sed -i "s/PROJECT=sais/PROJECT?=sais/" Makefile
PREFIX="$DESTPATH"/libsais/build CFLAGS="-march=native -flto -O3 -fopenmp -fPIC" PROJECT=sais64 make -B all
PREFIX="$DESTPATH"/libsais/build CFLAGS="-march=native -flto -O3 -fopenmp -fPIC" PROJECT=sais64 make -B install
PREFIX="$DESTPATH"/libsais/build CFLAGS="-march=native -flto -O3 -fopenmp -fPIC" PROJECT=sais make -B all
PREFIX="$DESTPATH"/libsais/build CFLAGS="-march=native -flto -O3 -fopenmp -fPIC" PROJECT=sais make -B install

# benchmark
cd "$DESTPATH" || exit
git clone git@github.com:google/benchmark.git
mkdir -p "$DESTPATH"/benchmark/installed
cd "$DESTPATH"/benchmark || exit
cmake -E make_directory "build"
cmake -E chdir "build" cmake -DBENCHMARK_DOWNLOAD_DEPENDENCIES=on -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$DESTPATH"/benchmark/installed ../
cmake --build "build" --config Release --target install

# sux
cd "$DESTPATH" || exit
git clone git@github.com:vigna/sux.git
cd "$DESTPATH"/sux || exit
git apply "$DESTPATH"/sux_vector_gcc_cpp20.patch

# folly
cd "$DESTPATH" || exit
git clone git@github.com:facebook/folly.git
mkdir -p "$DESTPATH"/folly/installed
cd "$DESTPATH"/folly || exit
python3 ./build/fbcode_builder/getdeps.py build --allow-system-packages --extra-cmake-defines '{"CMAKE_CXX_FLAGS": " -O3 -march=native -flto ", "BUILD_TESTS": "off"}'
cp -r /tmp/fbcode_builder_getdep*/installed/fmt-* "$DESTPATH"/folly/installed/fmt
cp -r /tmp/fbcode_builder_getdep*/installed/googletest-* "$DESTPATH"/folly/installed/googletest
cp -r /tmp/fbcode_builder_getdep*/installed/folly "$DESTPATH"/folly/installed/folly
rm -rf /tmp/fbcode_builder_getdeps-*

# pizza&chili
cd "$DESTPATH" || exit
wget -P "$DESTPATH" "http://pizzachili.dcc.uchile.cl/utils/utils.tar.gz"
tar -xzvf utils.tar.gz
rm utils.tar.gz
mv utils pizzachili
cd "$DESTPATH"/pizzachili || exit
make

# sdsl-lite
cd "$DESTPATH" || exit
git clone --recursive git@github.com:xxsds/sdsl-lite.git
cd "$DESTPATH"/sdsl-lite || exit
./install.sh ./installed
