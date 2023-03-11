#pragma once

#include "../common.hpp"

TEST(AllTheSame, fibonacci) {
	using namespace zarr;
	std::string text = fibonacci(10);
	text.push_back(std::numeric_limits<char>::max());

	SimpleSuffixArray<char> simple{text};
	EnhancedSuffixArray<char> enhanced{text};
	MemcmpZuffixArray<char, RabinKarpHash> rabinkarp{text};
	MemcmpZuffixArray<char, CyclicPoly128Hash> cyclicpoly{text};
	MemcmpZuffixArray<char, O1Hash> o1{text};
	MemcmpZuffixArray<char, XXH3Hash> xxh3{text};
	MemcmpZuffixArray<char, CRC32Hash> crc32{text};
	SignatureZuffixArray<char, CRC32CFollyHash> pcrc32{text};

	MemcmpZuffixArray<char, BadHash> bad{text};

	for (int i = 2; i < 10; i++) {
		std::string p = fibonacci(i);
		EXPECT_EQ(simple.find(p), rabinkarp.find(p));
		EXPECT_EQ(simple.find(p), cyclicpoly.find(p));
		EXPECT_EQ(simple.find(p), o1.find(p));
		EXPECT_EQ(simple.find(p), xxh3.find(p));
		EXPECT_EQ(simple.find(p), crc32.find(p));
		EXPECT_EQ(simple.find(p), bad.find(p));
		EXPECT_EQ(simple.find(p), pcrc32.find(p));
	}
}

inline void fixlen(std::string charset, size_t n, size_t m) {
	using namespace zarr;
	std::string text = random(charset, n);
	text.push_back(std::numeric_limits<char>::max());

	SimpleSuffixArray<char> simple{text};
	EnhancedSuffixArray<char> enhanced{text};
	MemcmpZuffixArray<char, RabinKarpHash> rabinkarp{text};
	MemcmpZuffixArray<char, CyclicPoly128Hash> cyclicpoly128{text};
	MemcmpZuffixArray<char, O1Hash> o1{text};
	MemcmpZuffixArray<char, XXH3Hash> xxh3{text};
	MemcmpZuffixArray<char, CRC32Hash> crc32{text};
	SignatureZuffixArray<char, CRC32CFollyHash> pcrc32{text};

	MemcmpZuffixArray<char, BadHash> bad{text};

	for (int i = 0; i < 100; i++) {
		auto p = random(charset, m);
		EXPECT_EQ(simple.find(p), rabinkarp.find(p));
		EXPECT_EQ(simple.find(p), cyclicpoly128.find(p));
		EXPECT_EQ(simple.find(p), o1.find(p));
		EXPECT_EQ(simple.find(p), xxh3.find(p));
		EXPECT_EQ(simple.find(p), crc32.find(p));
		EXPECT_EQ(simple.find(p), bad.find(p));
		EXPECT_EQ(simple.find(p), pcrc32.find(p));
	}
}

inline void battery(std::string charset) {
	for (size_t n = 10; n < 10000; n *= 10) {
		for (size_t m = 1; m < n; m *= 10) {
			fixlen(charset, n, m);
		}
	}
}

TEST(AllTheSame, ab) { battery("ab"); }
TEST(AllTheSame, ACGT) { battery("ACGT"); }
TEST(AllTheSame, az) { battery("abcdefghijklmnopqrstuvwxyz"); }
TEST(AllTheSame, azAZ09) { battery("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"); }
