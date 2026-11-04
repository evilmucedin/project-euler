#include "lib/ann/bruteForce.h"
#include "lib/ann/evaluation.h"
#include "lib/ann/hnsw.h"
#include "lib/ann/synthetic.h"

#include "gtest/gtest.h"

#include <algorithm>
#include <cmath>
#include <set>
#include <thread>
#include <vector>

namespace {

template <class Metric>
double recallAgainstBruteForce(const ann::HnswIndex<Metric>& index, const ann::VectorSet& set, const ann::VectorSet& queries,
                               std::size_t k, std::size_t ef) {
    ann::BruteForceIndex<Metric> exact(set.dim());
    exact.add(set);
    std::vector<std::vector<ann::Neighbor>> approx;
    std::vector<std::vector<ann::Neighbor>> truth;
    for (ann::Id qi = 0; qi < queries.size(); ++qi) {
        approx.push_back(index.search(queries[qi], k, ef));
        truth.push_back(exact.search(queries[qi], k));
    }
    return ann::meanRecall(approx, truth);
}

}  // namespace

TEST(Hnsw, RejectsBadParams) {
    ann::HnswIndex<ann::L2>::Params params;
    params.M = 1;
    EXPECT_THROW(ann::HnswIndex<ann::L2>(4, params), std::invalid_argument);
    params.M = 8;
    params.efConstruction = 0;
    EXPECT_THROW(ann::HnswIndex<ann::L2>(4, params), std::invalid_argument);
}

TEST(Hnsw, EmptyAndTinyIndexes) {
    ann::HnswIndex<ann::L2> index(3);
    const float q[3] = {0, 0, 0};
    EXPECT_TRUE(index.search(q, 5).empty());
    const float a[3] = {1, 0, 0};
    const float b[3] = {0, 3, 0};
    EXPECT_EQ(index.add(a), 0u);
    EXPECT_EQ(index.size(), 1u);
    EXPECT_EQ(index.entryPoint(), 0u);
    std::vector<ann::Neighbor> got = index.search(q, 5);
    ASSERT_EQ(got.size(), 1u);
    EXPECT_EQ(got[0], ann::Neighbor(0, 1.0f));
    index.add(b);
    got = index.search(q, 5);
    ASSERT_EQ(got.size(), 2u);
    EXPECT_EQ(got[0].id, 0u);
    EXPECT_EQ(got[1], ann::Neighbor(1, 9.0f));
    EXPECT_TRUE(index.search(q, 0).empty());
    got = index.search(q, 1);
    ASSERT_EQ(got.size(), 1u);
}

TEST(Hnsw, GraphInvariants) {
    const std::size_t dim = 8;
    const ann::VectorSet set = ann::randomGaussian(3000, dim, 5);
    ann::HnswIndex<ann::L2>::Params params;
    params.M = 8;
    params.efConstruction = 64;
    ann::HnswIndex<ann::L2> index(dim, params);
    index.add(set);
    ASSERT_EQ(index.size(), set.size());
    EXPECT_GE(index.maxLevel(), 1);
    EXPECT_EQ(index.levelOf(index.entryPoint()), index.maxLevel());
    std::size_t nodesAbove0 = 0;
    for (ann::Id id = 0; id < index.size(); ++id) {
        const int level = index.levelOf(id);
        nodesAbove0 += level > 0;
        for (int l = 0; l <= level; ++l) {
            const std::vector<ann::Id> links = index.neighbors(id, l);
            EXPECT_LE(links.size(), l == 0 ? 2 * params.M : params.M);
            std::set<ann::Id> unique(links.begin(), links.end());
            EXPECT_EQ(unique.size(), links.size()) << "duplicate link at node " << id << " level " << l;
            for (ann::Id nb : links) {
                EXPECT_NE(nb, id) << "self link";
                EXPECT_LT(nb, index.size());
                EXPECT_GE(index.levelOf(nb), l) << "link to a node absent from this level";
            }
        }
        if (level == 0) {
            EXPECT_FALSE(index.neighbors(id, 0).empty()) << "isolated node " << id;
        }
    }
    // With M = 8 roughly 1/8 of nodes get a level above zero.
    EXPECT_GT(nodesAbove0, set.size() / 20);
    EXPECT_LT(nodesAbove0, set.size() / 4);
}

