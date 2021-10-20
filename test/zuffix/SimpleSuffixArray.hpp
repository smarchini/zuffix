#pragma once

#include <SpookyV2.h>
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <unordered_set>
#include <zuffix/SimpleSuffixArray.hpp>
#include <zuffix/random/xoroshiro128plus_engine.hpp>
#include <zuffix/util/String.hpp>

zarr::String<char> stringToString(std::string string, bool dollar = false) { return zarr::String<char>(string, dollar); }

TEST(SimpleSuffixArray, abracadabra) {
	std::string t("ABRACADABRA");
	zarr::SimpleSuffixArray<char> simple{zarr::String<char>(t, true)};

	EXPECT_EQ(simple.getSA().size(), 12);
	const size_t sa[12] = {0, 7, 3, 5, 10, 1, 8, 4, 6, 2, 9, 11};
	for (size_t i = 0; i < 12; i++) EXPECT_EQ(simple.getSA()[i], sa[i]);

	EXPECT_EQ(simple.find(stringToString("0")), zarr::LInterval<ssize_t>(0, -1));
	EXPECT_EQ(simple.find(stringToString("Z")), zarr::LInterval<ssize_t>(11, 10));
	EXPECT_EQ(simple.find(stringToString("ABRACADABR0")), zarr::LInterval<ssize_t>(0, -1));
	EXPECT_EQ(simple.find(stringToString("ABRACADABRZ")), zarr::LInterval<ssize_t>(1, 0));

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
		for (ssize_t i = match.from; i < match.to; i++) EXPECT_EQ(t.substr(sa[i], p.length()), p) << t.substr(sa[i], p.length()) << " != " << p;
	}
}
