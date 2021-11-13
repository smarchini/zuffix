#pragma once

#include <iostream>

#include <cstddef>
#include <cstdint>

namespace zarr {

template <typename T> class RabinKarpHash {
  public:
	const uint64_t m = 0xf7c35;

  private:
	T *string;
	uint64_t state = 0;
	size_t l = 0, r = 0;

  public:
	RabinKarpHash(T *string) : string(string) {}

	uint64_t operator()(size_t from, size_t length) {
		// WARNING this is not quite Rabin-Karp: sum instead of multiplication
		for (; l < from; l++) state -= string[l] * m;
		for (; l > from; l--) state += string[l - 1] * m;
		for (; r < from + length; r++) state += string[r] * m;
		for (; r > from + length; r--) state -= string[r - 1] * m;
		return state;
	}
};

} // namespace zarr
