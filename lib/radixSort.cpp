#include "lib/radixSort.h"

void radixSort(U32Vector& vct) {
    U32Vector counts((1 << 8) + 1);
    U32Vector temp(vct.size());
    for (auto shift : {0, 8, 16, 24}) {
        const u32 mask = ((1UL << 8) - 1) << shift;
        fill(counts.begin(), counts.end(), 0);
        for (auto v: vct) {
            auto bucket = ((v & mask) >> shift) + 1;
            ++counts[bucket];
        }
        for (int i = 1; i <= (1 << 8); ++i) {
            counts[i] += counts[i - 1];
        }
        for (size_t i = 0; i < vct.size(); ++i) {
            auto bucket = (vct[i] & mask) >> shift;
            temp[ counts[bucket]++ ] = vct[i];
        }
        vct.swap(temp);
    }
}
