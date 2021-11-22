DEBUG = -g -O0
RELEASE = -g -O3 -fno-omit-frame-pointer -mavx -mavx2 -msse2 -fopenmp -funroll-loops 
CXXFLAGS = -std=c++17 -Wall -Wextra -march=native -I./  #-I./external/sux/ -I./external/ -I./
LDLIBS=-lgtest -lbenchmark -lpthread -ldivsufsort64 -lxxhash

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
benchmark: bin/benchmark/saca bin/benchmark/hash bin/benchmark/find_random_dna bin/benchmark/fibonacci bin/benchmark/findfile
	./bin/benchmark/saca --benchmark_color=yes
	./bin/benchmark/hash --benchmark_color=yes
	./bin/benchmark/find_random_dna --benchmark_color=yes
	./bin/benchmark/fibonacci --benchmark_color=yes

bin/benchmark/saca: benchmark/*  zuffix/* zuffix/*/*
	@mkdir -p bin/benchmark
	$(CXX) $(CXXFLAGS) $(RELEASE) benchmark/saca.cpp -o bin/benchmark/saca $(LDLIBS)

bin/benchmark/hash: benchmark/*  zuffix/* zuffix/*/*
	@mkdir -p bin/benchmark
	$(CXX) $(CXXFLAGS) $(DEBUG) benchmark/hash.cpp -o bin/benchmark/hash $(LDLIBS)

bin/benchmark/find_random_dna: benchmark/*  zuffix/* zuffix/*/*
	@mkdir -p bin/benchmark
	$(CXX) $(CXXFLAGS) $(DEBUG) benchmark/find_random_dna.cpp -o bin/benchmark/find_random_dna $(LDLIBS)

bin/benchmark/fibonacci: benchmark/*  zuffix/* zuffix/*/*
	@mkdir -p bin/benchmark
	$(CXX) $(CXXFLAGS) $(RELEASE)  benchmark/fibonacci.cpp -o bin/benchmark/fibonacci $(LDLIBS)

bin/benchmark/findfile: benchmark/*  zuffix/* zuffix/*/*
    # 8 bit
	@mkdir -p bin/benchmark/find8
	$(CXX) $(CXXFLAGS) $(RELEASE) -DSYMBOLTYPE=uint8_t -DDATASTRUCTURETYPE=SimpleSuffixArray\<SYMBOLTYPE\> benchmark/findfile.cpp -o bin/benchmark/find8/SimpleSuffixArray $(LDLIBS)
	$(CXX) $(CXXFLAGS) $(RELEASE) -DSYMBOLTYPE=uint8_t -DDATASTRUCTURETYPE=EnhancedSuffixArray\<SYMBOLTYPE\> benchmark/findfile.cpp -o bin/benchmark/find8/EnhancedSuffixArray $(LDLIBS)
	$(CXX) $(CXXFLAGS) $(RELEASE) -DSYMBOLTYPE=uint8_t -DDATASTRUCTURETYPE=EnhancedZuffixArray\<SYMBOLTYPE\,RabinKarpHash\> benchmark/findfile.cpp -o bin/benchmark/find8/EnhancedZuffixArray_RabinKarp $(LDLIBS)
	$(CXX) $(CXXFLAGS) $(RELEASE) -DSYMBOLTYPE=uint8_t -DDATASTRUCTURETYPE=EnhancedZuffixArray\<SYMBOLTYPE\,CyclicPoly4Hash\> benchmark/findfile.cpp -o bin/benchmark/find8/EnhancedZuffixArray_CyclicPoly4 $(LDLIBS)
	$(CXX) $(CXXFLAGS) $(RELEASE) -DSYMBOLTYPE=uint8_t -DDATASTRUCTURETYPE=EnhancedZuffixArray\<SYMBOLTYPE\,CyclicPoly128Hash\> benchmark/findfile.cpp -o bin/benchmark/find8/EnhancedZuffixArray_CyclicPoly128 $(LDLIBS)

# DATASET
dataset: dataset/download dataset/text dataset/pattern
	@mkdir -p dataset
	aunpack -X ./dataset -e ./dataset/download/*

dataset/text:
	@mkdir -p dataset/gzip
	#wget -P ./dataset/gzip "http://pizzachili.dcc.uchile.cl/texts/dna/dna.gz"
	wget -P ./dataset/gzip "http://pizzachili.dcc.uchile.cl/texts/dna/dna.50MB.gz"
	#wget -P ./dataset/gzip "http://pizzachili.dcc.uchile.cl/texts/dna/dna.100MB.gz"
	#wget -P ./dataset/gzip "http://pizzachili.dcc.uchile.cl/texts/dna/dna.200MB.gz"
	aunpack -X ./dataset/text -e ./dataset/gzip/*

dataset/patterns: dataset/text
	@mkdir -p ./dataset/pattern
# dna
	#./pizzachili/genpatterns ./dataset/dna 1 100 ./pattern/dna.1 || true
	#./pizzachili/genpatterns ./dataset/dna 1000 100 ./pattern/dna.1000 || true
# dna.50MB
	genpatterns ./dataset/dna.50MB 1 100 ./pattern/dna.50MB.1 || true
	genpatterns ./dataset/dna.50MB 10 100 ./pattern/dna.50MB.10 || true
	genpatterns ./dataset/dna.50MB 1000000 100 ./pattern/dna.50MB.1000000 || true
	genpatterns ./dataset/dna.50MB 10000000 100 ./pattern/dna.50MB.10000000 || true
	genpatterns ./dataset/dna.50MB 50000000 100 ./pattern/dna.50MB.50000000 || true
# dna.100MB
	#./pizzachili/genpatterns ./dataset/dna.100MB 1 100 ./pattern/dna.100MB.1 || true
	#./pizzachili/genpatterns ./dataset/dna.100MB 10 100 ./pattern/dna.100MB.10 || true
	#./pizzachili/genpatterns ./dataset/dna.100MB 1000000 100 ./pattern/dna.100MB.1000000 || true
	#./pizzachili/genpatterns ./dataset/dna.100MB 10000000 100 ./pattern/dna.100MB.10000000 || true
	#./pizzachili/genpatterns ./dataset/dna.100MB 100000000 100 ./pattern/dna.100MB.100000000 || true
# dna.200MB
	#./pizzachili/genpatterns ./dataset/dna.200MB 1 100 ./pattern/dna.200MB.1 || true
	#./pizzachili/genpatterns ./dataset/dna.200MB 10 100 ./pattern/dna.200MB.10 || true
	#./pizzachili/genpatterns ./dataset/dna.200MB 1000000 100 ./pattern/dna.200MB.1000000 || true
	#./pizzachili/genpatterns ./dataset/dna.200MB 10000000 100 ./pattern/dna.200MB.10000000 || true
	#./pizzachili/genpatterns ./dataset/dna.200MB 200000000 100 ./pattern/dna.200MB.200000000 || true


.PHONY: clean dataset

clean:
	@rm -rf ./bin
	@rm -rf ./dataset
