#pragma once

#include <cstddef>
#include <cstdint>

namespace zarr {

// TODO scrivere unit test per le rolling hash functions
template <typename T> class RabinKarpHash {
  public:
	const uint64_t magic = 0xf7c35; // TODO chiedere a Vigna dei numeri buoni

  private:
	T *string;
	uint64_t state = 0;
	size_t lpos = 0, rpos = 0;
	uint64_t lmul = magic, rmul = magic;

  public:
	RabinKarpHash(T *string) : string(string) {}

	uint64_t operator()(size_t from, size_t length) {
		for (; lpos < from; lpos++, lmul *= magic) state -= string[lpos] * lmul;
		for (; lpos > from; lpos--, lmul /= magic) state += string[lpos] * lmul;
		for (; rpos < from + length; rpos++, rmul *= magic) state += string[rpos] * rmul;
		for (; rpos > from + length; rpos--, rmul /= magic) state -= string[rpos] * rmul;
		return state;
	}
};

} // namespace zarr
