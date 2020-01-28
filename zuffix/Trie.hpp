#pragma once

#include "util/Interval.hpp"

#include <sais.hxx>
#include <algorithm>
#include <limits>
#include <memory>
#include <set>
#include <stack>
#include <sux/support/common.hpp>
#include <sux/util/Vector.hpp>
#include <type_traits>
#include <unordered_map>

namespace zarr {

class ZTrieNode {
public:
  typedef char edge_type;

private:
  std::string value;
  std::unordered_map<edge_type, ZTrieNode> children;

public:
  ZTrieNode(std::string string) : value(std::move(string)) {}

  void addChild(edge_type edge, ZTrieNode node) {
    auto [it, res] = children.insert(std::pair(edge, std::move(node)));
    assert(res && "The element alaredy exists");
  }
};

// Hash function type: it is not used as a function pointer, the compiler can perform inlining
typedef uint64_t (*hash_function)(const void *, size_t);

template <hash_function Hash> class ZTrie {
private:
  std::unique_ptr<ZTrieNode> root;

public:
  template <typename iterator_type> ZTrie(iterator_type it, iterator_type end) {
    while (it != end) {

    }
  }
};

} // namespace zarr
