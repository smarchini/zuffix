#pragma once

#include <optional>
#include <tuple>

#include <sux/util/Vector.hpp>

namespace zarr {

template <typename T, sux::util::AllocType AT = sux::util::MALLOC> class LinearProber {
  private:
	sux::util::Vector<std::tuple<uint64_t, T>> table;
	size_t mask, count = 0;

  public:
	LinearProber() : LinearProber(1024) {}
	LinearProber(size_t size) : table(round_pow2(size)), mask(round_pow2(size) - 1) {}

	void store(uint64_t signature, T value) {
		count++;
		size_t pos = signature & mask;
		while (std::get<0>(table[pos]) != signature && std::get<0>(table[pos]) != 0) pos = (pos + 1) & mask;
		table[pos] = {signature, value};
	}

	std::optional<T> operator[](uint64_t signature) {
		size_t pos = signature & mask;
		while (std::get<0>(table[pos]) != signature && std::get<0>(table[pos]) != 0) pos = (pos + 1) & mask;
		return std::get<0>(table[pos]) == signature ? std::optional(std::get<1>(table[pos])) : std::nullopt;
	};

	size_t size() const { return table.size(); }

	void resize(size_t size) {
		size = round_pow2(size);
		table.resize(size);
		mask = size - 1;
	}

	size_t elements() { return count; }
	std::tuple<uint64_t, T> *getTable() { return &table; }

  private:
	friend std::ostream &operator<<(std::ostream &os, const LinearProber<T, AT> &ds) { return os << ds.table << ds.mask << ds.count; }
	friend std::istream &operator>>(std::istream &is, LinearProber<T, AT> &ds) { return is >> ds.table >> ds.mask >> ds.count; }
};

} // namespace zarr
