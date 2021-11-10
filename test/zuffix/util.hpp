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

uint64_t spooky(const void *message, size_t length) { return SpookyHash::Hash64(message, length, 0); }
