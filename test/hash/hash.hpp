#pragma once

#include "../common.hpp"

template <typename T> using CyclicPoly128Hash = zarr::CyclicPolyHash<T, 128>;

template <typename T> inline void battery(T h) {
	EXPECT_EQ(h(0, 12), h.immediate(0, 12)) << "first";
	for (size_t i = 0; i <= 12; i++) EXPECT_EQ(h(0, i), h.immediate(0, i)) << "at index " << i;
	for (size_t i = 0; i <= 12; i++) EXPECT_EQ(h(i), h.immediate(0, i)) << "at index " << i;
	for (int i = 0; i <= 12; i++) EXPECT_EQ(h(0, i), h.immediate(0, i)) << "at index " << i;
	for (int i = 0; i <= 12; i++) EXPECT_EQ(h(i), h.immediate(0, i)) << "at index " << i;
	for (int i = 12; i >= 0; i--) EXPECT_EQ(h(0, i), h.immediate(0, i)) << "at index " << i;
	for (int i = 12; i >= 0; i--) EXPECT_EQ(h(i), h.immediate(0, i)) << "at index " << i;
	for (size_t i = 0; i < 12 / 2; i++) EXPECT_EQ(h(i, 12 - i), h.immediate(i, 12 - i)) << "at index " << i;
}

zarr::String<char> abracadabra("abracadabra~");

TEST(hash, RabinKarp) { battery(zarr::RabinKarpHash<char>(&abracadabra)); }
TEST(hash, CyclicPoly128) { battery(zarr::CyclicPolyHash<char, 128>(&abracadabra)); }
TEST(hash, O1) { battery(zarr::O1Hash<char>(&abracadabra)); }
TEST(hash, XXH3) { battery(zarr::XXH3Hash<char>(&abracadabra)); }
TEST(hash, CRC64) { battery(zarr::CRC64Hash<char>(&abracadabra)); }

zarr::String<char> fibo(fibonacci(30));

TEST(longhash, RabinKarp) { battery(zarr::RabinKarpHash(&fibo)); }
TEST(longhash, CyclicPoly128) { battery(zarr::CyclicPolyHash<char, 128>(&fibo)); }
TEST(longhash, O1) { battery(zarr::O1Hash<char>(&fibo)); }
TEST(longhash, XXH3) { battery(zarr::XXH3Hash<char>(&fibo)); }
TEST(longhash, CRC64) { battery(zarr::CRC64Hash<char>(&fibo)); }
