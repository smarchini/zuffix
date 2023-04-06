#pragma once

#include <optional>
#include <tuple>

#include "../hash/XXH3Hash.hpp"
#include <sux/util/Vector.hpp>

namespace zarr {
using ::sux::util::Vector;
using ::sux::util::AllocType;

template <typename S, typename T, AllocType AT = MALLOC> class LinearProber {
  private:
	Vector<std::tuple<S, T>, AT> table;
	size_t mask, count = 0;

  public:
	LinearProber() : LinearProber(32) {}

	LinearProber(size_t size) : table(round_pow2(size)), mask(round_pow2(size) - 1) {}

	LinearProber(LinearProber<S, T, AT> &old, size_t size) : table(round_pow2(size)), mask(round_pow2(size) - 1) {
		size_t oldsize = old.size();
		for (size_t i = 0; i < oldsize; i++) {
			auto [signature, value] = old.table[i];
			if (!isEmpty(signature)) store(signature, value);
		}
	}

	// Delete copy operators
	LinearProber(const LinearProber &) = delete;
	LinearProber &operator=(const LinearProber &) = delete;

	// Define move operators
	LinearProber(LinearProber<S, T, AT> &&oth)
		: table(std::exchange(oth.table, Vector<T, AT>())),
		  mask(std::exchange(oth.mask, 0)),
		  count(std::exchange(oth.count, 0)) {}

	LinearProber<S, T, AT> &operator=(LinearProber<S, T, AT> &&oth) {
		swap(*this, oth);
		return *this;
	}

	void store(S signature, T value) {
		DEBUGDO(_store++);
		count++;
		size_t pos = position(signature);
		DEBUGDO(for (size_t _i = 0; std::get<0>(table[(pos + _i)]) != signature && !isEmpty(std::get<0>(table[(pos + _i)])); _i++) if (_i > _store_maxstride) _store_maxstride = _i);
		while (std::get<0>(table[pos]) != signature && !isEmpty(std::get<0>(table[pos]))) pos = (pos + 1) & mask;
		DEBUGDO(if (std::get<0>(table[pos]) == signature) _store_collisions++);
		table[pos] = {signature, value};
	}

	std::optional<T> operator[](S signature) {
		DEBUGDO(_get++);
		size_t pos = position(signature);
		while (std::get<0>(table[pos]) != signature && !isEmpty(std::get<0>(table[pos]))) pos = (pos + 1) & mask;
		DEBUGDO(if (std::get<0>(table[pos]) == signature) _get_found++; else _get_notfound++);
		return std::get<0>(table[pos]) == signature ? std::optional(std::get<1>(table[pos])) : std::nullopt;
	};

	size_t size() const { return table.size(); }

	void resize(size_t size) {
		DEBUGDO(_resize++);
		size = round_pow2(size);
		table.resize(size);
		mask = size - 1;
	}

	size_t bitCount() const { return sizeof(*this) * 8 + table.bitCount() - sizeof(table) * 8; }

	size_t elements() { return count; }

	friend void swap(LinearProber<S, T, AT> &first, LinearProber<S, T, AT> &second) noexcept {
		std::swap(first.table, second.table);
		std::swap(first.mask, second.mask);
		std::swap(first.count, second.count);
	}

  private:
	// TODO: This is ugly, LinearProber shouldn't know aobut XXH128_hash_t
	size_t position(S signature) {
		if constexpr (std::is_same<S, XXH128_hash_t>::value) {
			return signature.low64 & mask;
		} else {
			return signature & mask;
		}
	}

	bool isEmpty(S signature) {
		if constexpr (std::is_same<S, XXH128_hash_t>::value) {
			return signature.low64 == 0 && signature.high64 == 0;
		} else {
			return signature == 0;
		}
	}

	friend std::ostream &operator<<(std::ostream &os, const LinearProber<S, T, AT> &ds) { return os << ds.table << ds.mask << ds.count; }
	friend std::istream &operator>>(std::istream &is, LinearProber<S, T, AT> &ds) { return is >> ds.table >> ds.mask >> ds.count; }

#ifdef DEBUG
  public:
	int _store = 0;
	int _get = 0;
	int _resize = 0;

	int _store_maxstride = 0;
	int _store_collisions = 0;

	int _get_notfound = 0;
	int _get_found = 0;

	void reset_stats() {
		_store = 0;
		_get = 0;
		_resize = 0;

		_store_maxstride = 0;
		_store_collisions = 0;

		_get_notfound = 0;
		_get_found = 0;
	}

	void print_stats(const char *msg = "") {
		std::cerr << "--------------------------------------------------------------------------------" << std::endl;
		std::cerr << "LinearProber.hpp: " << msg << std::endl;
		std::cerr << "- table.size(): " << table.size() << std::endl;
		std::cerr << "- store: " << _store << std::endl;
		std::cerr << "- get: " << _get << std::endl;
		std::cerr << "- resize: " << _resize << std::endl;
		std::cerr << "- store_maxstride: " << _store_maxstride << std::endl;
		std::cerr << "- store_collisions: " << _store_collisions << std::endl;
		std::cerr << "- get_notfound: " << _get_notfound << std::endl;
		std::cerr << "- get_found: " << _get_found << std::endl;
		std::cerr << "--------------------------------------------------------------------------------" << std::endl;
	}
#endif
};

} // namespace zarr
