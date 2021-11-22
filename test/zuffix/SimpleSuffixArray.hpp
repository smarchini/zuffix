#pragma once

#include "../common.hpp"

TEST(SimpleSuffixArray, abracadabra) {
	using namespace zarr;
	std::string t("ABRACADABRA");
	SimpleSuffixArray<char> simple{String<char>(t, true)};

	EXPECT_EQ(simple.getSA().size(), 12);
	const size_t sa[12] = {0, 7, 3, 5, 10, 1, 8, 4, 6, 2, 9, 11};
	for (size_t i = 0; i < 12; i++) EXPECT_EQ(simple.getSA()[i], sa[i]) << "at index " << i;

	EXPECT_EQ(simple.find(String<char>("0")), LInterval<size_t>(0, 0));
	EXPECT_EQ(simple.find(String<char>("Z")), LInterval<size_t>(11, 11));
	EXPECT_EQ(simple.find(String<char>("ABRACADABR0")), LInterval<size_t>(0, 0));
	EXPECT_EQ(simple.find(String<char>("ABRACADABRZ")), LInterval<size_t>(1, 1));

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
		auto match = simple.find(zarr::String<char>(p));
		EXPECT_LE(match.from, match.to) << " on pattern: " << p;
		for (size_t i = match.from; i < match.to; i++) EXPECT_EQ(t.substr(sa[i], p.length()), p) << t.substr(sa[i], p.length()) << " != " << p;
	}
}
