# https://clang.llvm.org/cxx_status.html#p0522
CXX := clang++ -frelaxed-template-template-args -fno-omit-frame-pointer
CXXFLAGS += -std=c++17 -march=native -mtune=native -fopenmp -Wall -Wextra -I ./
LDLIBS += -lgtest -lbenchmark -lpthread -ldivsufsort64 -lsais64 -lxxhash -lz -l:libsais.a
DEPENDENCIES = $(shell find . -name "*.cpp" -name "*.hpp")
DEBUG := -g -O0
RELEASE := -g -O3

all: test benchmark

TESTS = bin/test/random \
		bin/test/hash   \
		bin/test/saca   \
		bin/test/zuffix \

BENCHMARKS = bin/benchmark/lambda          \
			 bin/benchmark/saca            \
			 bin/benchmark/hash_block_size \
			 bin/benchmark/hash            \
			 bin/benchmark/find_random     \
			 bin/benchmark/fibonacci       \
			 bin/benchmark/interactive     \
			 bin/benchmark/findfile        \

# TEST
test: $(TESTS)
	./bin/test/random --gtest_color=yes
	./bin/test/hash --gtest_color=yes
	./bin/test/saca --gtest_color=yes
	./bin/test/zuffix --gtest_color=yes

bin/test/random: test/random/test.cpp $(DEPENDENCIES)
	@mkdir -p bin/test
	$(CXX) $(CXXFLAGS) $(DEBUG) -o $@ $< $(LDLIBS)

bin/test/hash: test/hash/test.cpp $(DEPENDENCIES)
	@mkdir -p bin/test
	$(CXX) $(CXXFLAGS) $(DEBUG) -o $@ $< $(LDLIBS)

bin/test/saca: test/saca/test.cpp $(DEPENDENCIES)
	@mkdir -p bin/test
	$(CXX) $(CXXFLAGS) $(DEBUG) -o $@ $< $(LDLIBS)

bin/test/zuffix: test/zuffix/test.cpp $(DEPENDENCIES)
	@mkdir -p bin/test
	$(CXX) $(CXXFLAGS) $(DEBUG) -o $@ $< $(LDLIBS)

# BENCHMARK
benchmark: $(BENCHMARKS)
	./bin/benchmark/lambda --benchmark_color=yes
	./bin/benchmark/saca --benchmark_color=yes
	./bin/benchmark/hash_block_size --benchmark_color=yes
	./bin/benchmark/hash --benchmark_color=yes
	./bin/benchmark/find_random --benchmark_color=yes
	./bin/benchmark/fibonacci --benchmark_color=yes

bin/benchmark/saca: benchmark/saca.cpp $(DEPENDENCIES)
	@mkdir -p bin/benchmark
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@ $< $(LDLIBS)

bin/benchmark/lambda: benchmark/lambda.cpp $(DEPENDENCIES)
	@mkdir -p bin/benchmark
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@ $< $(LDLIBS)

bin/benchmark/hash_block_size: benchmark/hash_block_size.cpp $(DEPENDENCIES)
	@mkdir -p bin/benchmark
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@ $< $(LDLIBS)

bin/benchmark/hash: benchmark/hash.cpp $(DEPENDENCIES)
	@mkdir -p bin/benchmark
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@ $< $(LDLIBS)

bin/benchmark/find_random: benchmark/find_random.cpp $(DEPENDENCIES)
	@mkdir -p bin/benchmark
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@ $< $(LDLIBS)

bin/benchmark/fibonacci: benchmark/fibonacci.cpp $(DEPENDENCIES)
	@mkdir -p bin/benchmark
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@ $< $(LDLIBS)

bin/benchmark/interactive: benchmark/interactive.cpp  $(DEPENDENCIES)
	@mkdir -p bin/benchmark
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@ $< $(LDLIBS)

bin/benchmark/findfile: benchmark/findfile.cpp $(objects)
	@mkdir -p $@
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/simple $< $(LDLIBS) -DSYMBOLTYPE=uint8_t -DDATASTRUCTURETYPE=SimpleSuffixArray\<SYMBOLTYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/enhanced $< $(LDLIBS) -DSYMBOLTYPE=uint8_t -DDATASTRUCTURETYPE=EnhancedSuffixArray\<SYMBOLTYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/zuffix-xxh3 $< $(LDLIBS) -DSYMBOLTYPE=uint8_t -DDATASTRUCTURETYPE=ZuffixArray\<SYMBOLTYPE\,XXH3Hash\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/zuffix-crc32 $< $(LDLIBS) -DSYMBOLTYPE=uint8_t -DDATASTRUCTURETYPE=ZuffixArray\<SYMBOLTYPE\,CRC32ZlibHash\>

.PHONY: clean

clean:
	@rm -rf ./bin
