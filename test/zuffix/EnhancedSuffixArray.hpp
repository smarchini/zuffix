#pragma once

#include "../common.hpp"

TEST(EnhancedSuffixArray, abracadabra) {
    using namespace zarr;
    std::string text = "ABRACADABRA";
    text.push_back(std::numeric_limits<char>::max());
    EnhancedSuffixArray<char> enhanced{text};

    EXPECT_EQ(enhanced.getSA().size(), 12);
    const size_t sa[12] = {0, 7, 3, 5, 10, 1, 8, 4, 6, 2, 9, 11};
    for (size_t i = 0; i < 12; i++) EXPECT_EQ(enhanced.getSA()[i], sa[i]) << "at index " << i;

    EXPECT_EQ(enhanced.getLCP().size(), 13);
    const ssize_t lcp[13] = {-1, 4, 1, 1, 1, 0, 3, 0, 0, 0, 2, 0, -1};
    for (size_t i = 0; i < 13; i++) EXPECT_EQ(enhanced.getLCP()[i], lcp[i]) << "at index " << i;

    EXPECT_EQ(enhanced.getCT().size(), 12);
    const size_t ct[12] = {12, 1, 3, 4, 2, 7, 6, 8, 9, 11, 10, 5};
    for (size_t i = 0; i < 12; i++) EXPECT_EQ(enhanced.getCT()[i], ct[i]) << "at index " << i;

    // NOTE: string literals are null-terminated, they don't exist in the text
    EXPECT_EQ(enhanced.find(std::string("ABRACADABRA")), LInterval<size_t>(0, 1));
    EXPECT_EQ(enhanced.find(std::string("ABRACAD")), LInterval<size_t>(0, 1));
    EXPECT_EQ(enhanced.find(std::string("0")), LInterval<size_t>(1, 0));
    EXPECT_EQ(enhanced.find(std::string("Z")), LInterval<size_t>(1, 0));
    EXPECT_EQ(enhanced.find(std::string("ABRACADABR0")), LInterval<size_t>(1, 0));
    EXPECT_EQ(enhanced.find(std::string("ABRACADABRZ")), LInterval<size_t>(1, 0));

    std::string patterns[] = { "A", "B", "C", "D", "R",
                               "AB", "BR", "CA", "DA", "RA",
                               "ABR", "BRA", "CAD", "DAB",
                               "ABRA", "BRAC", "CADA", "DABR",
                               "ABRAC", "BRACA", "CADAB", "DABRA",
                               "ABRACA", "BRACAD", "CADABR",
                               "ABRACAD", "BRACADA", "CADABRA",
                               "ABRACADA", "BRACADAB",
                               "ABRACADAB", "ABRACADABR", "ABRACADABRA" };

    for (auto p : patterns) {
        auto match = enhanced.find(p);
        EXPECT_LE(match.from, match.to) << " on pattern: " << p;
        for (size_t i = match.from; i < match.to; i++) EXPECT_EQ(text.substr(sa[i], p.length()), p) << text.substr(sa[i], p.length()) << " != " << p;
    }
}
