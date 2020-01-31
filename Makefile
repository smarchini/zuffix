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
utils: bin/utils/generate_pattern

bin/utils/generate_pattern: utils/*
	@mkdir -p bin/utils
	$(CXX) $(CXXFLAGS) $(RELEASE) utils/generate_pattern.cpp -o bin/utils/generate_pattern


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
	./bin/utils/generate_pattern ./dataset/dna 1 1 > ./pattern/dna.1
	./bin/utils/generate_pattern ./dataset/dna 1 1000 > ./pattern/dna.1000
# dna.50MB
	./bin/utils/generate_pattern ./dataset/dna.50MB 1 1 > ./pattern/dna.50MB.1
	./bin/utils/generate_pattern ./dataset/dna.50MB 1 10 > ./pattern/dna.50MB.10
	./bin/utils/generate_pattern ./dataset/dna.50MB 1 1000000 > ./pattern/dna.50MB.1000000
	./bin/utils/generate_pattern ./dataset/dna.50MB 1 10000000 > ./pattern/dna.50MB.10000000
	./bin/utils/generate_pattern ./dataset/dna.50MB 1 50000000 > ./pattern/dna.50MB.50000000
# dna.100MB
	./bin/utils/generate_pattern ./dataset/dna.100MB 1 1 > ./pattern/dna.100MB.1
	./bin/utils/generate_pattern ./dataset/dna.100MB 1 10 > ./pattern/dna.100MB.10
	./bin/utils/generate_pattern ./dataset/dna.100MB 1 1000000 > ./pattern/dna.100MB.1000000
	./bin/utils/generate_pattern ./dataset/dna.100MB 1 10000000 > ./pattern/dna.100MB.10000000
	./bin/utils/generate_pattern ./dataset/dna.100MB 1 100000000 > ./pattern/dna.100MB.100000000
# dna.200MB
	./bin/utils/generate_pattern ./dataset/dna.200MB 1 1 > ./pattern/dna.200MB.1
	./bin/utils/generate_pattern ./dataset/dna.200MB 1 10 > ./pattern/dna.200MB.10
	./bin/utils/generate_pattern ./dataset/dna.200MB 1 1000000 > ./pattern/dna.200MB.1000000
	./bin/utils/generate_pattern ./dataset/dna.200MB 1 10000000 > ./pattern/dna.200MB.10000000
	./bin/utils/generate_pattern ./dataset/dna.200MB 1 200000000 > ./pattern/dna.200MB.200000000


.PHONY: clean dataset

clean:
	@rm -rf ./bin
	@rm -rf ./dataset
	@rm -rf ./pattern
