#pragma once

#include <xxhash.h>

namespace zarr {

template <typename T> class XXH3Hash {
  private:
	const T *string;

  public:
	XXH3Hash(T *string) : string(string) {}
	uint64_t operator()(size_t to) { return (*this)(0, to); }
	uint64_t operator()(size_t from, size_t length) { return XXH3_64bits(string + from, length * sizeof(T)); }
	uint64_t immediate(size_t from, size_t length) { return XXH3_64bits(string + from, length * sizeof(T)); }
};

} // namespace zarr