TEST(Hnsw, HighRecallOnGaussianData) {
    const std::size_t dim = 16;
    const ann::VectorSet set = ann::randomGaussian(5000, dim, 21);
    const ann::VectorSet queries = ann::randomGaussian(200, dim, 22);
    ann::HnswIndex<ann::L2>::Params params;
    params.M = 16;
    params.efConstruction = 128;
    ann::HnswIndex<ann::L2> index(dim, params);
    index.reserve(set.size());
    index.add(set);
    EXPECT_GT(recallAgainstBruteForce(index, set, queries, 10, 128), 0.97);
    // A wider beam never hurts and a very narrow one still finds most of them.
    EXPECT_GT(recallAgainstBruteForce(index, set, queries, 10, 16), 0.75);
    EXPECT_GE(recallAgainstBruteForce(index, set, queries, 1, 64), 0.98);
}

TEST(Hnsw, ResultsAreSortedWithCorrectDistances) {
    const std::size_t dim = 12;
    const ann::VectorSet set = ann::randomUniform(2000, dim, 61);
    ann::HnswIndex<ann::L2> index(dim);
    index.add(set);
    const ann::VectorSet queries = ann::randomUniform(20, dim, 62);
    for (ann::Id qi = 0; qi < queries.size(); ++qi) {
        const std::vector<ann::Neighbor> got = index.search(queries[qi], 15);
        ASSERT_EQ(got.size(), 15u);
        for (std::size_t i = 0; i < got.size(); ++i) {
            EXPECT_NEAR(got[i].dist, ann::l2Sq(queries[qi], set[got[i].id], dim), 1e-4f);
            if (i > 0) {
                EXPECT_LE(got[i - 1].dist, got[i].dist);
                EXPECT_NE(got[i - 1].id, got[i].id);
            }
        }
    }
}

TEST(Hnsw, CosineAndInnerProductMetrics) {
    const std::size_t dim = 24;
    ann::VectorSet set = ann::randomGaussian(3000, dim, 71);
    // Scale rows wildly so cosine and L2 disagree.
    for (ann::Id i = 0; i < set.size(); ++i) {
        float* row = set.mutableRow(i);
        const float scale = 0.01f + static_cast<float>(i % 7);
        for (std::size_t d = 0; d < dim; ++d) {
            row[d] *= scale;
        }
    }
    const ann::VectorSet queries = ann::randomGaussian(100, dim, 72);
    {
        ann::HnswIndex<ann::Cosine> index(dim);
        index.add(set);
        EXPECT_GT(recallAgainstBruteForce(index, set, queries, 10, 128), 0.95);
        for (ann::Id i = 0; i < 10; ++i) {
            EXPECT_NEAR(ann::norm(index.vectors()[i], dim), 1.0f, 1e-5f);
        }
    }
    {
        ann::HnswIndex<ann::InnerProduct> index(dim);
        index.add(set);
        // Inner product has no triangle inequality, so the graph is weaker; still useful.
        EXPECT_GT(recallAgainstBruteForce(index, set, queries, 10, 256), 0.8);
    }
}

TEST(Hnsw, ConcurrentSearchesMatchSequential) {
    const std::size_t dim = 16;
    const ann::VectorSet set = ann::randomGaussian(4000, dim, 81);
    const ann::VectorSet queries = ann::randomGaussian(400, dim, 82);
    ann::HnswIndex<ann::L2> index(dim);
    index.add(set);
    std::vector<std::vector<ann::Neighbor>> sequential(queries.size());
    for (ann::Id qi = 0; qi < queries.size(); ++qi) {
        sequential[qi] = index.search(queries[qi], 10);
    }
    std::vector<std::vector<ann::Neighbor>> parallel(queries.size());
    const unsigned threads = 8;
    std::vector<std::thread> pool;
    for (unsigned t = 0; t < threads; ++t) {
        pool.emplace_back([&, t] {
            for (ann::Id qi = t; qi < queries.size(); qi += threads) {
                parallel[qi] = index.search(queries[qi], 10);
            }
        });
    }
    for (std::thread& th : pool) {
        th.join();
    }
    EXPECT_EQ(parallel, sequential);
}

TEST(Hnsw, DeterministicForSameSeed) {
    const std::size_t dim = 8;
    const ann::VectorSet set = ann::randomGaussian(1000, dim, 91);
    ann::HnswIndex<ann::L2> a(dim);
    ann::HnswIndex<ann::L2> b(dim);
    a.add(set);
    b.add(set);
    for (ann::Id id = 0; id < set.size(); id += 37) {
        EXPECT_EQ(a.levelOf(id), b.levelOf(id));
        EXPECT_EQ(a.neighbors(id, 0), b.neighbors(id, 0));
    }
}
