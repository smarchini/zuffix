#pragma once

#include <zuffix/hash/CyclicPolyHash.hpp>
#include <zuffix/hash/RabinKarpHash.hpp>

TEST(RabinKarpHash, abracadabra) {
	zarr::RabinKarpHash h("abracadabra~");

	for (size_t i = 0; i < 12; i++) {
      zarr::RabinKarpHash g("abracadabra~");
      EXPECT_EQ(h(0, i), g(0, i)) << "at index " << i;
    }

	for (size_t i = 0; i < 12; i++) {
      zarr::RabinKarpHash g("abracadabra~");
      EXPECT_EQ(h(i, 12), g(i, 12)) << "at index " << i;
    }
}

TEST(CyclicPolyHash, abracadabra) {
	zarr::CyclicPolyHash<char, 6> h("abracadabra~");

	for (size_t i = 0; i < 12; i++) {
      zarr::CyclicPolyHash<char, 6> g("abracadabra~");
      EXPECT_EQ(h(0, i), g(0, i)) << "at index " << i;
    }
}
