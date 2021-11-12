#pragma once

#include <iostream>
using namespace std;

#include <cstddef>
#include <cstdint>

namespace zarr {

template <typename T> class RabinKarpHash {
  public:

  private:
	T *string;
	size_t pos = 0;
	uint64_t state = 0, magic = 0xf7031; // TODO chiedere a Vigna dei numeri buoni

  public:
	RabinKarpHash(T *string) : string(string) {}

	uint64_t operator+=(size_t n) { // plus to the right
		for (size_t i = 0; i < n; i++, magic *= magic, pos++) state += string[pos] * magic;
		// cout << "hash += " << n << " = " << state << endl;
		return state;
	}

	uint64_t operator-=(size_t n) { // minus to the right
		// WARNING division, uhm...
		for (size_t i = 0; i < n; i++, magic /= magic, pos--) state -= string[pos] * magic;
		// cout << "hash -= " << n << " = " << state << endl;
		return state;
	}

	uint64_t operator()(size_t n) {
		uint64_t x = n < pos ? *this += pos - n : *this += n - pos;
		cout << "hash(" << n << ") = " << state << endl;
        return x;
	}

  uint64_t slow_for_testing(size_t from, size_t to) {
    for (size_t i = 0; i < from; i++, magic *= magic, pos++); // skip
    for (size_t i = from; i < to; i++, magic *= magic, pos++) state += string[pos] * magic;
    return state;
  }
};

} // namespace zarr
