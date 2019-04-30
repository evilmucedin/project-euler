#pragma once

#include "lib/header.h"

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

template <typename T, typename F>
class RangeMQuery {
   public:
    template <typename It>
    RangeMQuery(It begin, It end) {
        const size_t n = end - begin;
        const size_t nLevels = std::max<size_t>(1, logCeil(n));
        agg_.resize(nLevels);
    }

   private:
    using VectorT = std::vector<T>;
    using VectorVectorT = std::vector<VectorT>;

    VectorVectorT agg_;
};
