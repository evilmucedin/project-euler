#include "lib/ann/bruteForce.h"
#include "lib/ann/synthetic.h"

#include "gtest/gtest.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace {

// Independent double-precision ground truth.
std::vector<ann::Neighbor> naiveL2(const ann::VectorSet& set, const float* q, std::size_t k) {
    std::vector<ann::Neighbor> all;
    for (ann::Id i = 0; i < set.size(); ++i) {
        double s = 0.0;
        for (std::size_t d = 0; d < set.dim(); ++d) {
            const double diff = static_cast<double>(set[i][d]) - static_cast<double>(q[d]);
            s += diff * diff;
        }
        all.emplace_back(i, static_cast<float>(s));
    }
    std::sort(all.begin(), all.end());
    all.resize(std::min(k, all.size()));
    return all;
}

}  // namespace

TEST(BruteForce, MatchesNaiveScan) {
    for (std::size_t dim : {1u, 3u, 16u, 37u, 128u}) {
        const ann::VectorSet set = ann::randomUniform(500, dim, 100 + dim);
        const ann::VectorSet queries = ann::randomUniform(20, dim, 200 + dim);
        ann::BruteForceIndex<ann::L2> index(dim);
        index.add(set);
        EXPECT_EQ(index.size(), 500u);
        for (ann::Id qi = 0; qi < queries.size(); ++qi) {
            const std::vector<ann::Neighbor> expected = naiveL2(set, queries[qi], 10);
            const std::vector<ann::Neighbor> got = index.search(queries[qi], 10);
            ASSERT_EQ(got.size(), 10u);
            for (std::size_t i = 0; i < 10; ++i) {
                EXPECT_EQ(got[i].id, expected[i].id) << "dim=" << dim << " q=" << qi << " i=" << i;
                EXPECT_NEAR(got[i].dist, expected[i].dist, 1e-3f * (1.0f + expected[i].dist));
            }
        }
    }
}

TEST(BruteForce, EdgeCases) {
    ann::BruteForceIndex<ann::L2> index(4);
    const float q[4] = {0, 0, 0, 0};
    EXPECT_TRUE(index.search(q, 5).empty());
    const float a[4] = {1, 0, 0, 0};
    const float b[4] = {0, 2, 0, 0};
    EXPECT_EQ(index.add(a), 0u);
    EXPECT_EQ(index.add(b), 1u);
    EXPECT_TRUE(index.search(q, 0).empty());
    const std::vector<ann::Neighbor> got = index.search(q, 5);  // k larger than size
    ASSERT_EQ(got.size(), 2u);
    EXPECT_EQ(got[0], ann::Neighbor(0, 1.0f));
    EXPECT_EQ(got[1], ann::Neighbor(1, 4.0f));
}

TEST(BruteForce, InnerProductPrefersLargeDot) {
    ann::BruteForceIndex<ann::InnerProduct> index(2);
    const float a[2] = {1, 0};
    const float b[2] = {10, 0};
    const float c[2] = {-1, 0};
    index.add(a);
    index.add(b);
    index.add(c);
    const float q[2] = {1, 0};
    const std::vector<ann::Neighbor> got = index.search(q, 3);
    ASSERT_EQ(got.size(), 3u);
    EXPECT_EQ(got[0].id, 1u);
    EXPECT_FLOAT_EQ(got[0].dist, -10.0f);
    EXPECT_EQ(got[1].id, 0u);
    EXPECT_EQ(got[2].id, 2u);
}

TEST(BruteForce, CosineIgnoresMagnitude) {
    ann::BruteForceIndex<ann::Cosine> index(2);
    const float a[2] = {100, 1};   // almost parallel to q, huge norm
    const float b[2] = {0.1f, 0};  // exactly parallel to q, tiny norm
    const float c[2] = {0, 1};     // orthogonal
    index.add(a);
    index.add(b);
    index.add(c);
    const float q[2] = {5, 0};
    const std::vector<ann::Neighbor> got = index.search(q, 3);
    ASSERT_EQ(got.size(), 3u);
    EXPECT_EQ(got[0].id, 1u);
    EXPECT_NEAR(got[0].dist, 0.0f, 1e-6f);
    EXPECT_EQ(got[1].id, 0u);
    EXPECT_EQ(got[2].id, 2u);
    EXPECT_NEAR(got[2].dist, 1.0f, 1e-6f);
    // Stored rows were normalised in place.
    EXPECT_NEAR(ann::norm(index.vectors()[0], 2), 1.0f, 1e-6f);
}
