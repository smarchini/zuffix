#pragma once

#include <chrono>
#include <fstream>
#include <memory>
#include <random>
#include <sstream>
#include <string>

#include <sux/util/Vector.hpp>

#include <zuffix/util/common.hpp>

#include <zuffix/hash/WyHash.hpp>
#include <zuffix/hash/CRC32Plus32CFollyHash.hpp>
#include <zuffix/hash/CRC32CFollyHash.hpp>
#include <zuffix/hash/CRC32ZlibHash.hpp>
#include <zuffix/hash/CyclicPolyHash.hpp>
#include <zuffix/hash/O1Hash.hpp>
#include <zuffix/hash/RabinKarpHash.hpp>
#include <zuffix/hash/XXH3Hash.hpp>
#include <zuffix/hash/functions.hpp>

#include <zuffix/SimpleSuffixArray.hpp>
#include <zuffix/EnhancedSuffixArray.hpp>
#include <zuffix/MemcmpZuffixArray.hpp>
#include <zuffix/SignatureZuffixArray.hpp>
#include <zuffix/NothingZuffixArray.hpp>

#ifndef ALLOC_TYPE
#define ALLOC_TYPE MALLOC
#endif

static std::mt19937 rng(2023);

template <typename T> using CyclicPoly4Hash = zarr::CyclicPolyHash<T, 4>;
template <typename T> using CyclicPoly128Hash = zarr::CyclicPolyHash<T, 128>;

sux::util::Vector<char, sux::util::ALLOC_TYPE> randword(size_t n) {
	std::string s;
	s.reserve(n);
	std::uniform_int_distribution<char> dist('0', 'z');
	for (size_t i = 0; i < n; i++) s += dist(rng);
    return sux::util::Vector<char, sux::util::ALLOC_TYPE>(s.c_str(), s.length());
}

template <class T> std::string pretty(T value) {
	std::stringstream ss;
	ss.imbue(std::locale(""));
	ss << std::fixed << value;
	return ss.str();
}

std::tuple<std::chrono::nanoseconds::rep, std::chrono::nanoseconds::rep> median(std::vector<std::chrono::nanoseconds::rep> record) {
	size_t n = record.size();
	auto mid = record[n / 2];
	return std::make_tuple(mid, std::max(mid - record[0], record[n - 1] - mid));
}

std::tuple<std::chrono::nanoseconds::rep, std::chrono::nanoseconds::rep> mean(std::vector<std::chrono::nanoseconds::rep> record) {
	size_t n = record.size();
	auto avg = accumulate(begin(record), end(record), .0) / n;
	std::chrono::nanoseconds::rep var = 0;
	for(size_t i = 0; i < n; i++) var += (record[i] - avg) * (record[i] - avg);
	var /= n;
	return std::make_tuple(avg, sqrt(var));
}
