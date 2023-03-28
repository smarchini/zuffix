#pragma once

#include "../common.hpp"

template <typename T, sux::util::AllocType AT = sux::util::AllocType::MALLOC> using CyclicPoly128Hash = zarr::CyclicPolyHash<T, 128, AT>;

template <typename T> inline void battery(T h, int n, int skip = 1) {
	EXPECT_EQ(h(0, n), h.immediate(0, n)) << "first";
	for (size_t i = 0; i <= n; i += skip) EXPECT_EQ(h(0, i), h.immediate(0, i)) << "at index " << i;
	for (size_t i = 0; i <= n; i += skip) EXPECT_EQ(h(i), h.immediate(0, i)) << "at index " << i;
	for (int i = 0; i <= n; i += skip) EXPECT_EQ(h(0, i), h.immediate(0, i)) << "at index " << i;
	for (int i = 0; i <= n; i += skip) EXPECT_EQ(h(i), h.immediate(0, i)) << "at index " << i;
	for (int i = n; i >= 0; i -= skip) EXPECT_EQ(h(0, i), h.immediate(0, i)) << "at index " << i;
	for (int i = n; i >= 0; i -= skip) EXPECT_EQ(h(i), h.immediate(0, i)) << "at index " << i;
	for (size_t i = 0; i < n / 2; i += skip) EXPECT_EQ(h(i, n - i), h.immediate(i, n - i)) << "at index " << i;
}

// TODO rimuovere zarr::String
zarr::String<char> abracadabra("abracadabra~");

TEST(hash, XXH3) { battery(zarr::XXH3Hash<char>(&abracadabra, abracadabra.length()), 12); }
TEST(hash, WyHash) { battery(zarr::WyHash<char>(&abracadabra, abracadabra.length()), 12); }
TEST(hash, CRC32CFolly) { battery(zarr::CRC32CFollyHash<char>(&abracadabra, abracadabra.length()), 12); }
TEST(hash, CRC32Zlib) { battery(zarr::CRC32ZlibHash<char>(&abracadabra, abracadabra.length()), 12); }
TEST(hash, CRC32Plus32CFolly) { battery(zarr::CRC32Plus32CFollyHash<char>(&abracadabra, abracadabra.length()), 12); }
TEST(hash, RabinKarp) { battery(zarr::RabinKarpHash<char>(&abracadabra, abracadabra.length()), 12); }
TEST(hash, CyclicPoly128) { battery(zarr::CyclicPolyHash<char, 128>(&abracadabra, abracadabra.length()), 12); }
TEST(hash, CRC32) { battery(zarr::CRC32Hash<char>(&abracadabra, abracadabra.length()), 12); }
TEST(hash, O1) { battery(zarr::O1Hash<char>(&abracadabra, abracadabra.length()), 12); }

zarr::String<char> fibo(fibonacci(34)); // slightly over 14 MiB

TEST(longhash, XXH3) { battery(zarr::XXH3Hash<char>(&fibo, fibo.length()), 1 << 20, 10000); }
TEST(longhash, WyHash) { battery(zarr::WyHash<char>(&fibo, fibo.length()), 1 << 20, 10000); }
TEST(longhash, CRC32CFolly) { battery(zarr::CRC32CFollyHash<char>(&fibo, fibo.length()), 1 << 20, 10000); }
TEST(longhash, CRC32Zlib) { battery(zarr::CRC32ZlibHash<char>(&fibo, fibo.length()), 1 << 20, 10000); }
TEST(longhash, CRC32Plus32CFolly) { battery(zarr::CRC32Plus32CFollyHash<char>(&fibo, fibo.length()), 1 << 20, 10000); }
TEST(longhash, RabinKarp) { battery(zarr::RabinKarpHash<char>(&fibo, fibo.length()), 1 << 20, 10000); }
TEST(longhash, CyclicPoly128) { battery(zarr::CyclicPolyHash<char, 128>(&fibo, fibo.length()), 1 << 20, 10000); }
TEST(longhash, CRC32) { battery(zarr::CRC32Hash<char>(&fibo, fibo.length()), 1 << 20, 10000); }
TEST(longhash, O1) { battery(zarr::O1Hash<char>(&fibo, fibo.length()), 1 << 20, 10000); }
