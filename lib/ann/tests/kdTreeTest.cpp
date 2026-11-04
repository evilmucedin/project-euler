#include "lib/ann/bruteForce.h"
#include "lib/ann/evaluation.h"
#include "lib/ann/kdTree.h"
#include "lib/ann/synthetic.h"

#include "gtest/gtest.h"

#include <vector>

namespace {

void expectSameNeighbors(const std::vector<ann::Neighbor>& got, const std::vector<ann::Neighbor>& expected) {
    ASSERT_EQ(got.size(), expected.size());
    for (std::size_t i = 0; i < got.size(); ++i) {
        // Ties at equal distance may legitimately come back in either order.
        EXPECT_NEAR(got[i].dist, expected[i].dist, 1e-5f * (1.0f + expected[i].dist)) << "i=" << i;
    }
    EXPECT_DOUBLE_EQ(ann::recall(got, expected), 1.0);
}

}  // namespace

TEST(KdTree, ExactSearchMatchesBruteForce) {
    for (std::size_t dim : {1u, 2u, 3u, 8u, 16u}) {
        for (std::size_t leafSize : {1u, 8u, 32u}) {
            const ann::VectorSet set = ann::randomUniform(2000, dim, 10 + dim);
            const ann::VectorSet queries = ann::randomUniform(50, dim, 20 + dim, -0.2f, 1.2f);
            ann::BruteForceIndex<ann::L2> exact(dim);
            exact.add(set);
            ann::KdTree tree(set, ann::KdTree::Params{leafSize});
            EXPECT_EQ(tree.size(), set.size());
            EXPECT_EQ(tree.dim(), dim);
            for (ann::Id qi = 0; qi < queries.size(); ++qi) {
                for (std::size_t k : {1u, 7u, 25u}) {
                    SCOPED_TRACE(::testing::Message() << "dim=" << dim << " leafSize=" << leafSize << " q=" << qi << " k=" << k);
                    expectSameNeighbors(tree.search(queries[qi], k), exact.search(queries[qi], k));
                }
            }
        }
    }
}

TEST(KdTree, HandlesDuplicatesAndTinySets) {
    ann::VectorSet set(2);
    const float p[2] = {1.0f, 1.0f};
    for (int i = 0; i < 100; ++i) {
        set.add(p);  // 100 identical points: spread is zero on every axis
    }
    const float other[2] = {2.0f, 2.0f};
    set.add(other);
    ann::KdTree tree(set, ann::KdTree::Params{4});
    const float q[2] = {1.9f, 1.9f};
    std::vector<ann::Neighbor> got = tree.search(q, 3);
    ASSERT_EQ(got.size(), 3u);
    EXPECT_EQ(got[0].id, 100u);
    EXPECT_NEAR(got[1].dist, 2 * 0.9f * 0.9f, 1e-5f);

    got = tree.search(q, 500);
    EXPECT_EQ(got.size(), 101u);

    ann::KdTree empty;
    EXPECT_TRUE(empty.search(q, 3).empty());
    ann::VectorSet one(2);
    one.add(p);
    ann::KdTree single(one);
    got = single.search(q, 3);
    ASSERT_EQ(got.size(), 1u);
    EXPECT_EQ(got[0].id, 0u);
}

TEST(KdTree, EpsilonSearchIsWithinBound) {
    const std::size_t dim = 8;
    const ann::VectorSet set = ann::randomUniform(5000, dim, 31);
    const ann::VectorSet queries = ann::randomUniform(100, dim, 32);
    ann::BruteForceIndex<ann::L2> exact(dim);
    exact.add(set);
    ann::KdTree tree(set);
    const float eps = 0.5f;
    double totalRecall = 0.0;
    for (ann::Id qi = 0; qi < queries.size(); ++qi) {
        ann::KdTree::SearchParams sp;
        sp.eps = eps;
        const std::vector<ann::Neighbor> got = tree.search(queries[qi], 10, sp);
        const std::vector<ann::Neighbor> truth = exact.search(queries[qi], 10);
        ASSERT_EQ(got.size(), 10u);
        // Squared-distance guarantee: got[i] <= (1 + eps)^2 * truth[i].
        for (std::size_t i = 0; i < 10; ++i) {
            EXPECT_LE(got[i].dist, truth[i].dist * (1.0f + eps) * (1.0f + eps) + 1e-6f);
        }
        totalRecall += ann::recall(got, truth);
    }
    EXPECT_GT(totalRecall / queries.size(), 0.7);
}

TEST(KdTree, LeafBudgetBoundsWorkAndKeepsResultsValid) {
    const std::size_t dim = 3;
    const ann::VectorSet set = ann::randomUniform(20000, dim, 41);
    const ann::VectorSet queries = ann::randomUniform(50, dim, 42);
    ann::BruteForceIndex<ann::L2> exact(dim);
    exact.add(set);
    ann::KdTree tree(set, ann::KdTree::Params{8});
    EXPECT_GT(tree.leafCount(), 1000u);
    double totalRecall = 0.0;
    for (ann::Id qi = 0; qi < queries.size(); ++qi) {
        ann::KdTree::SearchParams sp;
        sp.maxLeaves = 8;
        const std::vector<ann::Neighbor> got = tree.search(queries[qi], 5, sp);
        EXPECT_FALSE(got.empty());
        EXPECT_LE(got.size(), 5u);
        for (const ann::Neighbor& n : got) {
            EXPECT_LT(n.id, set.size());
            EXPECT_NEAR(n.dist, ann::l2Sq(queries[qi], set[n.id], dim), 1e-5f);
        }
        totalRecall += ann::recall(got, exact.search(queries[qi], 5));
    }
    // Eight leaves of eight points in 3-D still find most true neighbours.
    EXPECT_GT(totalRecall / queries.size(), 0.5);
}
