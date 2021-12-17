#pragma once

#include <optional>
#include <tuple>

#include <sux/util/Vector.hpp>

namespace zarr {

// TODO l'interfaccia fa abbastanza schifo
template <typename T, sux::util::AllocType AT = sux::util::MALLOC> class OpenAddressing {
  private:
	sux::util::Vector<std::tuple<uint64_t, T>> table;

  public:
	OpenAddressing() : OpenAddressing(1024) {}
	OpenAddressing(size_t size) : table(size) {}

	void store(uint64_t signature, T value) {
		assert(signature != 0);
		store_calls++;
		assert(store_calls <= table.size() / 2);
		size_t pos = signature % table.size();
		while (std::get<0>(table[pos]) != signature && std::get<0>(table[pos]) != 0) {
			store_duplicate++;
			pos = (pos + 1) % table.size();
		}

		if (std::get<0>(table[pos]) == signature) store_collision++;
		table[pos] = {signature, value};
	}

	std::optional<T> operator[](uint64_t signature) {
		assert(signature != 0);
		get_calls++;
		size_t pos = signature % table.size();
		while (std::get<0>(table[pos]) != signature && std::get<0>(table[pos]) != 0) {
			get_duplicate++;
			pos = (pos + 1) % table.size();
		}
		if (std::get<0>(table[pos]) == signature)
			get_okay++;
		else
			get_null++;
		return std::get<0>(table[pos]) == signature ? std::optional(std::get<1>(table[pos])) : std::nullopt;
	};

	size_t size() const { return table.size(); }
	void resize(size_t size) { table.resize(size); }

  private:
	friend std::ostream &operator<<(std::ostream &os, const OpenAddressing<T, AT> &ds) { return os << ds.table; }
	friend std::istream &operator>>(std::istream &is, OpenAddressing<T, AT> &ds) { return is >> ds.table; }

  public:
	int store_calls = 0;
	int store_duplicate = 0;
	int store_collision = 0;
	int get_calls = 0;
	int get_duplicate = 0;
	int get_null = 0;
	int get_okay = 0;

	void print_debug_stats() {
		std::cerr << "DEBUG_STATS_OpenAddressing.hpp: " << std::endl;
		std::cerr << "- table.size(): " << table.size() << std::endl;
		std::cerr << "- store_calls: " << store_calls << std::endl;
		std::cerr << "- store_duplicate: " << store_duplicate << std::endl;
		std::cerr << "- store_collision: " << store_collision << std::endl;
		std::cerr << "- get_calls: " << get_calls << std::endl;
		std::cerr << "- get_duplicate: " << get_duplicate << std::endl;
		std::cerr << "- get_null: " << get_null << std::endl;
		std::cerr << "- get_okay: " << get_okay << std::endl;
	}

	void reset_debug_stats() {
		store_calls = 0;
		store_duplicate = 0;
		store_collision = 0;
		get_calls = 0;
		get_duplicate = 0;
		get_null = 0;
		get_okay = 0;
	}
};

} // namespace zarr
