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
	XXH3Hash(T *string) : XXH3Hash(string, 0) {}

	// TODO parametrizzare 6 e 64 (constexpr)
	XXH3Hash(T *string, size_t length) : string(string), statetable(length / C + 1) {
		XXH3_state_t *state = XXH3_createState();
		XXH3_64bits_reset(state); // TODO forse non serve

		for (size_t i = 0; i <= length; i += C) {
			XXH3_64bits_update(state, string, i);
			statetable[i / C] = XXH3_createState();
			XXH3_copyState(statetable[i / C], state);
		}

		XXH3_freeState(state);
	}

	~XXH3Hash() {
		for (size_t i = 0; i < statetable.size(); i++) XXH3_freeState(statetable[i]);
	}

	uint64_t operator()(size_t to) {
    if (statetable.size() == 0) return immediate(0, to);
		XXH3_copyState(state, statetable[to / C]);
		XXH3_64bits_update(state, string, to % C);
		return XXH3_64bits_digest(state);
	}

	uint64_t operator()(size_t from, size_t length) { return immediate(from, length); }

	uint64_t immediate(size_t from, size_t length) { return XXH3_64bits(string + from, length * sizeof(T)); }
};

} // namespace zarr
