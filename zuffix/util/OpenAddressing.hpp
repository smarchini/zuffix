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
		size_t pos = signature % table.size();
		while (std::get<0>(table[pos]) != signature && std::get<0>(table[pos]) != 0) pos = (pos + 1) % table.size();
		table[pos] = {signature, value};
	}

	std::optional<T> operator[](uint64_t signature) {
		size_t pos = signature % table.size();
		while (std::get<0>(table[pos]) != signature && std::get<0>(table[pos]) != 0) pos = (pos + 1) % table.size();
		return std::get<0>(table[pos]) == signature ? std::optional(std::get<1>(table[pos])) : std::nullopt;
	};

	size_t size() const { return table.size(); }
	void resize(size_t size) { table.resize(size); }
};

} // namespace zarr
