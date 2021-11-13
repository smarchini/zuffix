#pragma once

#include <cstddef>
#include <cstdint>

namespace zarr {

template <typename T> class RabinKarpHash {
  public:
	const uint64_t m = 0x3; // WARNING must be small, otherwise it ruins the division

  private:
	T *string;
	uint64_t state = 0;
	size_t l = 0, r = 0;
	uint64_t mpow = 1;

  public:
	RabinKarpHash(T *string) : string(string) {}

	uint64_t operator()(size_t from, size_t length) {
		// WARNING division is really bad due to multiplication mod 2^64
		for (; l > from; l--, mpow *= m) state = state + string[l - 1] * mpow;
		for (; r < from + length; r++, mpow *= m) state = state * m + string[r];
		// NOTE mpow/=m must b after mpow*=m, otherwise mpow may become zero
		for (; l < from; l++, mpow /= m) state = state - string[l] * mpow / m;
		for (; r > from + length; r--, mpow /= m) state = (state - string[r - 1]) / m;
		return state;
	}
};

} // namespace zarr
