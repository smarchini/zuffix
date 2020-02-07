DEBUG = -g -O0
RELEASE = -g -O3 -DNDEBUG
CXXFLAGS = -std=c++17 -Wall -Wextra -march=native -I./external/sux/ -I./external/ -I./ -lgtest -lbenchmark

all: test benchmark

# EXTERNAL
external: external/init external/sdsl external/r-index

external/init:
	git submodule update --init --recursive

external/sdsl: external/init
	sed -i 's/$${HOME}/.\/external\/sdsl-lite\/install/' ./external/sdsl-lite/install.sh
	./external/sdsl-lite/install.sh

external/r-index: external/init external/sdsl
	sed -i 's/\~\/include/$${PROJECT_SOURCE_DIR}\/..\/sdsl-lite\/install\/include/' ./external/r-index/CMakeLists.txt
	sed -i 's/\~\/lib/$${PROJECT_SOURCE_DIR}\/..\/sdsl-lite\/install\/lib/' ./external/r-index/CMakeLists.txt
	mkdir -p ./external/r-index/build/
	cmake -B ./external/r-index/build/ ./external/r-index/
	make -C ./external/r-index/build
	mkdir -p bin/r-index
	cp ./external/r-index/build/ri-build ./bin/r-index/
	cp ./external/r-index/build/ri-count ./bin/r-index/
	cp ./external/r-index/build/ri-locate ./bin/r-index/


# TEST
test: bin/test/zuffix bin/test/random
	./bin/test/zuffix --gtest_color=yes
	./bin/test/random --gtest_color=yes

bin/test/zuffix: test/zuffix/* zuffix/* zuffix/*/*
	@mkdir -p bin/test
	$(CXX) $(CXXFLAGS) $(DEBUG) test/zuffix/test.cpp external/SpookyV2.cpp -o bin/test/zuffix

bin/test/random: test/random/* zuffix/* zuffix/*/*
	@mkdir -p bin/test
	$(CXX) $(CXXFLAGS) $(DEBUG) test/random/test.cpp -o bin/test/random


# BENCHMARK
benchmark: bin/benchmark/build_suffix_array bin/benchmark/zuffix_random_dna bin/benchmark/zuffix_file
	./bin/benchmark/build_suffix_array --benchmark_color=yes
	./bin/benchmark/zuffix_random_dna --benchmark_color=yes

bin/benchmark/build_suffix_array: benchmark/*  zuffix/* zuffix/*/*
	@mkdir -p bin/benchmark
	$(CXX) $(CXXFLAGS) $(RELEASE) benchmark/build_suffix_array.cpp -o bin/benchmark/build_suffix_array

bin/benchmark/zuffix_random_dna: benchmark/*  zuffix/* zuffix/*/*
	@mkdir -p bin/benchmark
	$(CXX) $(CXXFLAGS) $(RELEASE) benchmark/zuffix_random_dna.cpp external/SpookyV2.cpp -o bin/benchmark/zuffix_random_dna

bin/benchmark/zuffix_file: benchmark/*  zuffix/* zuffix/*/*
	@mkdir -p bin/benchmark
	$(CXX) $(CXXFLAGS) $(RELEASE) -DSYMBOLTYPE=char benchmark/zuffix_file.cpp external/SpookyV2.cpp -o bin/benchmark/zuffix_char_file
#	$(CXX) $(CXXFLAGS) $(RELEASE) -DSYMBOLTYPE=uint8_t benchmark/zuffix_file.cpp external/SpookyV2.cpp -o bin/benchmark/zuffix_uint8_file
#	$(CXX) $(CXXFLAGS) $(RELEASE) -DSYMBOLTYPE=uint16_t benchmark/zuffix_file.cpp external/SpookyV2.cpp -o bin/benchmark/zuffix_uint16_file
#	$(CXX) $(CXXFLAGS) $(RELEASE) -DSYMBOLTYPE=uint32_t benchmark/zuffix_file.cpp external/SpookyV2.cpp -o bin/benchmark/zuffix_uint32_file
#	$(CXX) $(CXXFLAGS) $(RELEASE) -DSYMBOLTYPE=uint64_t benchmark/zuffix_file.cpp external/SpookyV2.cpp -o bin/benchmark/zuffix_uint64_file


