#include <zuffix/util/LInterval.hpp>
#include <zuffix/util/LinearProber.hpp>
#include <zuffix/util/common.hpp>

#include <sdsl/suffix_trees.hpp>

namespace zarr {

//  NOTE: BE CAREFUL ABOUT FEW SUBTLE DIFFERENCES
//  1) strings ends in 0 instead than in numeric_limits<T>::max, thus SA & LCP
//     hold different values than our implementation;
//  2) closed ranges (i.e.,  [l .. r]) instead than half-open (ie., [l .. r));
//  3) the length of LCP is n - 1, so it doesn't start and ends for -1.
//
sdsl::cst_sct3 dummything;
using sdsl_sct3 = decltype(dummything); // TODO: I don't know how to do it yet

template <class cst_t, typename T, template <typename U> class RH> class MemcmpZSdsl {
    cst_t cst;
    std::span<const T> text;
    RH<T> htext, hpattern;
    //LinearProber<typename RH<T>::signature_t, typename cst_t::size_type> z;
    LinearProber<typename RH<T>::signature_t, std::tuple<size_t, size_t, size_t>> z;
    size_t maxnlen = 0, maxhlen = 0;

  public:
    MemcmpZSdsl(std::string filename, const std::span<const T> string)
        : text(std::move(string)),
          htext(text.data(), text.size()) {
        std::cout << "filename = " << filename << "std::endl";
        sdsl::construct(cst, filename, sizeof(T)); // TODO: ugly ugly
        //sdsl::construct(cst, string.data(), sizeof(T)); // TODO: doesn't work

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

            //z.store(htext(cst.csa[i], hlen), cst.id(*it));
            z.store(htext(cst.csa[i], hlen), {i, j, elen});
            if (z.elements() * 3 / 2 > z.size()) {
                // z = LinearProber<typename RH<T>::signature_t, typename cst_t::size_type>(z, z.size() * 2);
                z = LinearProber<typename RH<T>::signature_t, std::tuple<size_t, size_t, size_t>>(z, z.size() * 2);
            }
        }
    }

    // TODO credo che sia qui l'errore
    std::tuple<size_t, size_t, size_t> fatBinarySearch(std::span<const T> pattern) {
        size_t alphai = 0, alphaj = text.size(), alphalen = 0;
        size_t l = 0, r = std::min(pattern.size(), maxnlen); // maxhlen
        int64_t m = -1ULL << (lambda(l ^ r) + 1);
        while (l < r) {
            while ((m & l) == (m & r)) m >>= 1;
            size_t f = m & r;
            assert(f == twoFattestR(l, r) && "wrong 2-fattest number");
            auto beta = z[hpattern(f)];
            if (beta == std::nullopt) {
                r = f - 1;
                continue;
            }
            auto [betai, betaj, betalen] = *beta;
            size_t nlen = 1 + (betaj == cst.lcp.size() ? cst.lcp[betai] : std::max(cst.lcp[betai], cst.lcp[betaj]));
            size_t elen = betalen;
            size_t hlen = twoFattestLR(nlen, elen);
            if (hlen != f) {
                r = f - 1;
            } else if (alphai <= betai && betaj <= alphaj && (alphai != betai || alphaj != betaj)) {
                l = elen;
            } else {
                l = elen - 1;
                alphai = betai; alphaj = betaj; alphalen = betalen;
            }
        }
        size_t nlen = 1 + (alphaj == cst.lcp.size() ? cst.lcp[alphai] : std::max(cst.lcp[alphai], cst.lcp[alphaj]));
        size_t elen = alphalen;
        size_t hlen = zarr::twoFattestLR(nlen, elen);
        size_t end = std::min(hlen, pattern.size());
        if (memcmp(pattern.data(), text.data() + cst.csa[alphai], end * sizeof(T))) {
            return {0, text.size(), 0};
        }
        return {alphai, alphaj, end}; // TODO: This can totally be hlen (instead of nlen) and maybe it should be!
    }

    size_t count_forward(std::span<const T> pattern) {
        size_t dummyl, dummyr;
        return sdsl::forward_search(cst.csa, 0, cst.csa.size() - 1, std::begin(pattern), std::end(pattern), dummyl, dummyr);
    }

    size_t count_backward(std::span<const T> pattern) {
        size_t dummyl, dummyr;
        return sdsl::backward_search(cst.csa, 0, cst.csa.size() - 1, std::begin(pattern), std::end(pattern), dummyl, dummyr);
    }

    size_t zfast_count_forward(std::span<const T> pattern) {
        size_t dummyl, dummyr;
        hpattern.setString(pattern.data());
        auto [i, j, skip] = fatBinarySearch(pattern);
        return sdsl::forward_search(cst.csa, i, j - 1, std::begin(pattern) + skip, std::end(pattern), dummyl, dummyr);
    }

    size_t zfast_count_backward(std::span<const T> pattern) {
        size_t dummyl, dummyr;
        hpattern.setString(pattern.data());
        auto [i, j, skip] = fatBinarySearch(pattern);
        return sdsl::backward_search(cst.csa, i, j - 1, std::begin(pattern) + skip, std::end(pattern), dummyl, dummyr);
    }
};

} // namespace zarr
