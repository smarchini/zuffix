#include <zuffix/util/LInterval.hpp>
#include <zuffix/util/LinearProber.hpp>
#include <zuffix/util/common.hpp>

#include <sdsl/suffix_tree_algorithm.hpp>
#include <sdsl/suffix_trees.hpp>

namespace zarr {

//  WARNING: BE CAREFUL ABOUT A FEW SUBTLE DIFFERENCES
//  1) strings ends in 0 instead than in numeric_limits<T>::max, thus in SDSL
//     the value of SA & LCP is different than in our implementation;
//  2) closed ranges (i.e.,  [l .. r]) instead than half-open (ie., [l .. r));
//  3) the length of LCP is n - 1, so it doesn't start and ends for -1.

sdsl::cst_sct3 dummything;
using sdsl_sct3 = decltype(dummything); // TODO: I don't know how to do it yet

template <class cst_t, typename T, template <typename U> class RH> class MemcmpZSdsl {
    cst_t cst;
    RH<T> hpattern;
    LinearProber<typename RH<T>::signature_t, std::tuple<typename cst_t::size_type, size_t, size_t, typename decltype(cst)::size_type>> z;
    size_t maxnlen = 0, maxhlen = 0;

  public:
    MemcmpZSdsl(std::string filename, const std::span<const T> string) {
        sdsl::load_from_file(cst, filename);
        RH<T> htext(string.data(), string.size());

        // typedef sdsl::cst_dfs_const_forward_iterator<cst_t> iterator;
        typedef sdsl::cst_bfs_iterator<cst_t> iterator;
        iterator begin = iterator(&cst, cst.root());
        iterator end = iterator(&cst, cst.root(), true, true);

        for (iterator it = begin; it != end; ++it) {
            size_t i = cst.lb(*it), j = cst.rb(*it) + 1;
            if (j - i <= 1) continue;

            ssize_t nlen = 1 + (j == cst.lcp.size() ? cst.lcp[i] : std::max(cst.lcp[i], cst.lcp[j]));
            ssize_t elen = cst.depth(*it);
            size_t hlen = twoFattestLR(nlen, elen);

            if (maxnlen <= nlen) maxnlen = nlen;
            if (maxhlen <= hlen) maxhlen = hlen;

            auto char_pos = hlen > 0 ? sdsl::get_char_pos((*it).i, hlen - 1, cst.csa) : 0;
            // NOTE: Storing betaid (i.e., typename cst_t::size_type) is sufficient, storing "everything" is faster
            z.store(htext(cst.csa[i], hlen), std::tuple<typename cst_t::size_type, size_t, size_t, typename cst_t::size_type>(cst.id(*it), nlen, elen, char_pos));
            if (z.elements() * 3 / 2 > z.size())
                z = LinearProber<typename RH<T>::signature_t, std::tuple<typename cst_t::size_type, size_t, size_t, typename decltype(cst)::size_type>>(z, z.size() * 2);
        }
    }

    std::tuple<typename cst_t::node_type, size_t, typename decltype(cst)::size_type> fatBinarySearch(std::span<const T> pattern) {
        auto alpha = cst.root();
        size_t alphanlen = 0, alphaelen = 0;
        typename decltype(cst)::size_type alpha_char_pos = 0;
        size_t l = 0, r = std::min(pattern.size(), maxnlen);
        int64_t m = -1ULL << (lambda(l ^ r) + 1);
        while (l < r) {
            while ((m & l) == (m & r)) m >>= 1;
            size_t f = m & r;
            assert(f == twoFattestR(l, r) && "wrong 2-fattest number");
            auto betaopt = z[hpattern(f)];
            if (betaopt == std::nullopt) {
                r = f - 1;
                continue;
            }
            auto [betaid, nlen, elen, char_pos] = *betaopt;
            auto beta = cst.inv_id(betaid);
            // NOTE: If your z-map only stores betaid you can use the following:
            // size_t nlen = 1 + (beta.j + 1 == cst.lcp.size() ? cst.lcp[beta.i] : std::max(cst.lcp[beta.i], cst.lcp[beta.j + 1]));
            // size_t elen = cst.depth(beta) + 1;
            size_t hlen = twoFattestLR(nlen, elen);
            if (hlen != f) {
                r = f - 1;
            } else if (alpha == beta || !(alpha.i <= beta.i && beta.j <= alpha.j)) {
                l = elen;
            } else {
                l = elen - 1;
                alpha = beta;
                alphanlen = nlen;
                alphaelen = elen;
                alpha_char_pos = char_pos;
            }
        }
        size_t hlen = zarr::twoFattestLR(alphanlen, alphaelen);
        size_t end = std::min(hlen, pattern.size());
        return {alpha, end, alpha_char_pos};
    }

    size_t count_forward(std::span<const T> pattern) {
        auto v = cst.root();
        typename decltype(cst)::size_type char_pos = 0;
        return sdsl::forward_search<decltype(cst)>(cst, v, 0, std::begin(pattern), std::end(pattern), char_pos);
    }

    size_t count_backward(std::span<const T> pattern) {
        size_t dummyl, dummyr;
        return sdsl::backward_search(cst.csa, 0, cst.csa.size() - 1, std::begin(pattern), std::end(pattern), dummyl, dummyr);
    }

    size_t zfast_count_forward(std::span<const T> pattern) {
        hpattern.setString(pattern.data());
        auto [v, skip, char_pos] = fatBinarySearch(pattern);
        // NOTE: If your z-map only stores betaid you can use the following:
        // typename decltype(cst)::size_type char_pos = skip == 0 ? 0 : sdsl::get_char_pos(v.i, skip - 1, cst.csa);
        typename decltype(cst)::size_type d = skip;
        return sdsl::forward_search<decltype(cst)>(cst, v, d, std::begin(pattern) + skip, std::end(pattern), char_pos);
    }
};

} // namespace zarr
