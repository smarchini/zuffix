#pragma once

#include <gtest/gtest.h>

#include <random>
#include <string>

#include <SpookyV2.h>

#include <zuffix/random/xoroshiro128plus_engine.hpp>
#include <zuffix/util/String.hpp>

#include <zuffix/EnhancedSuffixArray.hpp>
#include <zuffix/EnhancedZuffixArray.hpp>
#include <zuffix/SimpleSuffixArray.hpp>

#include <zuffix/hash/CyclicPolyHash.hpp>
#include <zuffix/hash/RabinKarpHash.hpp>

inline zarr::String<char> stdToZarr(std::string string, bool dollar = false) { return zarr::String<char>(string, dollar); }

template <typename T> sux::util::Vector<T> stdToZarr(const T *array, size_t length) {
	sux::util::Vector<T> result(length);
	for (size_t i = 0; i < length; i++) result[i] = array[i];
	return result;
}

// sux::util::Vector<char> stringToVector(std::string string) {
// 	size_t length = string.length();
// 	sux::util::Vector<char> result(length);
// 	for (size_t i = 0; i < length; i++) result[i] = string[i];
// 	return result;
// }

inline std::string randstring(std::string charset, size_t length) {
	std::string result;
	std::random_device rd;
	zarr::xoroshiro128plus_engine rng(rd());
	std::uniform_int_distribution<size_t> dist(0, charset.length());

	result.reserve(length);
	for (size_t i = 0; i < length; i++) result += charset[dist(rng)];
	return result;
}


template <typename T> using CyclicPoly128Hash = zarr::CyclicPolyHash<T, 128>;

uint64_t spooky(const void *message, size_t length) { return SpookyHash::Hash64(message, length, 0); }

template <typename T> class ShittyBitHash {
  private:
	T *string;
	uint64_t state = 0;
	size_t l = 0, r = 0;

  public:
	ShittyBitHash(T *string) : string(string) {}

	uint64_t operator()(size_t from, size_t length) {
		for (; l < from; l++) state ^= string[l] & 0b11;
		for (; l > from; l--) state ^= string[l - 1] & 0b11;
		for (; r < from + length; r++) state ^= string[r] & 0b11;
		for (; r > from + length; r--) state ^= string[r - 1] & 0b11;
		return state;
	}
};
