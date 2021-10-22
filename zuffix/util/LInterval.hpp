#pragma once

#include <iostream>
#include <utility>

namespace zarr {

/** Immutable pair of values to represent an interval [from, to).
 *
 * @tparam T the data type of the interval.
 */
template <class T> class LInterval {
  private:
  public:
	const T from, to;
	// explicit LInterval(T point) : LInterval(point, point + 1) {}
	LInterval(T from, T to) : from(std::move(from)), to(std::move(to)) {}

	constexpr LInterval() = default;
	constexpr LInterval(LInterval &&) = default;
	constexpr LInterval(const LInterval &) = default;
	LInterval<T> &operator=(LInterval<T> &&) = default;
	LInterval<T> &operator=(const LInterval<T> &) = default;

	// T getFrom() const { std::cout << "pippo" << std::endl; return from; }
	// T getTo() const { std::cout << "pippo" << std::endl; return to; }

	/** Length of the interval */
	T length() const { return to - from; }

	/** Check if the interval is empty
	 *
	 * @return true if the interval is empty, false otherwise
	 */
	bool isEmpty() const { return to <= from; }

	/** Check if the interval is a point interval
	 *
	 * @return true if the interval is in a singleton, false otherwise
	 */
	bool isSingleton() const { return from + 1 == to; }

	// TODO Convert (from and to) different kind of intervals and compare

	/** Check if two interval are equals. Two empty intervals are always equals.
	 *
	 * @return true if the other interval is equals to this, false otherwise.
	 */
	bool operator==(const LInterval<T> &oth) const {
		if (isEmpty() && oth.isEmpty()) return true;
		return from == oth.from && to == oth.to;
	}

	/** Check if two interval are different.
	 *
	 * @return false if the other interval is equals to this, true otherwise.
	 */
	bool operator!=(const LInterval<T> &oth) const { return !(this == oth); }

	/** Serialize the interval in a human-readable format */
	friend std::ostream &operator<<(std::ostream &os, const LInterval<T> &interval) { return os << "[" << interval.from << ", " << interval.to << ")"; }

	// WARNING dirty hack, maybe undefined behavior
	operator std::tuple<T &, T &>() { return std::tie(const_cast<T &>(from), const_cast<T &>(to)); }
};

} // namespace zarr
