#pragma once

#include <cstddef>
#include <cstdint>

namespace zarr {

template <typename T> class RabinKarpHash {
  public:
	// md = 1 mod 2^64
	const uint64_t m = 0xf7c35;
	const uint64_t d = 0xe94870e35dd4f61d;

  private:
	T *string;
	uint64_t state = 0;
	size_t l = 0, r = 0;
	uint64_t pow = 1;

  public:
	RabinKarpHash(T *string) : string(string) {}

	uint64_t operator()(size_t from, size_t length) {
		for (; l < from; l++, pow *= d) state -= string[l] * pow * d;
		for (; l > from; l--, pow *= m) state += string[l - 1] * pow;
		for (; r < from + length; r++, pow *= m) state = state * m + string[r];
		for (; r > from + length; r--, pow *= d) state = (state - string[r - 1]) * d;
		return state;
	}
};

} // namespace zarr
