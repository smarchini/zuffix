#pragma once

#include <sux/util/Vector.hpp>
#include <xxhash.h>

namespace zarr {
using ::sux::util::Vector;

template <typename T> class XXH3Hash {
  private:
	const T *string;
	Vector<XXH3_state_t *> statetable;
	static constexpr size_t C = 1 << 20;
	XXH3_state_t *state = XXH3_createState();

  public:
	XXH3Hash(T *string) : string(string), statetable(1) {
		// statetable[0] = XXH3_createState();
		// XXH3_64bits_update(statetable[0], string, 0);
	}

	~XXH3Hash() {
		XXH3_freeState(state);
		for (size_t i = 0; i < statetable.size(); i++) XXH3_freeState(statetable[i]);
	}

	uint64_t operator()(size_t to) {
		return immediate(0, to);
		// TODO work in progress
		const uint8_t *string = reinterpret_cast<const uint8_t *>(string);
		const size_t length = to * sizeof(T);

		const size_t tpos = length / C;
		// if (statetable.size() <= tpos) {
		// 	const size_t last = statetable.size() - 1;
		// 	XXH3_copyState(state, statetable[last]);
		// 	statetable.resize(tpos + 1);
		// 	for (size_t i = last; i <= tpos; i++) {
		// 		XXH3_64bits_update(state, string + i * C, C);
		// 		statetable[i] = XXH3_createState();
		// 		XXH3_copyState(statetable[i], state);
		// 	}
		// }

		std::cout << "tpos = " << tpos << std::endl;
		XXH3_copyState(state, statetable[tpos]);
		std::cout << "tpos = " << tpos << std::endl;
		if (to % C != 0) XXH3_64bits_update(state, string + tpos * C, length % C);
		std::cout << "tpos = " << tpos << std::endl;
		return XXH3_64bits_digest(state);
	}

	uint64_t operator()(size_t from, size_t length) { return immediate(from, length); }

	uint64_t immediate(size_t from, size_t length) { return XXH3_64bits(string + from, length * sizeof(T)); }
};

} // namespace zarr
