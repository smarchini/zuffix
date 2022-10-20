CXX = g++
DEBUG = -g -O0
RELEASE = -g -O3
CXXFLAGS = -std=c++17 -Wall -Wextra -march=native -mavx -mavx2 -msse2 -msse4.2 -fopenmp -funroll-loops -fno-omit-frame-pointer -I./ 
LDLIBS=-lgtest -lbenchmark -lpthread -ldivsufsort64 -lsais -lsais64 -l:libsais.a -l:libsais64.a -l:libxxhash.a

all: test benchmark

# TEST
test: bin/test/random bin/test/hash bin/test/saca bin/test/zuffix
	./bin/test/random --gtest_color=yes
	./bin/test/hash --gtest_color=yes
	./bin/test/saca --gtest_color=yes
	./bin/test/zuffix --gtest_color=yes

bin/test/random: test/random/* zuffix/* zuffix/*/*
	@mkdir -p bin/test
	$(CXX) $(CXXFLAGS) $(DEBUG) test/random/test.cpp -o bin/test/random $(LDLIBS)

bin/test/hash: test/hash/* zuffix/* zuffix/*/*
	@mkdir -p bin/test
	$(CXX) $(CXXFLAGS) $(DEBUG) test/hash/test.cpp -o bin/test/hash $(LDLIBS)

bin/test/saca: test/saca/* zuffix/* zuffix/*/*
	@mkdir -p bin/test
	$(CXX) $(CXXFLAGS) $(DEBUG) test/saca/test.cpp -o bin/test/saca $(LDLIBS)

bin/test/zuffix: test/zuffix/* zuffix/* zuffix/*/*
	@mkdir -p bin/test
	$(CXX) $(CXXFLAGS) $(DEBUG) test/zuffix/test.cpp -o bin/test/zuffix $(LDLIBS)

# BENCHMARK
benchmark: bin/benchmark/lambda bin/benchmark/saca bin/benchmark/hash_block_size bin/benchmark/hash bin/benchmark/find_random bin/benchmark/fibonacci bin/benchmark/build bin/benchmark/findfile bin/benchmark/interactive
	./bin/benchmark/lambda --benchmark_color=yes
	./bin/benchmark/saca --benchmark_color=yes
	./bin/benchmark/hash_block_size --benchmark_color=yes
	./bin/benchmark/hash --benchmark_color=yes
	./bin/benchmark/find_random --benchmark_color=yes
	./bin/benchmark/fibonacci --benchmark_color=yes

bin/benchmark/saca: benchmark/*  zuffix/* zuffix/*/*
	@mkdir -p bin/benchmark
	$(CXX) $(CXXFLAGS) $(RELEASE) benchmark/saca.cpp -o bin/benchmark/saca $(LDLIBS)

bin/benchmark/lambda: benchmark/*  zuffix/* zuffix/*/*
	@mkdir -p bin/benchmark
	$(CXX) $(CXXFLAGS) $(RELEASE) benchmark/lambda.cpp -o bin/benchmark/lambda $(LDLIBS)

bin/benchmark/hash_block_size: benchmark/*  zuffix/* zuffix/*/*
	@mkdir -p bin/benchmark
	$(CXX) $(CXXFLAGS) $(RELEASE) benchmark/hash_block_size.cpp -o bin/benchmark/hash_block_size $(LDLIBS)

bin/benchmark/hash: benchmark/*  zuffix/* zuffix/*/*
	@mkdir -p bin/benchmark
	$(CXX) $(CXXFLAGS) $(RELEASE) benchmark/hash.cpp -o bin/benchmark/hash $(LDLIBS)

bin/benchmark/find_random: benchmark/*  zuffix/* zuffix/*/*
	@mkdir -p bin/benchmark
	$(CXX) $(CXXFLAGS) $(RELEASE) benchmark/find_random.cpp -o bin/benchmark/find_random $(LDLIBS)

