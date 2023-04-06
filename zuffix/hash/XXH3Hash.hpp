#pragma once

#include <sux/util/Vector.hpp>
#include <xxhash.h>

bool operator==(const XXH128_hash_t& lhs, const XXH128_hash_t& rhs) {
	return XXH128_isEqual(lhs, rhs);
}

bool operator!=(const XXH128_hash_t& lhs, const XXH128_hash_t& rhs) {
	return !XXH128_isEqual(lhs, rhs);
}

namespace zarr {
using ::sux::util::Vector;
using ::sux::util::AllocType;

template <typename T, AllocType AT = MALLOC, size_t C = 1 << 16> class XXH3Hash {
  public:
	using signature_t =  XXH128_hash_t;

  private:
	const T *string;
	Vector<XXH3_state_t *, AT> statetable;
	XXH3_state_t *state = XXH3_createState();

  public:

	XXH3Hash() : string(nullptr), statetable(1) {
		XXH3_128bits_reset(state);
		statetable[0] = XXH3_createState();
		XXH3_copyState(statetable[0], state);
	}

	XXH3Hash(const T *string, size_t size) : string(string) {
		XXH3_128bits_reset(state);
        statetable.reserve(size / C);
        statetable.pushBack(XXH3_createState());
		XXH3_copyState(statetable[0], state);
	}

	~XXH3Hash() {
		XXH3_freeState(state);
		for (size_t i = 0; i < statetable.size(); i++) XXH3_freeState(statetable[i]);
	}

    void setString(const T *s) {
        string = s;
        statetable.resize(1);
    }

    void reserve(size_t size) { statetable.reserve(size / C); }

	signature_t operator()(size_t to) {
		const uint8_t *str = reinterpret_cast<const uint8_t *>(string);
		const size_t length = to * sizeof(T);

		const size_t tpos = length / C;
		if (statetable.size() <= tpos) {
			const size_t last = statetable.size() - 1;
			XXH3_copyState(state, statetable[last]);
			statetable.resize(tpos + 1);
			for (size_t i = last + 1; i <= tpos; i++) {
				XXH3_128bits_update(state, str + (i - 1) * C, C);
				statetable[i] = XXH3_createState();
				XXH3_copyState(statetable[i], state);
			}
		}

		XXH3_copyState(state, statetable[tpos]);
		if (length % C) XXH3_128bits_update(state, str + tpos * C, length % C);
		return XXH3_128bits_digest(state);
	}

	signature_t operator()(size_t from, size_t length) { return immediate(from, length); }

	signature_t immediate(size_t from, size_t length) { return XXH3_128bits(string + from, length * sizeof(T)); }

	size_t bitCount() const {
		return sizeof(*this) * 8
			+ statetable.bitCount() - sizeof(statetable) * 8
			+ statetable.size() * 4608; // 4608 = sizeof(XXH3_state_t) * 8
	}
};

} // namespace zarr
