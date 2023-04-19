EXTERNAL_INCLUDES = -I ./dependencies/sux \
					-I ./dependencies/wyhash \
					-I ./dependencies/libdivsufsort/build/include \
					-I ./dependencies/libsais/build/include \
					-I ./dependencies/xxHash/build/include \
					-I ./dependencies/zlib/build/include \
					-I ./dependencies/sdsl-lite/include \
					-I ./dependencies/benchmark/installed/include \
					-I ./dependencies/folly/installed/folly/include \
					-I ./dependencies/folly/installed/fmt/include

EXTERNAL_STATIC_LIBS = $(shell pwd)/dependencies/libdivsufsort/build/lib/libdivsufsort.a \
					   $(shell pwd)/dependencies/libdivsufsort/build/lib/libdivsufsort64.a \
					   $(shell pwd)/dependencies/zlib/build/lib/libz.a \
					   $(shell pwd)/dependencies/xxHash/build/lib/libxxhash.a \
					   $(shell pwd)/dependencies/libsais/build/lib/libsais64.a \
					   $(shell pwd)/dependencies/libsais/build/lib/libsais.a \
					   $(shell pwd)/dependencies/benchmark/installed/lib64/libbenchmark.a \
					   $(shell pwd)/dependencies/folly/installed/folly/lib/libfolly.a \
					   $(shell pwd)/dependencies/folly/installed/fmt/lib64/libfmt.a

CXXFLAGS += -std=c++20 -march=native -mtune=native -fomit-frame-pointer -flto -fopenmp -Wall -Wextra -I ./ $(EXTERNAL_INCLUDES) $(EXTERNAL_SOURCES)
ifeq ($(CXX), clang++)
	# NOTE: https://clang.llvm.org/cxx_status.html#p0522
	CXXFLAGS += -frelaxed-template-template-args
endif
LDLIBS += -flto -Bstatic -lgtest -lpthread $(EXTERNAL_STATIC_LIBS) #-lbenchmark -lfolly
DEPENDENCIES = $(shell find . -name "*.[ch]pp")
DEBUG := -g3 -O3 # -DDEBUG
RELEASE := -O3 -DNDEBUG

SIGMA_T?=uint8_t
ALLOC_TYPE?=MALLOC
FIND_OP?=find_prefix

all: test benchmark util

TESTS = bin/test/random \
		bin/test/hash   \
		bin/test/saca   \
		bin/test/zuffix \

BENCHMARKS = bin/benchmark/lambda          \
			 bin/benchmark/saca            \
			 bin/benchmark/hash_block_size \
			 bin/benchmark/hash            \
			 bin/benchmark/interactive     \
			 bin/benchmark/fibonacci       \
			 bin/benchmark/findrandom      \
			 bin/benchmark/nofindrandom    \
			 bin/benchmark/findfile        \
			 bin/benchmark/sdsl            \
			 #bin/benchmark/build           \

UTILS = bin/util/generate_random_string \
		bin/util/generate_fibonacci_string

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

# UTIL
util: $(UTILS)

bin/util/generate_random_string: util/generate_random_string.cpp $(DEPENDENCIES)
	@mkdir -p bin/util
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@ $< $(LDLIBS)

bin/util/generate_fibonacci_string: util/generate_fibonacci_string.cpp $(DEPENDENCIES)
	@mkdir -p bin/util
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@ $< $(LDLIBS)

# BENCHMARK
benchmark: $(BENCHMARKS)
	# ./bin/benchmark/lambda --benchmark_color=yes
	# ./bin/benchmark/saca --benchmark_color=yes
	# ./bin/benchmark/hash_block_size --benchmark_color=yes
	# ./bin/benchmark/hash --benchmark_color=yes
	# ./bin/benchmark/fibonacci --benchmark_color=yes

bin/benchmark/saca: benchmark/saca.cpp $(DEPENDENCIES)
	@mkdir -p bin/benchmark
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@ $< $(LDLIBS)

bin/benchmark/lambda: benchmark/lambda.cpp $(DEPENDENCIES)
	@mkdir -p bin/benchmark
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@ $< $(LDLIBS)

bin/benchmark/hash_block_size: benchmark/hash_block_size.cpp $(DEPENDENCIES)
	@mkdir -p bin/benchmark
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@ $< $(LDLIBS) -DALLOC_TYPE=$(ALLOC_TYPE)

bin/benchmark/hash: benchmark/hash.cpp $(DEPENDENCIES)
	@mkdir -p bin/benchmark
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@ $< $(LDLIBS)

