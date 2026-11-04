#include "lib/ann/common.h"
#include "lib/ann/topk.h"

#include "gtest/gtest.h"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <random>
#include <vector>

TEST(Neighbor, OrderingIsByDistanceThenId) {
    EXPECT_LT(ann::Neighbor(5, 1.0f), ann::Neighbor(1, 2.0f));
    EXPECT_LT(ann::Neighbor(1, 1.0f), ann::Neighbor(2, 1.0f));
    EXPECT_GT(ann::Neighbor(2, 1.0f), ann::Neighbor(1, 1.0f));
    EXPECT_EQ(ann::Neighbor(2, 1.0f), ann::Neighbor(2, 1.0f));
    EXPECT_NE(ann::Neighbor(2, 1.0f), ann::Neighbor(2, 1.5f));
}

TEST(AlignedAllocator, BlocksAreCacheLineAligned) {
    for (std::size_t n : {1u, 3u, 17u, 1000u}) {
        ann::AlignedVector<float> v(n, 1.0f);
        EXPECT_EQ(reinterpret_cast<std::uintptr_t>(v.data()) % ann::kAlignment, 0u);
        ann::AlignedVector<double> d(n);
        EXPECT_EQ(reinterpret_cast<std::uintptr_t>(d.data()) % ann::kAlignment, 0u);
    }
    EXPECT_EQ(ann::roundUp(0, 16), 0u);
    EXPECT_EQ(ann::roundUp(1, 16), 16u);
    EXPECT_EQ(ann::roundUp(16, 16), 16u);
    EXPECT_EQ(ann::roundUp(17, 16), 32u);
}

TEST(VisitedSet, ClearForgetsEverything) {
    ann::VisitedSet visited(10);
    EXPECT_FALSE(visited.test(3));
    EXPECT_FALSE(visited.testAndSet(3));
    EXPECT_TRUE(visited.testAndSet(3));
    EXPECT_TRUE(visited.test(3));
    visited.set(7);
    EXPECT_TRUE(visited.test(7));
    visited.clear();
    EXPECT_FALSE(visited.test(3));
    EXPECT_FALSE(visited.test(7));
    visited.resize(20);
    EXPECT_EQ(visited.size(), 20u);
    EXPECT_FALSE(visited.test(15));
    visited.resize(5);  // never shrinks
    EXPECT_EQ(visited.size(), 20u);
}

TEST(VisitedSet, SurvivesManyClears) {
    ann::VisitedSet visited(4);
    for (int i = 0; i < 100000; ++i) {
        visited.set(1);
        visited.clear();
        ASSERT_FALSE(visited.test(1));
    }
}

TEST(VisitedPool, ReusesSets) {
    ann::VisitedPool pool;
    ann::VisitedSet* first = nullptr;
    {
        ann::VisitedPool::Lease lease = pool.acquire(8);
        first = &*lease;
        lease->set(2);
        EXPECT_TRUE(lease->test(2));
    }
    {
        ann::VisitedPool::Lease lease = pool.acquire(16);
        EXPECT_EQ(&*lease, first);
        EXPECT_EQ(lease->size(), 16u);
    }
}

TEST(TopK, KeepsSmallestK) {
    ann::TopK top(3);
    EXPECT_TRUE(top.empty());
    EXPECT_EQ(top.worst(), std::numeric_limits<float>::infinity());
    EXPECT_TRUE(top.push(0, 5.0f));
    EXPECT_TRUE(top.push(1, 1.0f));
    EXPECT_TRUE(top.push(2, 3.0f));
    EXPECT_TRUE(top.full());
    EXPECT_FLOAT_EQ(top.worst(), 5.0f);
    EXPECT_FALSE(top.push(3, 6.0f));
    EXPECT_FALSE(top.push(4, 5.0f));  // ties do not displace
    EXPECT_TRUE(top.push(5, 2.0f));
    EXPECT_FLOAT_EQ(top.worst(), 3.0f);
    const std::vector<ann::Neighbor> sorted = top.takeSorted();
    ASSERT_EQ(sorted.size(), 3u);
    EXPECT_EQ(sorted[0], ann::Neighbor(1, 1.0f));
    EXPECT_EQ(sorted[1], ann::Neighbor(5, 2.0f));
    EXPECT_EQ(sorted[2], ann::Neighbor(2, 3.0f));
    EXPECT_TRUE(top.empty());
}

TEST(TopK, ZeroCapacityAcceptsNothing) {
    ann::TopK top(0);
    EXPECT_TRUE(top.full());
    EXPECT_FALSE(top.push(1, 0.0f));
    EXPECT_TRUE(top.takeSorted().empty());
}

TEST(TopK, MatchesSortOnRandomInput) {
    std::mt19937 rng(3);
    std::uniform_real_distribution<float> dist(0.0f, 100.0f);
    for (std::size_t k : {1u, 5u, 50u, 500u}) {
        std::vector<ann::Neighbor> all;
        ann::TopK top(k);
        for (ann::Id i = 0; i < 300; ++i) {
            const float d = dist(rng);
            all.emplace_back(i, d);
            top.push(i, d);
        }
        std::sort(all.begin(), all.end());
        all.resize(std::min(k, all.size()));
        EXPECT_EQ(top.takeSorted(), all) << "k=" << k;
    }
}
