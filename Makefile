DEBUG = -g -O0
RELEASE = -g -O3 -DNDEBUG
CXXFLAGS = -std=c++17 -Wall -Wextra -march=native -I./external/sux/ -I./external/ -I./ -lgtest -lbenchmark

all: test benchmark

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
benchmark: bin/benchmark/build_suffix_array bin/benchmark/zuffix_dna bin/benchmark/zuffix_find_dna
	./bin/benchmark/build_suffix_array --benchmark_color=yes
	./bin/benchmark/zuffix_dna --benchmark_color=yes

bin/benchmark/build_suffix_array: benchmark/*  zuffix/* zuffix/*/*
	@mkdir -p bin/benchmark
	$(CXX) $(CXXFLAGS) $(RELEASE) benchmark/build_suffix_array.cpp -o bin/benchmark/build_suffix_array

bin/benchmark/zuffix_dna: benchmark/*  zuffix/* zuffix/*/*
	@mkdir -p bin/benchmark
	$(CXX) $(CXXFLAGS) $(RELEASE) benchmark/zuffix_dna.cpp external/SpookyV2.cpp -o bin/benchmark/zuffix_dna

bin/benchmark/zuffix_find_dna: benchmark/*  zuffix/* zuffix/*/*
	@mkdir -p bin/benchmark
	$(CXX) $(CXXFLAGS) $(RELEASE) benchmark/zuffix_find_dna.cpp external/SpookyV2.cpp -o bin/benchmark/zuffix_find_dna


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


.PHONY: clean dataset

clean:
	@rm -rf ./bin
