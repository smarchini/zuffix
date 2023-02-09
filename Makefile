CXXFLAGS += -std=c++17 -fno-omit-frame-pointer -march=native -mtune=native -fopenmp -Wall -Wextra -I ./
ifeq ($(CXX), clang++)
	# NOTE: https://clang.llvm.org/cxx_status.html#p0522
	CXXFLAGS += -frelaxed-template-template-args
endif
LIBFOLLY := -lfolly -lfmt -lboost_context -lboost_filesystem -lboost_program_options -ldouble-conversion -lgflags -lglog -levent -lz -lssl -lcrypto -llzma -llz4 -lzstd -lunwind -lrt -lpthread -lboost_atomic -ldl
LDLIBS += -lgtest -lbenchmark -lpthread -ldivsufsort64 -lsais64 -lxxhash -lz -l:libsais.a $(LIBFOLLY)
DEPENDENCIES = $(shell find . -name "*.[ch]pp")
DEBUG := -g3 -O0
RELEASE := -g3 -O3

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
			 bin/benchmark/build        \

# TEST
test: $(TESTS)
	# ./bin/test/random --gtest_color=yes
	# ./bin/test/hash --gtest_color=yes
	# ./bin/test/saca --gtest_color=yes
	# ./bin/test/zuffix --gtest_color=yes

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
	# ./bin/benchmark/lambda --benchmark_color=yes
	# ./bin/benchmark/saca --benchmark_color=yes
	# ./bin/benchmark/hash_block_size --benchmark_color=yes
	# ./bin/benchmark/hash --benchmark_color=yes
	# ./bin/benchmark/find_random --benchmark_color=yes
	# ./bin/benchmark/fibonacci --benchmark_color=yes

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
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/zuffix-xxh3 $< $(LDLIBS) -DSYMBOLTYPE=uint8_t -DDATASTRUCTURETYPE=ExactZuffixArray\<SYMBOLTYPE\,XXH3Hash\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/zuffix-crc32zlib $< $(LDLIBS) -DSYMBOLTYPE=uint8_t -DDATASTRUCTURETYPE=ExactZuffixArray\<SYMBOLTYPE\,CRC32ZlibHash\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/zuffix-crc32folly $< $(LDLIBS) -DSYMBOLTYPE=uint8_t -DDATASTRUCTURETYPE=ExactZuffixArray\<SYMBOLTYPE\,CRC32FollyHash\>

# TODO Test operator<< and operator>> they are likely wrong.
# For now this is only useful to benchmark construction time.
bin/benchmark/build: benchmark/build.cpp $(objects)
	@mkdir -p $@
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/simple $< $(LDLIBS) -DSYMBOLTYPE=uint8_t -DDATASTRUCTURETYPE=SimpleSuffixArray\<SYMBOLTYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/enhanced $< $(LDLIBS) -DSYMBOLTYPE=uint8_t -DDATASTRUCTURETYPE=EnhancedSuffixArray\<SYMBOLTYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/zuffix-xxh3 $< $(LDLIBS) -DSYMBOLTYPE=uint8_t -DDATASTRUCTURETYPE=ExactZuffixArray\<SYMBOLTYPE\,XXH3Hash\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/zuffix-crc32zlib $< $(LDLIBS) -DSYMBOLTYPE=uint8_t -DDATASTRUCTURETYPE=ExactZuffixArray\<SYMBOLTYPE\,CRC32ZlibHash\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/zuffix-crc32folly $< $(LDLIBS) -DSYMBOLTYPE=uint8_t -DDATASTRUCTURETYPE=ExactZuffixArray\<SYMBOLTYPE\,CRC32FollyHash\>

.PHONY: clean

clean:
	@rm -rf ./bin
