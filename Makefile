SUX = $(HOME)/src/sux/  # Change me
DEBUG = -g -O0
RELEASE = -g -O3 -DNDEBUG
CXXFLAGS = -std=c++17 -Wall -Wextra -march=native -I./ -I$(SUX) -lgtest -lbenchmark

all: test benchmark

# TEST
test: bin/test/zuffix bin/test/random
	./bin/test/zuffix --gtest_color=yes
	./bin/test/random --gtest_color=yes

bin/test/zuffix: test/zuffix/* zuffix/* zuffix/*/*
	@mkdir -p bin/test
	$(CXX) $(CXXFLAGS) $(DEBUG) test/zuffix/test.cpp zuffix/external/SpookyV2.cpp -o bin/test/zuffix

bin/test/random: test/random/* zuffix/* zuffix/*/*
	@mkdir -p bin/test
	$(CXX) $(CXXFLAGS) $(DEBUG) test/random/test.cpp -o bin/test/random


# BENCHMARK
benchmark: bin/benchmark/build_suffix_array
	./bin/benchmark/build_suffix_array --benchmark_color=yes

bin/benchmark/build_suffix_array: benchmark/*
	@mkdir -p bin/benchmark
	$(CXX) $(CXXFLAGS) $(RELEASE) benchmark/build_suffix_array.cpp -o bin/benchmark/build_suffix_array

.PHONY: clean

clean:
	@rm -rf ./bin
