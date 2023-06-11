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

static const std::string a = "abracadabra~";

TEST(hash, XXH3) { battery(zarr::XXH3Hash<char>(a.c_str(), a.length()), a.length()); }
TEST(hash, WyHash) { battery(zarr::WyHash<char>(a.c_str(), a.length()), a.length()); }
TEST(hash, CRC32CFolly) { battery(zarr::CRC32CFollyHash<char>(a.c_str(), a.length()), a.length()); }
TEST(hash, CRC32Zlib) { battery(zarr::CRC32ZlibHash<char>(a.c_str(), a.length()), a.length()); }
TEST(hash, CRC32Plus32CFolly) { battery(zarr::CRC32Plus32CFollyHash<char>(a.c_str(), a.length()), a.length()); }
TEST(hash, RabinKarp) { battery(zarr::RabinKarpHash<char>(a.c_str(), a.length()), a.length()); }
TEST(hash, CyclicPoly128) { battery(CyclicPoly128Hash<char>(a.c_str(), a.length()), a.length()); }
TEST(hash, O1) { battery(zarr::O1Hash<char>(a.c_str(), a.length()), a.length()); }

static const std::string f = fiboword(34);

TEST(longhash, XXH3) { battery(zarr::XXH3Hash<char>(f.c_str(), f.length()), 1 << 20, 10000); }
TEST(longhash, WyHash) { battery(zarr::WyHash<char>(f.c_str(), f.length()), 1 << 20, 10000); }
TEST(longhash, CRC32CFolly) { battery(zarr::CRC32CFollyHash<char>(f.c_str(), f.length()), 1 << 20, 10000); }
TEST(longhash, CRC32Zlib) { battery(zarr::CRC32ZlibHash<char>(f.c_str(), f.length()), 1 << 20, 10000); }
TEST(longhash, CRC32Plus32CFolly) { battery(zarr::CRC32Plus32CFollyHash<char>(f.c_str(), f.length()), 1 << 20, 10000); }
TEST(longhash, RabinKarp) { battery(zarr::RabinKarpHash<char>(f.c_str(), f.length()), 1 << 20, 10000); }
TEST(longhash, CyclicPoly128) { battery(CyclicPoly128Hash<char>(f.c_str(), f.length()), 1 << 20, 10000); }
TEST(longhash, O1) { battery(zarr::O1Hash<char>(f.c_str(), f.length()), 1 << 20, 10000); }
