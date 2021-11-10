#pragma once

#include "util.hpp"
#include <iostream>

inline void fixedLength(std::string text, std::string charset, size_t plen) {
	zarr::SimpleSuffixArray<char> simple{zarr::String<char>(text, true)};
	zarr::EnhancedSuffixArray<char> enhanced{zarr::String<char>(text, true)};
	zarr::EnhancedZuffixArray<char, spooky> zuffix{zarr::String<char>(text, true)};

	for (int i = 0; i < 100; i++) {
		auto p = stdToZarr(randstring(charset, plen));
		EXPECT_EQ(simple.find(p), enhanced.find(p));
		EXPECT_EQ(simple.find(p), zuffix.find(p));
	}
}

inline void battery(std::string s) {
	fixedLength(randstring(s, 10), s, 0);
	fixedLength(randstring(s, 10), s, 1);
	fixedLength(randstring(s, 10), s, 2);
	fixedLength(randstring(s, 10), s, 3);
	fixedLength(randstring(s, 10), s, 4);
	fixedLength(randstring(s, 10), s, 5);
	fixedLength(randstring(s, 10), s, 6);
	fixedLength(randstring(s, 10), s, 7);
	fixedLength(randstring(s, 10), s, 8);
	fixedLength(randstring(s, 10), s, 9);
	fixedLength(randstring(s, 10), s, 10);

	fixedLength(randstring(s, 100), s, 5);
	fixedLength(randstring(s, 100), s, 10);
	fixedLength(randstring(s, 100), s, 20);
	fixedLength(randstring(s, 100), s, 30);
	fixedLength(randstring(s, 100), s, 40);
	fixedLength(randstring(s, 100), s, 50);
	fixedLength(randstring(s, 100), s, 60);
	fixedLength(randstring(s, 100), s, 70);
	fixedLength(randstring(s, 100), s, 80);
	fixedLength(randstring(s, 100), s, 90);

	fixedLength(randstring(s, 1000), s, 3);
	fixedLength(randstring(s, 1000), s, 5);
	fixedLength(randstring(s, 1000), s, 7);
	fixedLength(randstring(s, 1000), s, 9);

	fixedLength(randstring(s, 10000), s, 13);
	fixedLength(randstring(s, 10000), s, 15);
	fixedLength(randstring(s, 10000), s, 20);
	fixedLength(randstring(s, 10000), s, 30);
}

TEST(AllTheSame, sigma2) { battery("ab"); }

TEST(AllTheSame, sigma4) { battery("ACGT"); }

TEST(AllTheSame, sigma26) { battery("abcdefghijklmnopqrstuvwxyz"); }

TEST(AllTheSame, sigma72) { battery("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"); }
