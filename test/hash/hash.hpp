#pragma once

#include "../common.hpp"

template <typename T> using CyclicPoly128Hash = zarr::CyclicPolyHash<T, 128>;

template <typename T> inline void battery(T h) {
	for (size_t i = 0; i <= 12; i++) EXPECT_EQ(h(0, i), h.immediate(0, i)) << "at index " << i;
	for (int i = 0; i <= 12; i++) EXPECT_EQ(h(0, i), h.immediate(0, i)) << "at index " << i;
	for (int i = 12; i >= 0; i--) EXPECT_EQ(h(0, i), h.immediate(0, i)) << "at index " << i;
	for (size_t i = 0; i < 12 / 2; i++) EXPECT_EQ(h(i, 12 - i), h.immediate(i, 12 - i)) << "at index " << i;
}

TEST(hash, RabinKarp) { battery(zarr::RabinKarpHash("abracadabra~")); }
TEST(hash, CyclicPoly128) { battery(zarr::CyclicPolyHash<char, 128>("abracadabra~")); }
TEST(hash, O1) { battery(zarr::CyclicPolyHash<char, 128>("abracadabra~")); }
TEST(hash, XXH3) { battery(zarr::CyclicPolyHash<char, 128>("abracadabra~")); }