bin/benchmark/interactive: benchmark/interactive.cpp $(DEPENDENCIES)
	@mkdir -p $@
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/simple                        $< $(LDLIBS) -DFIND_OP=find       -DSTRUCTURE_T=SimpleSuffixArray\<char\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/enhanced                      $< $(LDLIBS) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=EnhancedSuffixArray\<char\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/memcmp-zuffix-xxh3            $< $(LDLIBS) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=MemcmpZuffixArray\<char\,XXH3Hash\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/memcmp-zuffix-wyhash          $< $(LDLIBS) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=MemcmpZuffixArray\<char\,WyHash\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/memcmp-zuffix-crc32zlib       $< $(LDLIBS) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=MemcmpZuffixArray\<char\,CRC32ZlibHash\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/memcmp-zuffix-crc32cfolly     $< $(LDLIBS) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=MemcmpZuffixArray\<char\,CRC32CFollyHash\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/memcmp-zuffix-crc32+crc32c    $< $(LDLIBS) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=MemcmpZuffixArray\<char\,CRC32Plus32CFollyHash\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/signature-zuffix-xxh3         $< $(LDLIBS) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=SignatureZuffixArray\<char\,XXH3Hash\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/signature-zuffix-wyhash       $< $(LDLIBS) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=SignatureZuffixArray\<char\,WyHash\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/signature-zuffix-crc32zlib    $< $(LDLIBS) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=SignatureZuffixArray\<char\,CRC32ZlibHash\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/signature-zuffix-crc32cfolly  $< $(LDLIBS) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=SignatureZuffixArray\<char\,CRC32CFollyHash\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/signature-zuffix-crc32+crc32c $< $(LDLIBS) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=SignatureZuffixArray\<char\,CRC32Plus32CFollyHash\>

bin/benchmark/fibonacci: benchmark/fibonacci.cpp $(DEPENDENCIES)
	@mkdir -p $@
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/simple                        $< $(LDLIBS) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=find       -DSTRUCTURE_T=SimpleSuffixArray\<char,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/enhanced                      $< $(LDLIBS) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=EnhancedSuffixArray\<char,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/memcmp-zuffix-xxh3            $< $(LDLIBS) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=MemcmpZuffixArray\<char\,XXH3Hash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/memcmp-zuffix-wyhash          $< $(LDLIBS) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=MemcmpZuffixArray\<char\,WyHash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/memcmp-zuffix-crc32zlib       $< $(LDLIBS) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=MemcmpZuffixArray\<char\,CRC32ZlibHash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/memcmp-zuffix-crc32cfolly     $< $(LDLIBS) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=MemcmpZuffixArray\<char\,CRC32CFollyHash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/memcmp-zuffix-crc32+crc32c    $< $(LDLIBS) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=MemcmpZuffixArray\<char\,CRC32Plus32CFollyHash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/signature-zuffix-xxh3         $< $(LDLIBS) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=SignatureZuffixArray\<char\,XXH3Hash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/signature-zuffix-wyhash       $< $(LDLIBS) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=SignatureZuffixArray\<char\,WyHash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/signature-zuffix-crc32zlib    $< $(LDLIBS) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=SignatureZuffixArray\<char\,CRC32ZlibHash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/signature-zuffix-crc32cfolly  $< $(LDLIBS) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=SignatureZuffixArray\<char\,CRC32CFollyHash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/signature-zuffix-crc32+crc32c $< $(LDLIBS) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=SignatureZuffixArray\<char\,CRC32Plus32CFollyHash,ALLOC_TYPE\>

bin/benchmark/findfile: benchmark/findfile.cpp benchmark/findfile_errors.cpp $(DEPENDENCIES)
	@mkdir -p $@
	#$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/errors benchmark/findfile_errors.cpp $(LDLIBS) -DSIGMA_T=SIGMA_T
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/simple                        $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=find       -DSTRUCTURE_T=SimpleSuffixArray\<SIGMA_T,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/enhanced                      $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=EnhancedSuffixArray\<SIGMA_T,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/memcmp-zuffix-xxh3            $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=MemcmpZuffixArray\<SIGMA_T\,XXH3Hash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/memcmp-zuffix-wyhash          $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=MemcmpZuffixArray\<SIGMA_T\,WyHash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/memcmp-zuffix-crc32zlib       $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=MemcmpZuffixArray\<SIGMA_T\,CRC32ZlibHash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/memcmp-zuffix-crc32cfolly     $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=MemcmpZuffixArray\<SIGMA_T\,CRC32CFollyHash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/memcmp-zuffix-crc32+crc32c    $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=MemcmpZuffixArray\<SIGMA_T\,CRC32Plus32CFollyHash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/signature-zuffix-xxh3         $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=SignatureZuffixArray\<SIGMA_T\,XXH3Hash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/signature-zuffix-wyhash       $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=SignatureZuffixArray\<SIGMA_T\,WyHash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/signature-zuffix-crc32zlib    $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=SignatureZuffixArray\<SIGMA_T\,CRC32ZlibHash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/signature-zuffix-crc32cfolly  $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=SignatureZuffixArray\<SIGMA_T\,CRC32CFollyHash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/signature-zuffix-crc32+crc32c $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=SignatureZuffixArray\<SIGMA_T\,CRC32Plus32CFollyHash,ALLOC_TYPE\>