# UTILS
utils: bin/utils/generate_pattern pizzachili

bin/utils/generate_pattern: utils/*
	@mkdir -p bin/utils
	$(CXX) $(CXXFLAGS) $(RELEASE) utils/generate_pattern.cpp -o bin/utils/generate_pattern

pizzachili:
	@mkdir -p pizzachili
	wget -P ./pizzachili "http://pizzachili.dcc.uchile.cl/utils/utils.tar.gz"
	aunpack -X ./pizzachili -e ./pizzachili/utils.tar.gz
	mv ./pizzachili/utils/* ./pizzachili
	rmdir ./pizzachili/utils
	make -C ./pizzachili

# DATASET
dataset: dataset/download
	@mkdir -p dataset
	aunpack -X ./dataset -e ./dataset/download/*

dataset/download:
	@mkdir -p dataset/download
	wget -P ./dataset/download "http://pizzachili.dcc.uchile.cl/texts/dna/dna.gz"
	wget -P ./dataset/download "http://pizzachili.dcc.uchile.cl/texts/dna/dna.50MB.gz"
	wget -P ./dataset/download "http://pizzachili.dcc.uchile.cl/texts/dna/dna.100MB.gz"
	wget -P ./dataset/download "http://pizzachili.dcc.uchile.cl/texts/dna/dna.200MB.gz"


# PATTERN
patterns: utils
	@mkdir -p pattern
# dna
	./pizzachili/genpatterns ./dataset/dna 1 100 ./pattern/dna.1 || true
	./pizzachili/genpatterns ./dataset/dna 1000 100 ./pattern/dna.1000 || true
# dna.50MB
	./pizzachili/genpatterns ./dataset/dna.50MB 1 100 ./pattern/dna.50MB.1 || true
	./pizzachili/genpatterns ./dataset/dna.50MB 10 100 ./pattern/dna.50MB.10 || true
	./pizzachili/genpatterns ./dataset/dna.50MB 1000000 100 ./pattern/dna.50MB.1000000 || true
	./pizzachili/genpatterns ./dataset/dna.50MB 10000000 100 ./pattern/dna.50MB.10000000 || true
	./pizzachili/genpatterns ./dataset/dna.50MB 50000000 100 ./pattern/dna.50MB.50000000 || true
# dna.100MB
	./pizzachili/genpatterns ./dataset/dna.100MB 1 100 ./pattern/dna.100MB.1 || true
	./pizzachili/genpatterns ./dataset/dna.100MB 10 100 ./pattern/dna.100MB.10 || true
	./pizzachili/genpatterns ./dataset/dna.100MB 1000000 100 ./pattern/dna.100MB.1000000 || true
	./pizzachili/genpatterns ./dataset/dna.100MB 10000000 100 ./pattern/dna.100MB.10000000 || true
	./pizzachili/genpatterns ./dataset/dna.100MB 100000000 100 ./pattern/dna.100MB.100000000 || true
# dna.200MB
	./pizzachili/genpatterns ./dataset/dna.200MB 1 100 ./pattern/dna.200MB.1 || true
	./pizzachili/genpatterns ./dataset/dna.200MB 10 100 ./pattern/dna.200MB.10 || true
	./pizzachili/genpatterns ./dataset/dna.200MB 1000000 100 ./pattern/dna.200MB.1000000 || true
	./pizzachili/genpatterns ./dataset/dna.200MB 10000000 100 ./pattern/dna.200MB.10000000 || true
	./pizzachili/genpatterns ./dataset/dna.200MB 200000000 100 ./pattern/dna.200MB.200000000 || true


.PHONY: clean dataset

clean:
	@rm -rf ./bin
	@rm -rf ./dataset
	@rm -rf ./pattern
	@rm -rf ./pizzachili
