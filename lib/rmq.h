#pragma once

#include "lib/header.h"

#include "glog/logging.h"

#include <type_traits>

template <typename T>
size_t logCeil(T n);

template <>
inline size_t logCeil<u32>(u32 n) {
    return 32 - __builtin_clz(n);
}

template <>
inline size_t logCeil<u64>(u64 n) {
    return 64 - __builtin_clzl(n);
}

template <typename T>
struct MinF {
    bool operator()(const T& a, const T& b) { return a < b; }
};

template <typename T>
struct MaxF {
    bool operator()(const T& a, const T& b) { return a > b; }
};

template <typename It, typename F>
class RangeAggQuery {
   private:
    using T = typename std::remove_reference<It>::type;

    static It cmp(It a, It b) {
        if (F{}(*a, *b)) {
            return a;
        } else {
            return b;
        }
    }

   public:
    RangeAggQuery(It begin, It end) : begin_(begin), end_(end) {
        const size_t n = end - begin;
        const size_t nLevels = std::max<size_t>(1, logCeil(n));
        agg_.resize(nLevels);
        for (size_t i = 0; i < nLevels; ++i) {
            agg_[i].resize(n);
        }
        for (It i = begin; i != end; ++i) {
            agg_[0][i - begin] = i;
        }
        for (size_t level = 1; level < nLevels; ++level) {
            const size_t gap = 1ULL << (level - 1);
            for (It i = begin; i != end; ++i) {
                if (i + gap + gap < end) {
                    agg_[level][i - begin] = cmp(agg_[level - 1][i - begin], agg_[level - 1][i + gap - begin]);
                } else {
                    agg_[level][i - begin] = agg_[level - 1][i - begin];
                }
            }
        }
    }

    It agg(It begin, It end) const {
        size_t diff = end - begin;
        size_t level = std::log2(diff);
        auto second = end - (1ULL << (level));
        LOG(INFO) << OUT(diff) << OUT(level) << OUT(second - begin_) << OUT(agg_.size()) << OUT(agg_[level].size());
        return cmp(agg_[level][begin - begin_], agg_[level][second - begin_]);
    }

   private:
    using VectorIt = std::vector<It>;
    using VectorVectorIt = std::vector<VectorIt>;

    It begin_;
    It end_;
    VectorVectorIt agg_;
};