bin/benchmark/findrandom: benchmark/findrandom.cpp $(DEPENDENCIES)
	@mkdir -p $@
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/simple                        $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=find       -DSTRUCTURE_T=SimpleSuffixArray\<SIGMA_T,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/enhanced                      $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=EnhancedSuffixArray\<SIGMA_T,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/memcmp-zuffix-xxh3            $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=MemcmpZuffixArray\<SIGMA_T\,XXH3Hash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/memcmp-zuffix-wyhash          $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=MemcmpZuffixArray\<SIGMA_T\,WyHash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/memcmp-zuffix-crc32zlib       $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=MemcmpZuffixArray\<SIGMA_T\,CRC32ZlibHash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/memcmp-zuffix-crc32cfolly     $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=MemcmpZuffixArray\<SIGMA_T\,CRC32CFollyHash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/memcmp-zuffix-crc32+crc32c    $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=MemcmpZuffixArray\<SIGMA_T\,CRC32Plus32CFollyHash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/signature-zuffix-xxh3         $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=SignatureZuffixArray\<SIGMA_T\,XXH3Hash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/signature-zuffix-wyhash       $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=SignatureZuffixArray\<SIGMA_T\,WyHash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/signature-zuffix-crc32zlib    $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=SignatureZuffixArray\<SIGMA_T\,CRC32ZlibHash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/signature-zuffix-crc32cfolly  $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=SignatureZuffixArray\<SIGMA_T\,CRC32CFollyHash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/signature-zuffix-crc32+crc32c $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DFIND_OP=$(FIND_OP) -DSTRUCTURE_T=SignatureZuffixArray\<SIGMA_T\,CRC32Plus32CFollyHash,ALLOC_TYPE\>

bin/benchmark/nofindrandom: benchmark/nofindrandom.cpp $(DEPENDENCIES)
	@mkdir -p $@
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/simple                        $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DSTRUCTURE_T=SimpleSuffixArray\<SIGMA_T,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/enhanced                      $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DSTRUCTURE_T=EnhancedSuffixArray\<SIGMA_T,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/memcmp-zuffix-xxh3            $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DSTRUCTURE_T=MemcmpZuffixArray\<SIGMA_T\,XXH3Hash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/memcmp-zuffix-wyhash          $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DSTRUCTURE_T=MemcmpZuffixArray\<SIGMA_T\,WyHash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/memcmp-zuffix-crc32zlib       $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DSTRUCTURE_T=MemcmpZuffixArray\<SIGMA_T\,CRC32ZlibHash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/memcmp-zuffix-crc32cfolly     $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DSTRUCTURE_T=MemcmpZuffixArray\<SIGMA_T\,CRC32CFollyHash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/memcmp-zuffix-crc32+crc32c    $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DSTRUCTURE_T=MemcmpZuffixArray\<SIGMA_T\,CRC32Plus32CFollyHash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/signature-zuffix-xxh3         $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DSTRUCTURE_T=SignatureZuffixArray\<SIGMA_T\,XXH3Hash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/signature-zuffix-wyhash       $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DSTRUCTURE_T=SignatureZuffixArray\<SIGMA_T\,WyHash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/signature-zuffix-crc32zlib    $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DSTRUCTURE_T=SignatureZuffixArray\<SIGMA_T\,CRC32ZlibHash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/signature-zuffix-crc32cfolly  $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DSTRUCTURE_T=SignatureZuffixArray\<SIGMA_T\,CRC32CFollyHash,ALLOC_TYPE\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/signature-zuffix-crc32+crc32c $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DSTRUCTURE_T=SignatureZuffixArray\<SIGMA_T\,CRC32Plus32CFollyHash,ALLOC_TYPE\>