bin/benchmark/fibonacci: benchmark/*  zuffix/* zuffix/*/*
	@mkdir -p bin/benchmark
	$(CXX) $(CXXFLAGS) $(RELEASE)  benchmark/fibonacci.cpp -o bin/benchmark/fibonacci $(LDLIBS)

bin/benchmark/build: benchmark/*  zuffix/* zuffix/*/*
	@mkdir -p bin/benchmark/build8
	$(CXX) $(CXXFLAGS) $(RELEASE) -DSYMBOLTYPE=uint8_t -DDATASTRUCTURETYPE=SimpleSuffixArray\<SYMBOLTYPE\> benchmark/build.cpp -o bin/benchmark/build8/SimpleSuffixArray $(LDLIBS)
	$(CXX) $(CXXFLAGS) $(RELEASE) -DSYMBOLTYPE=uint8_t -DDATASTRUCTURETYPE=EnhancedSuffixArray\<SYMBOLTYPE\> benchmark/build.cpp -o bin/benchmark/build8/EnhancedSuffixArray $(LDLIBS)
	$(CXX) $(CXXFLAGS) $(RELEASE) -DSYMBOLTYPE=uint8_t -DDATASTRUCTURETYPE=ZuffixArray\<SYMBOLTYPE\,RabinKarpHash\> benchmark/build.cpp -o bin/benchmark/build8/ZuffixArray_RabinKarp $(LDLIBS)
	$(CXX) $(CXXFLAGS) $(RELEASE) -DSYMBOLTYPE=uint8_t -DDATASTRUCTURETYPE=ZuffixArray\<SYMBOLTYPE\,CyclicPoly4Hash\> benchmark/build.cpp -o bin/benchmark/build8/ZuffixArray_CyclicPoly4 $(LDLIBS)
	$(CXX) $(CXXFLAGS) $(RELEASE) -DSYMBOLTYPE=uint8_t -DDATASTRUCTURETYPE=ZuffixArray\<SYMBOLTYPE\,CyclicPoly128Hash\> benchmark/build.cpp -o bin/benchmark/build8/ZuffixArray_CyclicPoly128 $(LDLIBS)
	$(CXX) $(CXXFLAGS) $(RELEASE) -DSYMBOLTYPE=uint8_t -DDATASTRUCTURETYPE=ZuffixArray\<SYMBOLTYPE\,O1Hash\> benchmark/build.cpp -o bin/benchmark/build8/ZuffixArray_O1 $(LDLIBS)
	$(CXX) $(CXXFLAGS) $(RELEASE) -DSYMBOLTYPE=uint8_t -DDATASTRUCTURETYPE=ZuffixArray\<SYMBOLTYPE\,XXH3Hash\> benchmark/build.cpp -o bin/benchmark/build8/ZuffixArray_XXH3 $(LDLIBS)

