#pragma once

#include "util.hpp"
#include <SpookyV2.h>
#include <gtest/gtest.h>
#include <string>
#include <unordered_set>
#include <zuffix/ZuffixArray.hpp>
#include <zuffix/hash/CyclicHash.hpp>
#include <zuffix/random/xoroshiro128plus_engine.hpp>
#include <zuffix/util/String.hpp>

uint64_t spooky_hash(const void *message, size_t length) { return SpookyHash::Hash64(message, length, 0); }

uint64_t cyclic_hash(const void *message, size_t length) {
	static zarr::CyclicHash<zarr::xoroshiro128plus_engine> hash(0, 37);
	return hash(message, length);
}

uint64_t broken_spooky_hash(const void *message, size_t length) {
	static constexpr size_t mask = (1ULL << 37) - 1;
	return SpookyHash::Hash64(message, length, 0) & mask;
}

template <typename T> zarr::String<T> arrayToString(const T *array, size_t length, bool dollar = false) {
	zarr::String<T> result(length, dollar);
	for (size_t i = 0; i < length; i++) result[i] = array[i];
	return result;
}

template <typename T1, typename T2> void print_pair(std::pair<T1, T2> pair) { std::cout << pair.first << ", " << pair.second << std::endl; }

template <typename T> bool string_equals(const zarr::String<T> &string, const T *array, size_t length) {
	if (string.length() != length) return false;
	for (size_t i = 0; i < length; i++) {
		if (string[i] != array[i]) return false;
	}

	return true;
}

template <typename T, zarr::hash_function HF> void check(const zarr::String<T> &string, const zarr::ZuffixArray<T, HF> &zuffix, const zarr::String<T> &pattern) {
	auto interval = zuffix.find(pattern);
	EXPECT_EQ(zuffix.findExact(pattern), zuffix.find(pattern));

	std::unordered_set<size_t> pos;

	for (size_t j = 0; j < interval.length(); j++) {
		size_t p = zuffix.getSuffix()[interval.from + j];
		pos.insert(p);
		// EXPECT_EQ
	}

	int64_t limit = string.length() - pattern.length();
	for (int64_t p = 0; p < limit; p++) EXPECT_EQ(string_equals(pattern, &string + p, pattern.length()), pos.find(p) != std::end(pos));
}

/*
zarr::String<int8_t> dna(size_t length, bool dollar = false) {
	zarr::String<int8_t> result(length, dollar);

	for (size_t i = 0, c = next() & 0b11; i < length; i++) {
		if (next() < UINT64_MAX / 4) c = next() & 0b11;
		result[i] = c;
	}

	return result;
}
*/

TEST(zuffix, abracadabra) {
	std::string abracadabra("ABRACADABRA");
	zarr::ZuffixArray<char, spooky_hash> zuffix{zarr::String<char>(abracadabra, true)};
	zuffix.visitPre(0, abracadabra.length(), 0, 0);

	std::cout << "test find: " << zuffix.findExact(zarr::String<char>(std::string("A"))) << std::endl;
	std::cout << "test find: " << zuffix.findExact(zarr::String<char>(std::string("AB"))) << std::endl;
	std::cout << "test find: " << zuffix.findExact(zarr::String<char>(std::string("ABR"))) << std::endl;
	std::cout << "test find: " << zuffix.findExact(zarr::String<char>(std::string("CAD"))) << std::endl;
	std::cout << "test find: " << zuffix.findExact(zarr::String<char>(std::string("CADR"))) << std::endl;
	std::cout << "test find: " << zuffix.findExact(zarr::String<char>(std::string("ABRAD"))) << std::endl;
}

/*
TEST(zuffix, test) {
	int8_t s[] = {0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1, 126};
	zarr::ZuffixArray<int8_t, spooky_hash> zuffix(arrayToString(s, sizeof(s) / sizeof(s[0]), true));
	zuffix.visitPre(0, sizeof(s) / sizeof(s[0]), 0, 0);

	int8_t _v[] = {0, 1, 0, 1};
	auto v = arrayToString(_v, sizeof(_v) / sizeof(_v[0]));
	std::cout << "find: " << zuffix.find(v) << std::endl;
	std::cout << "findExact: " << zuffix.findExact(v) << std::endl;
}

TEST(zuffix, test_all) {
	int8_t s_[] = {0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1};
	auto s = arrayToString(s_, sizeof(s_) / sizeof(s_[0]), true);
	zarr::ZuffixArray<int8_t, spooky_hash> zuffix(std::move(s));

	for (size_t l = 1; l < 15; l++) {
		for (size_t i = 0; i < (1ULL << l); i++) {
			zarr::String<int8_t> v(l);

			for (size_t j = 0; j < l; j++) v[j] = (i & (1ULL << j)) != 0;

			check(s, zuffix, v);
		}
	}
}

TEST(zuffix, test_const) {
	zarr::String<int8_t> s(5, true);
	zarr::ZuffixArray<int8_t, spooky_hash> zuffix(std::move(s));

	for (size_t l = 1; l < 7; l++) {
		for (size_t i = 0; i < (1ULL << l); i++) {
			zarr::String<int8_t> v(l);

			for (size_t j = 0; j < l; j++) v[j] = (i & (1ULL << j)) != 0;

			check(s, zuffix, v);
		}
	}
}

TEST(zuffix, test_distinct) {
	zarr::String<int8_t> s(4, true);

	for (size_t i = 0; i < s.length() - 1; i++) s[i] = i;

	zarr::ZuffixArray<int8_t, spooky_hash> zuffix(std::move(s));

	for (size_t l = 1; l < 6; l++) {
		for (size_t i = 0; i < (1ULL << 2 * l); i++) {
			zarr::String<int8_t> v(l);

			for (size_t j = 0; j < l; j++) v[j] = i >> j * 2 & 3;

			check(s, zuffix, v);
		}
	}
}

TEST(zuffix, dna_1000) {
	auto s = dna(1000, true);
	zarr::ZuffixArray<int8_t, spooky_hash> zuffix(std::move(s));

	for (size_t t = 1; t < 100000; t++) {
		auto v = dna(1 + next() % (s.length() + 4));
		check(s, zuffix, v);
	}
}

TEST(zuffix, dna_all) {
	auto s = dna(10, true);
	zarr::ZuffixArray<int8_t, spooky_hash> zuffix(std::move(s));

	for (size_t l = 1; l < 12; l++) {
		for (size_t i = 0; i < (1ULL << 2 * l); i++) {
			zarr::String<int8_t> v(l);

			for (size_t j = 0; j < l; j++) v[j] = i >> j * 2 & 3;

			check(s, zuffix, v);
		}
	}
}

TEST(zuffix, dna_collisions_spooky) {
	auto s = dna(1000000, true);
	size_t mask = (1ULL << 37) - 1;
	// we need to typedef this thing to make gtest macro trickeries happy
	typedef zarr::ZuffixArray<int8_t, broken_spooky_hash> zuffix_type;
	ASSERT_DEATH(zuffix_type zuffix(std::move(s)), "The element alaredy exists");
}

TEST(zuffix, dna_collisions_cyclic) {
	auto s = dna(1000000, true);
	// we need to typedef this thing to make gtest macro trickeries happy
	typedef zarr::ZuffixArray<int8_t, cyclic_hash> zuffix_type;
	ASSERT_DEATH(zuffix_type zuffix(std::move(s)), "The element alaredy exists");
}
*/
