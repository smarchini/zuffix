#pragma once

#include "../common.hpp"

TEST(AllTheSame, fibonacci) {
	using namespace zarr;
	std::string text = fibonacci(10);

	SimpleSuffixArray<char> simple{String<char>(text, true)};
	EnhancedSuffixArray<char> enhanced{String<char>(text, true)};
	EnhancedZuffixArray<char, RabinKarpHash> rabinkarp{String<char>(text, true)};
	EnhancedZuffixArray<char, CyclicPoly128Hash> cyclicpoly{String<char>(text, true)};
	EnhancedZuffixArray<char, BadHash> bad{String<char>(text, true)};

	for (int i = 1; i <= 10; i++) {
		String<char> p = String<char>(fibonacci(i));
		EXPECT_EQ(simple.find(p), rabinkarp.find(p));
		EXPECT_EQ(simple.find(p), cyclicpoly.find(p));
		EXPECT_EQ(simple.find(p), bad.find(p));
	}
}

inline void fixlen(std::string charset, size_t n, size_t m) {
	using namespace zarr;
	std::string text = random(charset, n);

	SimpleSuffixArray<char> simple{String<char>(text, true)};
	EnhancedSuffixArray<char> enhanced{String<char>(text, true)};
	EnhancedZuffixArray<char, RabinKarpHash> rabinkarp{String<char>(text, true)};
	EnhancedZuffixArray<char, CyclicPoly128Hash> cyclicpoly128{String<char>(text, true)};
	EnhancedZuffixArray<char, BadHash> bad{String<char>(text, true)};

	for (int i = 0; i < 100; i++) {
		auto p = String<char>(random(charset, m));
		EXPECT_EQ(simple.find(p), rabinkarp.find(p));
		EXPECT_EQ(simple.find(p), cyclicpoly128.find(p));
		EXPECT_EQ(simple.find(p), bad.find(p));
	}
}

inline void battery(std::string charset) {
	for (size_t n = 10; n < 10000; n *= 10)
		for (size_t m = 1; m < n; m *= 10) fixlen(charset, n, m);
}

TEST(AllTheSame, ab) { battery("ab"); }
TEST(AllTheSame, ACGT) { battery("ACGT"); }
TEST(AllTheSame, az) { battery("abcdefghijklmnopqrstuvwxyz"); }
TEST(AllTheSame, azAZ09) { battery("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"); }
