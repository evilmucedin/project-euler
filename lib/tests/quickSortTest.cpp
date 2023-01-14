#include <lib/quickSort.h>
#include <lib/header.h>

#include <gtest/gtest.h>

TEST(QuickSort, Random) {
    for (int size: {0, 1, 10, 100, 1000}) {
        for (int i = 0; i < 10; ++i) {
            vector<int> v(size);
            for (auto& j: v) {
                j = rand();
            }
            vector<int> copy = v;
            quickSort(v.data(), size);
            sort(copy);
        }
    }
}
