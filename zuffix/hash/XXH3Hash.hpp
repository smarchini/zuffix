#pragma once

#include <sux/util/Vector.hpp>
#include <xxhash.h>

namespace zarr {
using ::sux::util::Vector;

// WARNING: It is possible that XXH3 on master is bugged. If you set a different
// parameter C, you must run the tests to see if everything is okay. C = 1 << 12
// seems to be currently (2023/03/01) broken.
template <typename T, size_t C = 1 << 11> class XXH3Hash {
  private:
	const T *string;
	Vector<XXH3_state_t *> statetable;
	XXH3_state_t *state = XXH3_createState();

  public:
	XXH3Hash(T *string) : string(string), statetable(1) {
		XXH3_64bits_reset(state);
		statetable[0] = XXH3_createState();
		XXH3_copyState(statetable[0], state);
	}

	~XXH3Hash() {
		XXH3_freeState(state);
		for (size_t i = 0; i < statetable.size(); i++) XXH3_freeState(statetable[i]);
	}

	uint64_t operator()(size_t to) {
		const uint8_t *str = reinterpret_cast<const uint8_t *>(string);
		const size_t length = to * sizeof(T);

		const size_t tpos = length / C;
		if (statetable.size() <= tpos) {
			const size_t last = statetable.size() - 1;
			XXH3_copyState(state, statetable[last]);
			statetable.resize(tpos + 1);
			for (size_t i = last + 1; i <= tpos; i++) {
				XXH3_64bits_update(state, str + (i - 1) * C, C);
				statetable[i] = XXH3_createState();
				XXH3_copyState(statetable[i], state);
			}
		}

		XXH3_copyState(state, statetable[tpos]);
		if (to % C != 0) XXH3_64bits_update(state, str + tpos * C, length % C);
		return XXH3_64bits_digest(state);
	}

	uint64_t operator()(size_t from, size_t length) { return immediate(from, length); }

	uint64_t immediate(size_t from, size_t length) { return XXH3_64bits(string + from, length * sizeof(T)); }
};

} // namespace zarr