bin/benchmark/findfile: benchmark/*  zuffix/* zuffix/*/*
    # 8 bit
	@mkdir -p bin/benchmark/find8
	$(CXX) $(CXXFLAGS) $(RELEASE) -DSYMBOLTYPE=uint8_t -DDATASTRUCTURETYPE=SimpleSuffixArray\<SYMBOLTYPE\> benchmark/findfile.cpp -o bin/benchmark/find8/SimpleSuffixArray $(LDLIBS)
	$(CXX) $(CXXFLAGS) $(RELEASE) -DSYMBOLTYPE=uint8_t -DDATASTRUCTURETYPE=EnhancedSuffixArray\<SYMBOLTYPE\> benchmark/findfile.cpp -o bin/benchmark/find8/EnhancedSuffixArray $(LDLIBS)
	$(CXX) $(CXXFLAGS) $(RELEASE) -DSYMBOLTYPE=uint8_t -DDATASTRUCTURETYPE=ZuffixArray\<SYMBOLTYPE\,RabinKarpHash\> benchmark/findfile.cpp -o bin/benchmark/find8/ZuffixArray_RabinKarp $(LDLIBS)
	$(CXX) $(CXXFLAGS) $(RELEASE) -DSYMBOLTYPE=uint8_t -DDATASTRUCTURETYPE=ZuffixArray\<SYMBOLTYPE\,CyclicPoly4Hash\> benchmark/findfile.cpp -o bin/benchmark/find8/ZuffixArray_CyclicPoly4 $(LDLIBS)
	$(CXX) $(CXXFLAGS) $(RELEASE) -DSYMBOLTYPE=uint8_t -DDATASTRUCTURETYPE=ZuffixArray\<SYMBOLTYPE\,CyclicPoly128Hash\> benchmark/findfile.cpp -o bin/benchmark/find8/ZuffixArray_CyclicPoly128 $(LDLIBS)
	$(CXX) $(CXXFLAGS) $(RELEASE) -DSYMBOLTYPE=uint8_t -DDATASTRUCTURETYPE=ZuffixArray\<SYMBOLTYPE\,O1Hash\> benchmark/findfile.cpp -o bin/benchmark/find8/ZuffixArray_O1 $(LDLIBS)
	$(CXX) $(CXXFLAGS) $(RELEASE) -DSYMBOLTYPE=uint8_t -DDATASTRUCTURETYPE=ZuffixArray\<SYMBOLTYPE\,XXH3Hash\> benchmark/findfile.cpp -o bin/benchmark/find8/ZuffixArray_XXH3 $(LDLIBS)

bin/benchmark/interactive: benchmark/*  zuffix/* zuffix/*/*
	@mkdir -p bin/benchmark
	$(CXX) $(CXXFLAGS) $(RELEASE) benchmark/interactive.cpp -o bin/benchmark/interactive $(LDLIBS)

# DATASET
dataset: dataset/text dataset/patterns

