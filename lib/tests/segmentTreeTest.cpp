#include "gtest/gtest.h"

#include "lib/segmentTree.h"

TEST(SegmentTree, Max) {
    static constexpr int kSize = 12345;
    IntVector data(kSize);
    for (auto& n: data) {
        n = rand();
    }
    SegmentTree<int, Max<int>> st(data);
    for (size_t i = 0; i < kSize; ++i) {
        int left0 = rand() % kSize;
        int right0 = rand() % kSize;
        auto left = min(left0, right0);
        auto right = max(left0, right0);

        if (left < right) {
            int res = data[left];
            for (int i = left + 1; i < right; ++i) {
                res = max(res, data[i]);
            }

            EXPECT_EQ(res, st.get(left, right));
        }
    }
}
