SUX = $(HOME)/src/sux/  # Change me
CXXFLAGS = -g -std=c++17 -Wall -Wextra -O0 -march=native -I./ -I$(SUX) -l gtest

all: test

test: bin/test/zuffix bin/test/random
	./bin/test/zuffix --gtest_color=yes
	./bin/test/random --gtest_color=yes

bin/test/zuffix: test/zuffix/* zuffix/* zuffix/*/*
	@mkdir -p bin/test
	$(CXX) $(CXXFLAGS) test/zuffix/test.cpp zuffix/external/SpookyV2.cpp -o bin/test/zuffix

bin/test/random: test/random/* zuffix/* zuffix/*/*
	@mkdir -p bin/test
	$(CXX) $(CXXFLAGS) test/random/test.cpp -o bin/test/random

.PHONY: clean

clean:
	@rm -rf ./bin
