#pragma once

#include <zuffix/hash/CyclicPolyHash.hpp>
#include <zuffix/hash/RabinKarpHash.hpp>

TEST(RabinKarpHash, abracadabra) {
	zarr::RabinKarpHash h("abracadabra~");

	for (int i = 0; i <= 12; i++) {
      zarr::RabinKarpHash g("abracadabra~");
      EXPECT_EQ(h(0, i), g(0, i)) << "at index " << i;
    }

    {
      zarr::RabinKarpHash g("bracadabra~");
      EXPECT_EQ(h(1, 11), g(0, 11)) << "at pluto";
    }

    {
      zarr::RabinKarpHash g("racadabra~");
      EXPECT_EQ(h(2, 10), g(0, 10)) << "at pluto";
    }

    {
      zarr::RabinKarpHash g("bracadabra~");
      EXPECT_EQ(h(1, 11), g(0, 11)) << "at pluto";
    }

	for (int i = 0; i <= 12; i++) {
      zarr::RabinKarpHash g("abracadabra~");
      EXPECT_EQ(h(0, i), g(0, i)) << "at index " << i;
    }

	for (int i = 12; i >= 0; i--) {
      zarr::RabinKarpHash g("abracadabra~");
      EXPECT_EQ(h(0, i), g(0, i)) << "at index " << i;
    }

	for (size_t i = 0; i < 12/2; i++) {
      zarr::RabinKarpHash g("abracadabra~");
      EXPECT_EQ(h(i, 12-i), g(i, 12-i)) << "at index " << i;
    }
}

TEST(CyclicPolyHash, abracadabra) {
	zarr::CyclicPolyHash<char, 128> h("abracadabra~");

	for (size_t i = 0; i < 12; i++) {
      zarr::CyclicPolyHash<char, 128> g("abracadabra~");
      EXPECT_EQ(h(0, i), g(0, i)) << "at index " << i;
    }

    {
      zarr::CyclicPolyHash<char, 128> g("bracadabra~");
      EXPECT_EQ(h(1, 11), g(0, 11)) << "at pluto";
    }

    {
      zarr::CyclicPolyHash<char, 128> g("racadabra~");
      EXPECT_EQ(h(2, 10), g(0, 10)) << "at pluto";
    }

    {
      zarr::CyclicPolyHash<char, 128> g("bracadabra~");
      EXPECT_EQ(h(1, 11), g(0, 11)) << "at pluto";
    }

	for (int i = 0; i <= 12; i++) {
      zarr::CyclicPolyHash<char, 128> g("abracadabra~");
      EXPECT_EQ(h(0, i), g(0, i)) << "at index " << i;
    }

	for (int i = 12; i >= 0; i--) {
      zarr::CyclicPolyHash<char, 128> g("abracadabra~");
      EXPECT_EQ(h(0, i), g(0, i)) << "at index " << i;
    }

	for (size_t i = 0; i < 12/2; i++) {
      zarr::CyclicPolyHash<char, 128> g("abracadabra~");
      EXPECT_EQ(h(i, 12-i), g(i, 12-i)) << "at index " << i;
    }
}
