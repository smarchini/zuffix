#pragma once

#include "../common.hpp"

TEST(MemcmpZuffixArray, abracadabra) {
	using namespace zarr;
	std::string t("ABRACADABRA");
	MemcmpZuffixArray<char, RabinKarpHash> zuffix{String<char>(t, true)};

	EXPECT_EQ(zuffix.getSA().size(), 12);
	const size_t sa[12] = {0, 7, 3, 5, 10, 1, 8, 4, 6, 2, 9, 11};
	for (size_t i = 0; i < 12; i++) EXPECT_EQ(zuffix.getSA()[i], sa[i]) << "at index " << i;

	EXPECT_EQ(zuffix.getLCP().size(), 13);
	const ssize_t lcp[13] = {-1, 4, 1, 1, 1, 0, 3, 0, 0, 0, 2, 0, -1};
	for (size_t i = 0; i < 13; i++) EXPECT_EQ(zuffix.getLCP()[i], lcp[i]) << "at index " << i;

	EXPECT_EQ(zuffix.getCT().size(), 12);
	const size_t ct[12] = {12, 1, 3, 4, 2, 7, 6, 8, 9, 11, 10, 5};
	for (size_t i = 0; i < 12; i++) EXPECT_EQ(zuffix.getCT()[i], ct[i]) << "at index " << i;

	EXPECT_EQ(zuffix.find(String<char>("ABRACADABRA")), LInterval<size_t>(0, 1));
	EXPECT_EQ(zuffix.find(String<char>("ABRACAD")), LInterval<size_t>(0, 1));
	EXPECT_EQ(zuffix.find(String<char>("0")), LInterval<size_t>(1, 0));
	EXPECT_EQ(zuffix.find(String<char>("Z")), LInterval<size_t>(1, 0));
	EXPECT_EQ(zuffix.find(String<char>("ABRACADABR0")), LInterval<size_t>(1, 0));
	EXPECT_EQ(zuffix.find(String<char>("ABRACADABRZ")), LInterval<size_t>(1, 0));

	// clang-format off
	std::string patterns[] = { "A", "B", "C", "D", "R",
							   "AB", "BR", "CA", "DA", "RA",
							   "ABR", "BRA", "CAD", "DAB",
							   "ABRA", "BRAC", "CADA", "DABR",
							   "ABRAC", "BRACA", "CADAB", "DABRA",
							   "ABRACA", "BRACAD", "CADABR",
							   "ABRACAD", "BRACADA", "CADABRA",
							   "ABRACADA", "BRACADAB",
							   "ABRACADAB", "ABRACADABR", "ABRACADABRA" };
	// clang-format on

	for (auto p : patterns) {
		auto match = zuffix.find(String<char>(p));
		EXPECT_LE(match.from, match.to) << " on pattern: " << p;
		for (size_t i = match.from; i < match.to; i++) EXPECT_EQ(t.substr(sa[i], p.length()), p) << t.substr(sa[i], p.length()) << " != " << p;
	}
}

#define TS(NAME, DS)                                                                                                                                                                                   \
	TEST(MemcmpZuffixArray, NAME) {                                                                                                                                                                  \
		using namespace zarr;                                                                                                                                                                          \
		std::string t = fibonacci(10);                                                                                                                                                                 \
		MemcmpZuffixArray<char, RabinKarpHash> zuffix{String<char>(t, true)};                                                                                                                        \
		for (size_t i = 1; i <= 10; i++) {                                                                                                                                                             \
			auto match = zuffix.find(String<char>(fibonacci(i)));                                                                                                                                      \
			EXPECT_LE(match.from, match.to) << " on fibonacci: " << i;                                                                                                                                 \
		}                                                                                                                                                                                              \
	}

#define COMMA ,

TS(RabinKarp, MemcmpZuffixArray<char COMMA RabinKarpHash>)
TS(CyclicPoly128, MemcmpZuffixArray<char COMMA CyclicPoly128Hash>)
TS(O1, MemcmpZuffixArray<char COMMA O1Hash>)
TS(XXH3, MemcmpZuffixArray<char COMMA XXH3Hash>)
TS(CRC32, MemcmpZuffixArray<char COMMA CRC32Hash>)