bin/benchmark/sdsl: benchmark/sdsl.cpp $(DEPENDENCIES)
	@mkdir -p $@
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/sct3-zfast-backward-crc32cfolly     $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DSDSL_COUNT_OP=zfast_count_backward  -DSDSL_STRUCTURE_T=MemcmpZSdsl\<sdsl_sct3,SIGMA_T\,CRC32CFollyHash\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/sct3-backward-crc32cfolly     	$< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DSDSL_COUNT_OP=count_backward   	-DSDSL_STRUCTURE_T=MemcmpZSdsl\<sdsl_sct3,SIGMA_T\,CRC32CFollyHash\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/sct3-zfast-forward-crc32cfolly      $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DSDSL_COUNT_OP=zfast_count_forward  	-DSDSL_STRUCTURE_T=MemcmpZSdsl\<sdsl_sct3,SIGMA_T\,CRC32CFollyHash\>
	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/sct3-forward-crc32cfolly     	$< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DSDSL_COUNT_OP=count_forward 	-DSDSL_STRUCTURE_T=MemcmpZSdsl\<sdsl_sct3,SIGMA_T\,CRC32CFollyHash\>
	#$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/memcmp-zuffix-xxh3            $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DSDSL_COUNT_OP=$(SDSL_COUNT_OP) -DSDSL_STRUCTURE_T=MemcmpZSdsl\<sdsl_sct3,SIGMA_T\,XXH3Hash\>
	#$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/memcmp-zuffix-wyhash          $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DSDSL_COUNT_OP=$(SDSL_COUNT_OP) -DSDSL_STRUCTURE_T=MemcmpZSdsl\<sdsl_sct3,SIGMA_T\,WyHash\>
	#$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/memcmp-zuffix-crc32zlib       $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DSDSL_COUNT_OP=$(SDSL_COUNT_OP) -DSDSL_STRUCTURE_T=MemcmpZSdsl\<sdsl_sct3,SIGMA_T\,CRC32ZlibHash\>
	#$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/memcmp-zuffix-crc32cfolly     $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DSDSL_COUNT_OP=$(SDSL_COUNT_OP) -DSDSL_STRUCTURE_T=MemcmpZSdsl\<sdsl_sct3,SIGMA_T\,CRC32CFollyHash\>
	#$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/memcmp-zuffix-crc32+crc32c    $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DSDSL_COUNT_OP=$(SDSL_COUNT_OP) -DSDSL_STRUCTURE_T=MemcmpZSdsl\<sdsl_sct3,SIGMA_T\,CRC32Plus32CFollyHash\>

# TODO Test operator<< and operator>> they are likely wrong.
# For now this is only (slightly) useful to benchmark construction time, not to dump/load the structure into/from a file.
# bin/benchmark/build: benchmark/build.cpp $(DEPENDENCIES)
# 	@mkdir -p $@
# 	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/simple                        $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DSTRUCTURE_T=SimpleSuffixArray\<SIGMA_T,ALLOC_TYPE\>
# 	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/enhanced                      $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DSTRUCTURE_T=EnhancedSuffixArray\<SIGMA_T,ALLOC_TYPE\>
# 	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/memcmp-zuffix-xxh3            $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DSTRUCTURE_T=MemcmpZuffixArray\<SIGMA_T\,XXH3Hash,ALLOC_TYPE\>
# 	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/memcmp-zuffix-wyhash          $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DSTRUCTURE_T=MemcmpZuffixArray\<SIGMA_T\,WyHash,ALLOC_TYPE\>
# 	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/memcmp-zuffix-crc32zlib       $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DSTRUCTURE_T=MemcmpZuffixArray\<SIGMA_T\,CRC32ZlibHash,ALLOC_TYPE\>
# 	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/memcmp-zuffix-crc32cfolly     $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DSTRUCTURE_T=MemcmpZuffixArray\<SIGMA_T\,CRC32CFollyHash,ALLOC_TYPE\>
# 	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/memcmp-zuffix-crc32+crc32c    $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DSTRUCTURE_T=MemcmpZuffixArray\<SIGMA_T\,CRC32Plus32CFollyHash,ALLOC_TYPE\>
# 	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/signature-zuffix-xxh3         $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DSTRUCTURE_T=SignatureZuffixArray\<SIGMA_T\,XXH3Hash,ALLOC_TYPE\>
# 	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/signature-zuffix-wyhash       $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DSTRUCTURE_T=SignatureZuffixArray\<SIGMA_T\,WyHash,ALLOC_TYPE\>
# 	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/signature-zuffix-crc32zlib    $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DSTRUCTURE_T=SignatureZuffixArray\<SIGMA_T\,CRC32ZlibHash,ALLOC_TYPE\>
# 	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/signature-zuffix-crc32cfolly  $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DSTRUCTURE_T=SignatureZuffixArray\<SIGMA_T\,CRC32CFollyHash,ALLOC_TYPE\>
# 	$(CXX) $(CXXFLAGS) $(RELEASE) -o $@/signature-zuffix-crc32+crc32c $< $(LDLIBS) -DSIGMA_T=$(SIGMA_T) -DALLOC_TYPE=$(ALLOC_TYPE) -DSTRUCTURE_T=SignatureZuffixArray\<SIGMA_T\,CRC32Plus32CFollyHash,ALLOC_TYPE\>

.PHONY: clean

clean:
	@rm -rf ./bin
