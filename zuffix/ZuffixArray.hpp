#pragma once

#include "util/Interval.hpp"
#include "util/String.hpp"

#include <limits>
#include <sais.hxx>
#include <stack>
#include <sux/support/common.hpp>
#include <sux/util/Vector.hpp>
#include <unordered_map>

namespace zarr {
using ::sux::util::Vector;

// Hash function type: it is not used as a function pointer, the compiler can perform inlining
typedef uint64_t (*hash_function)(const void *, size_t);

template <typename T, hash_function Hash> class ZuffixArray {
private:
  const String<T> string;
  const Vector<size_t> sa, lcp;
  Vector<size_t> up, down, next;
  std::unordered_map<size_t, size_t> zmap;

public:
  explicit ZuffixArray(String<T> string) : ZuffixArray(std::move(string), 0) {}

  ZuffixArray(String<T> str, uint64_t seed)
      : string(std::move(str)), sa(buildSuffixArray(string)), lcp(buildLCP(string, sa)),
        up(string.length()), down(string.length()), next(string.length()) {
    assert(string[string.length() - 1] == string.DOLLAR && "The string is not prefix free");
    std::stack<size_t> stack;
    stack.push(0);

    size_t last = -1ULL;
    for (size_t i = 1; i < string.length(); i++) {
      while (lcp[i] < lcp[stack.top()]) {
        last = stack.top();
        stack.pop();
        size_t top = stack.top();

        if (lcp[i] <= lcp[top] && lcp[top] != lcp[last]) down[top] = last;
      }

      if (last != -1ULL) {
        up[i] = last;
        last = -1ULL;
      }

      stack.push(i);
    }

    stack = std::stack<size_t>();
    stack.push(0);

    for (size_t i = 1; i < string.length(); i++) {
      while (lcp[i] < lcp[stack.top()])
        stack.pop();

      if (lcp[i] == lcp[stack.top()]) {
        last = stack.top();
        stack.pop();
        next[last] = i;
      }

      stack.push(i);
    }

    buildZMap(0, string.length() - 1, 0);
  }

  Vector<size_t> getChildren(size_t i, size_t j) const {
    Vector<size_t> res;
    res.pushBack(i);

    if (i == 0 && j == up.size() - 1) {
      int k = 0;
      while (next[k] != 0) {
        res.pushBack(next[k]);
        k = next[k];
      }
    } else {
      size_t k = up[j + 1];
      if (k <= i || k > j) k = down[i];
      res.pushBack(k);
      while (next[k] != 0) {
        res.pushBack(next[k]);
        k = next[k];
      }
    }

    // TODO: trimming has a performance cost, better not do it; check for a
    // suitable value to reserve space
    res.trimToFit();
    return res;
  }

  size_t extentLength(size_t node) const { return extentLength(node >> 32, node); }

  size_t extentLength(uint32_t i, uint32_t j) const {
    if (i == j) return string.length() - sa[i] - 1;
    if (i == 0 && j == up.size() - 1) return lcp[next[0]];

    size_t k = up[j + 1];
    if (j < k || k <= i) k = down[i];
    return lcp[k];
  }

  void visitPre(size_t i, size_t j, size_t name_len, size_t d) {
    using namespace std;
    cout << std::string(d, '\t') << "[" << i << ", " << j << "] ";

    assert((i == 0 && j == string.length() - 1 && name_len == 0) ||
           ((i != 0 || j != string.length() - 1) && name_len == 1 + max(lcp[i], lcp[j + 1])));

    if (i == j) {
      size_t extent_len = string.length() - sa[i];
      size_t handle_len = twoFattest(name_len - 1, extent_len);
      cout << " skip: "
           << "[" << name_len << ", " << extent_len << "] ";

      if (name_len != extent_len) {
        cout << "(" << handle_len << ") ";

        for (size_t k = sa[i]; k < sa[i] + handle_len; k++)
          cout << string[k];
      }

      cout << endl;
    } else {
      auto children = getChildren(i, j);
      size_t extent_len = lcp[children[1]];
      size_t handle_len = twoFattest(name_len - 1, extent_len);

      cout << " skip: "
           << "[" << name_len << ", " << extent_len << "] "
           << "(" << handle_len << ") ";

      for (size_t k = sa[i]; k < sa[i] + handle_len; k++)
        cout << string[k];

      cout << endl;

      for (size_t p = 0; p < children.size() - 1; p++)
        visitPre(children[p], children[p + 1] - 1, extent_len + 1, d + 1);

      visitPre(children[children.size() - 1], j, extent_len + 1, d + 1);
    }
  }

  size_t fatBinarySearch(const String<T> &v, int64_t from, int64_t to) const {
#ifdef DEBUG
    std::cout << "fatBinarySearch([ ";
    for (size_t i = 0; i < v.length(); i++)
      std::cout << v[i] << " ";
    std::cout << "], (" << from << ".." << to << "))" << std::endl;
#endif
    to--;

    assert(from < to && "from < to");

    size_t top = -1ULL;
    if (from == -1) {
      // ALERT: this should return NULL in certain cases
      top = string.length(); // root
      from = 0;            // extent length
    }

    int64_t check_mask = -1ULL << sux::ceil_log2(to - from);

    while (to - from > 0) {
      assert(check_mask != 0);

#ifdef DEBUG
      std::cout << "(" << from << ".." << (to + 1) << ")" << std::endl;
#endif

      int64_t f = to & check_mask;

      if ((from & check_mask) != f) {
        auto it = zmap.find(Hash(&v, f));
        size_t node = it == std::end(zmap) ? -1ULL : it->second;
        size_t g;

        if (node == -1ULL || (g = extentLength(node)) < f) {
#ifdef DEBUG
          std::cout << "Missing" << std::endl;
#endif
          to = f - 1;
        } else {
#ifdef DEBUG
          std::cout << "Found extent of length " << g << std::endl;
#endif
          top = node;
          from = g;
        }
      }

      check_mask >>= 1;
    }

#ifdef DEBUG
    std::cout << "Final interval: (" << from << ".." << (to + 1) << "); top: ["
              << interval(top).first << ", " << interval(top).second << "]";
#endif
    return top;
  }

  Interval<size_t> findExact(const String<T> &v) const {
    size_t left = 0;
    size_t right = string.length() - 1;
    size_t m = v.length();

    for (size_t i = 0;; i++) {
      size_t extent_len = extentLength(left, right);
      size_t limit = std::min(m, extent_len);

      while (i < limit && v[i] == string[sa[left] + i])
        i++;

      if (i == m) return Interval(left, right);
      if (i < extent_len) return Interval<size_t>::empty();

      assert(i == extent_len && "i != extent_len");
      assert(m > extent_len && "m > extent_len");

      if (left == right) return Interval<size_t>::empty();

      auto children = getChildren(left, right);

      size_t c;
      for (c = 0; c < children.size(); c++) {
        if (c == children.size() - 1 && children[c] == right &&
            extent_len + 1 == string.length() - sa[children[c]]) {
#ifdef DEBUG
          std::cout << "Skipping fake child: (" << right << ", " << right << ")" << std::endl;
#endif
          continue;
        }

        if (string[sa[children[c]] + extent_len] == v[i]) break;
      }

      if (c == children.size()) return Interval<size_t>::empty();

      left = children[c];
      if (c != children.size() - 1) right = children[c + 1] - 1;
    }
  }

  Interval<size_t> find(const String<T> &v) const {
    size_t m = v.length();
    size_t pos = fatBinarySearch(v, -1ULL, m);
    size_t left = pos >> 32;
    size_t right = pos & 0xFFFFFFFF;

    size_t extent_len = extentLength(left, right);
    size_t name_len = pos == string.length() - 1 ? 0 : 1 + std::max(lcp[left], lcp[right + 1]);

    size_t limit = std::min(m, extent_len);

    //size_t i;
    //for (i = 0; i < limit; i++) {
    //if (v[i] != string[sa[left] + i]) break;
    //}

    size_t i = 0;
    while (i < limit && v[i] == string[sa[left] + i])
      i++;

    if (i < name_len) return findExact(v);

    if (i == m) return Interval(left, right);
    if (i < extent_len) return Interval<size_t>::empty();

    auto children = getChildren(left, right);

    size_t c;
    for (c = 0; c < children.size(); c++) {
      if (c == children.size() - 1 && children[c] == right &&
          extent_len + 1 == string.length() - sa[children[c]]) {
#ifdef DEBUG
        std::cout << "Skipping fake child: (" << right << ", " << right << ")" << std::endl;
#endif
        continue;
      }

      if (string[sa[children[c]] + extent_len] == v[i]) break;
    }

    if (c == children.size()) return Interval<size_t>::empty();

    size_t child_left = children[c];
    size_t child_right = c == children.size() - 1 ? right : children[c + 1] - 1;
    size_t start = sa[child_left];
    size_t child_extent_lenght = extentLength(child_left, child_right);

    size_t child_limit = std::min(m, child_extent_lenght);
    for (i++; i < child_limit; i++) {
      if (v[i] != string[start + i]) break;
    }

    if (i == m) return Interval(child_left, child_right);
    if (i < m) return Interval<size_t>::empty();

    assert(i == child_extent_lenght && "i == child_extent_lenght");
    assert(m > child_extent_lenght && "m > child_extent_lenght");
    return findExact(v);
  }

  const T *getString() const { return &string; }

  const size_t *getSuffix() const { return &sa; }

  const size_t *getLCP() const { return &lcp; }

private:
  static Vector<size_t> buildSuffixArray(const String<T> &string) {
    // One of the fastest linear time algorithm: https://sites.google.com/site/yuta256/sais
    Vector<size_t> result(string.length());
    int ret = saisxx((T *)&string, (long *)&result, (long)string.length(), 256L);
    assert(ret == 0 && "Error occurred during suffix array construction");
    return result;
  }

  static Vector<size_t> buildLCP(const String<T> &string, const Vector<size_t> &sa) {
    Vector<size_t> lcp(sa.size() + 1);
    size_t length = sa.size();

    for (size_t i = 1; i < length; i++) {
      while (sa[i - 1] + lcp[i] < length - 1 && sa[i] + lcp[i] < length - 1 &&
             string[sa[i - 1] + lcp[i]] == string[sa[i] + lcp[i]])
        lcp[i]++;
    }

    return lcp;
  }

  void buildZMap(size_t i, size_t j, size_t name_len) {
    if (i == j) return;

    assert((i == 0 && j == string.length() - 1 && name_len == 0) ||
           ((i != 0 || j != string.length() - 1) && name_len == 1 + std::max(lcp[i], lcp[j + 1])));

    auto children = getChildren(i, j);
    size_t extent_len = lcp[children[1]];
    size_t handle_len = twoFattest(name_len - 1, extent_len);

    auto [it, res] = zmap.insert(std::pair(Hash(&string + sa[i], handle_len), interval(i, j)));

    assert(res && "The element alaredy exists");

    for (size_t p = 0; p < children.size() - 1; p++)
      buildZMap(children[p], children[p + 1] - 1, extent_len + 1);

    buildZMap(children[children.size() - 1], j, extent_len + 1);
  }

  int64_t twoFattest(size_t a, size_t b) { return (1L << 63) >> __builtin_clzll(a ^ b) & b; }

  static Interval<size_t> interval(size_t pos) { return Interval(pos >> 32, pos); }

  static size_t interval(size_t i, size_t j) { return i << 32 | j; }
};

} // namespace zarr
