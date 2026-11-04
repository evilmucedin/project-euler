#include "lib/ann/vectorSet.h"

#include "gtest/gtest.h"

#include <cstdint>
#include <stdexcept>
#include <vector>

TEST(VectorSet, PadsRowsToCacheLines) {
    ann::VectorSet set(5);
    EXPECT_EQ(set.dim(), 5u);
    EXPECT_EQ(set.stride(), 16u);
    EXPECT_TRUE(set.empty());
    const std::vector<float> a = {1, 2, 3, 4, 5};
    const std::vector<float> b = {6, 7, 8, 9, 10};
    EXPECT_EQ(set.add(a), 0u);
    EXPECT_EQ(set.add(b.data()), 1u);
    EXPECT_EQ(set.size(), 2u);
    for (ann::Id i = 0; i < 2; ++i) {
        EXPECT_EQ(reinterpret_cast<std::uintptr_t>(set[i]) % ann::kAlignment, 0u);
        for (std::size_t d = set.dim(); d < set.stride(); ++d) {
            EXPECT_EQ(set[i][d], 0.0f) << "row " << i << " pad " << d;
        }
    }
    EXPECT_EQ(set[1][4], 10.0f);
    EXPECT_EQ(set.bytes(), 2 * 16 * sizeof(float));
}

TEST(VectorSet, ExactMultipleOfLineIsNotPadded) {
    ann::VectorSet set(32);
    EXPECT_EQ(set.stride(), 32u);
    ann::VectorSet set2(33);
    EXPECT_EQ(set2.stride(), 48u);
}

TEST(VectorSet, BulkConstructionAndAppend) {
    const std::vector<float> data = {1, 2, 3, 4, 5, 6};
    ann::VectorSet set(2, 3, data.data());
    ASSERT_EQ(set.size(), 3u);
    EXPECT_EQ(set[2][0], 5.0f);
    EXPECT_EQ(set[2][1], 6.0f);
    ann::VectorSet other(2, 1, data.data());
    set.append(other);
    EXPECT_EQ(set.size(), 4u);
    EXPECT_EQ(set[3][1], 2.0f);
    set.clear();
    EXPECT_TRUE(set.empty());
}

TEST(VectorSet, RejectsBadInput) {
    EXPECT_THROW(ann::VectorSet(0), std::invalid_argument);
    ann::VectorSet set(3);
    EXPECT_THROW(set.add(std::vector<float>{1.0f}), std::invalid_argument);
    ann::VectorSet other(4);
    EXPECT_THROW(set.append(other), std::invalid_argument);
}

TEST(VectorSet, ReserveKeepsRowsStable) {
    ann::VectorSet set(3);
    set.reserve(100);
    EXPECT_GE(set.capacity(), 100u);
    const std::vector<float> v = {1, 2, 3};
    set.add(v);
    const float* first = set[0];
    for (int i = 0; i < 99; ++i) {
        set.add(v);
    }
    EXPECT_EQ(set[0], first);
}

TEST(AlignedQuery, CopiesPadsAndNormalises) {
    ann::VectorSet set(3);
    const std::vector<float> q = {3.0f, 0.0f, 4.0f};
    ann::AlignedQuery plain = ann::AlignedQuery::forMetric<ann::L2>(q.data(), set);
    EXPECT_EQ(plain.data()[0], 3.0f);
    EXPECT_EQ(plain.data()[2], 4.0f);
    EXPECT_EQ(plain.data()[15], 0.0f);
    EXPECT_EQ(reinterpret_cast<std::uintptr_t>(plain.data()) % ann::kAlignment, 0u);
    ann::AlignedQuery unit = ann::AlignedQuery::forMetric<ann::Cosine>(q.data(), set);
    EXPECT_NEAR(unit.data()[0], 0.6f, 1e-6f);
    EXPECT_NEAR(unit.data()[2], 0.8f, 1e-6f);
}