dataset/text:
	@mkdir -p dataset/gzip
	wget -P ./dataset/gzip "http://pizzachili.dcc.uchile.cl/texts/dna/dna.gz"
	wget -P ./dataset/gzip "http://pizzachili.dcc.uchile.cl/texts/dna/dna.50MB.gz"
	wget -P ./dataset/gzip "http://pizzachili.dcc.uchile.cl/texts/dna/dna.100MB.gz"
	wget -P ./dataset/gzip "http://pizzachili.dcc.uchile.cl/texts/dna/dna.200MB.gz"
    wget -P ./dataset/gzip "http://pizzachili.dcc.uchile.cl/texts/code/sources.200MB.gz"
	@mkdir -p dataset/text
	aunpack -X ./dataset/text/ -e ./dataset/gzip/*

dataset/patterns: dataset/text
	@mkdir -p ./dataset/pattern
# dna
	genpatterns ./dataset/text/dna 1 100 ./dataset/pattern/dna.1 || true
	genpatterns ./dataset/text/dna 1000 100 ./dataset/pattern/dna.1000 || true
# dna.50MB
	genpatterns ./dataset/text/dna.50MB 1 100 ./dataset/pattern/dna.50MB.1 || true
	genpatterns ./dataset/text/dna.50MB 10 100 ./dataset/pattern/dna.50MB.10 || true
	genpatterns ./dataset/text/dna.50MB 1000000 100 ./dataset/pattern/dna.50MB.1000000 || true
	genpatterns ./dataset/text/dna.50MB 10000000 100 ./dataset/pattern/dna.50MB.10000000 || true
	genpatterns ./dataset/text/dna.50MB 50000000 100 ./dataset/pattern/dna.50MB.50000000 || true
# dna.100MB
	genpatterns ./dataset/text/dna.100MB 1 100 ./dataset/pattern/dna.100MB.1 || true
	genpatterns ./dataset/text/dna.100MB 10 100 ./dataset/pattern/dna.100MB.10 || true
	genpatterns ./dataset/text/dna.100MB 1000000 100 ./dataset/pattern/dna.100MB.1000000 || true
	genpatterns ./dataset/text/dna.100MB 10000000 100 ./dataset/pattern/dna.100MB.10000000 || true
	genpatterns ./dataset/text/dna.100MB 100000000 100 ./dataset/pattern/dna.100MB.100000000 || true
# dna.200MB
	genpatterns ./dataset/text/dna.200MB 1 100 ./dataset/pattern/dna.200MB.1 || true
	genpatterns ./dataset/text/dna.200MB 10 100 ./dataset/pattern/dna.200MB.10 || true
	genpatterns ./dataset/text/dna.200MB 1000000 100 ./dataset/pattern/dna.200MB.1000000 || true
	genpatterns ./dataset/text/dna.200MB 10000000 100 ./dataset/pattern/dna.200MB.10000000 || true
	genpatterns ./dataset/text/dna.200MB 200000000 100 ./dataset/pattern/dna.200MB.200000000 || true

dataset/zuffix: bin/benchmark/build dataset/text
	@mkdir -p dataset/zuffix8
	# simple
	./bin/benchmark/build8/SimpleSuffixArray ./dataset/text/dna > ./dataset/zuffix8/dna.simple
	./bin/benchmark/build8/SimpleSuffixArray ./dataset/text/dna.50MB > ./dataset/zuffix8/dna.50MB.simple
	./bin/benchmark/build8/SimpleSuffixArray ./dataset/text/dna.100MB > ./dataset/zuffix8/dna.100MB.simple
	./bin/benchmark/build8/SimpleSuffixArray ./dataset/text/dna.200MB > ./dataset/zuffix8/dna.200MB.simple
	./bin/benchmark/build8/SimpleSuffixArray ./dataset/text/sources.200MB > ./dataset/zuffix8/sources.200MB.simple
	# enhanced
	./bin/benchmark/build8/EnhancedSuffixArray ./dataset/text/dna > ./dataset/zuffix8/dna.enhanced
	./bin/benchmark/build8/EnhancedSuffixArray ./dataset/text/dna.50MB > ./dataset/zuffix8/dna.50MB.enhanced
	./bin/benchmark/build8/EnhancedSuffixArray ./dataset/text/dna.100MB > ./dataset/zuffix8/dna.100MB.enhanced
	./bin/benchmark/build8/EnhancedSuffixArray ./dataset/text/dna.200MB > ./dataset/zuffix8/dna.200MB.enhanced
	./bin/benchmark/build8/EnhancedSuffixArray ./dataset/text/sources.200MB > ./dataset/zuffix8/sources.200MB.enhanced
	# zuffix.xxh3
	./bin/benchmark/build8/ZuffixArray_XXH3 ./dataset/text/dna > ./dataset/zuffix8/dna.zuffix.xxh3
	./bin/benchmark/build8/ZuffixArray_XXH3 ./dataset/text/dna.50MB > ./dataset/zuffix8/dna.50MB.zuffix.xxh3
	./bin/benchmark/build8/ZuffixArray_XXH3 ./dataset/text/dna.100MB > ./dataset/zuffix8/dna.100MB.zuffix.xxh3
	./bin/benchmark/build8/ZuffixArray_XXH3 ./dataset/text/dna.200MB > ./dataset/zuffix8/dna.200MB.zuffix.xxh3
	./bin/benchmark/build8/ZuffixArray_XXH3 ./dataset/text/sources.200MB > ./dataset/zuffix8/sources.200MB.zuffix.xxh3

bear: bin/test/random bin/test/hash bin/test/saca bin/test/zuffix bin/benchmark/saca bin/benchmark/hash bin/benchmark/find_random bin/benchmark/fibonacci bin/benchmark/findfile

.PHONY: clean dataset

clean:
	@rm -rf ./bin
	#@rm -rf ./dataset
