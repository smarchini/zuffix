SUX = $(HOME)/src/sux/  # Change me
CXXFLAGS = -g -std=c++17 -Wall -Wextra -O0 -march=native -I./ -I$(SUX) -l gtest

all: test

test: bin/test/zuffix
	./bin/test/zuffix --gtest_color=yes

bin/test/zuffix: test/zuffix/* zuffix/*
	@mkdir -p bin/test
	$(CXX) $(CXXFLAGS) test/zuffix/test.cpp zuffix/hash/SpookyV2.cpp -o bin/test/zuffix

.PHONY: clean

clean:
	@rm -rf ./bin
