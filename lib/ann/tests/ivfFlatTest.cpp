#include "lib/ann/bruteForce.h"
#include "lib/ann/evaluation.h"
#include "lib/ann/ivfFlat.h"
#include "lib/ann/kmeans.h"
#include "lib/ann/synthetic.h"

#include "gtest/gtest.h"

#include <map>
#include <set>
#include <stdexcept>
#include <vector>

TEST(KMeans, RecoversWellSeparatedClusters) {
    const std::size_t clusters = 6;
    const ann::VectorSet set = ann::randomClustered(1200, 4, clusters, 3, 0.2f);
    const ann::KMeansResult result = ann::kmeans(set, clusters);
    ASSERT_EQ(result.centroids.size(), clusters);
    ASSERT_EQ(result.assignment.size(), set.size());
    EXPECT_GE(result.iterationsRun, 1u);
    // Every true cluster (row i % clusters) must map to a single k-means cell.
    std::map<std::size_t, std::set<std::uint32_t>> cellsPerTruth;
    for (std::size_t i = 0; i < set.size(); ++i) {
        cellsPerTruth[i % clusters].insert(result.assignment[i]);
    }
    std::set<std::uint32_t> usedCells;
    for (const auto& entry : cellsPerTruth) {
        EXPECT_EQ(entry.second.size(), 1u) << "true cluster " << entry.first << " split across cells";
        usedCells.insert(entry.second.begin(), entry.second.end());
    }
    EXPECT_EQ(usedCells.size(), clusters);
    // Inertia is small compared with the spread of the centres.
    EXPECT_LT(result.inertia / set.size(), 4 * 0.2 * 0.2 * 2);
}

TEST(KMeans, RejectsBadInput) {
    const ann::VectorSet set = ann::randomUniform(5, 2, 1);
    EXPECT_THROW(ann::kmeans(set, 0), std::invalid_argument);
    EXPECT_THROW(ann::kmeans(set, 6), std::invalid_argument);
    const ann::KMeansResult result = ann::kmeans(set, 5);
    EXPECT_EQ(result.centroids.size(), 5u);
}

TEST(IvfFlat, RequiresTraining) {
    ann::IvfFlatIndex<ann::L2> index(4);
    EXPECT_FALSE(index.trained());
    const float v[4] = {1, 2, 3, 4};
    EXPECT_THROW(index.add(v), std::logic_error);
    EXPECT_TRUE(index.search(v, 3).empty());
    ann::IvfFlatIndex<ann::L2>::Params params;
    params.nlist = 0;
    EXPECT_THROW(ann::IvfFlatIndex<ann::L2>(4, params), std::invalid_argument);
}

TEST(IvfFlat, ProbingEveryCellIsExact) {
    const std::size_t dim = 10;
    const ann::VectorSet set = ann::randomUniform(3000, dim, 7);
    const ann::VectorSet queries = ann::randomUniform(50, dim, 8);
    ann::IvfFlatIndex<ann::L2>::Params params;
    params.nlist = 32;
    ann::IvfFlatIndex<ann::L2> index(dim, params);
    index.build(set);
    EXPECT_TRUE(index.trained());
    EXPECT_EQ(index.size(), set.size());
    std::size_t stored = 0;
    for (std::size_t c = 0; c < index.centroids().size(); ++c) {
        stored += index.listSize(c);
    }
    EXPECT_EQ(stored, set.size());
    ann::BruteForceIndex<ann::L2> exact(dim);
    exact.add(set);
    for (ann::Id qi = 0; qi < queries.size(); ++qi) {
        EXPECT_EQ(index.search(queries[qi], 10, params.nlist), exact.search(queries[qi], 10));
    }
}

TEST(IvfFlat, RecallGrowsWithProbes) {
    const std::size_t dim = 32;
    const ann::VectorSet set = ann::randomClustered(20000, dim, 100, 17, 1.0f);
    const ann::VectorSet queries = ann::randomClustered(200, dim, 100, 17, 1.0f);
    ann::IvfFlatIndex<ann::L2>::Params params;
    params.nlist = 100;
    params.nprobe = 4;
    ann::IvfFlatIndex<ann::L2> index(dim, params);
    index.build(set);
    ann::BruteForceIndex<ann::L2> exact(dim);
    exact.add(set);
    std::vector<std::vector<ann::Neighbor>> truth;
    for (ann::Id qi = 0; qi < queries.size(); ++qi) {
        truth.push_back(exact.search(queries[qi], 10));
    }
    double previous = -1.0;
    for (std::size_t nprobe : {1u, 4u, 16u}) {
        std::vector<std::vector<ann::Neighbor>> approx;
        for (ann::Id qi = 0; qi < queries.size(); ++qi) {
            approx.push_back(index.search(queries[qi], 10, nprobe));
        }
        const double r = ann::meanRecall(approx, truth);
        EXPECT_GE(r, previous) << "nprobe=" << nprobe;
        previous = r;
    }
    EXPECT_GT(previous, 0.9);
    // Default nprobe from params is used when 0 is passed.
    EXPECT_EQ(index.search(queries[0], 10), index.search(queries[0], 10, 4));
}

TEST(IvfFlat, CosineMetricNormalises) {
    const std::size_t dim = 8;
    ann::VectorSet set = ann::randomGaussian(2000, dim, 27);
    for (ann::Id i = 0; i < set.size(); ++i) {
        float* row = set.mutableRow(i);
        for (std::size_t d = 0; d < dim; ++d) {
            row[d] *= 1.0f + static_cast<float>(i % 5);
        }
    }
    const ann::VectorSet queries = ann::randomGaussian(30, dim, 28);
    ann::IvfFlatIndex<ann::Cosine>::Params params;
    params.nlist = 16;
    ann::IvfFlatIndex<ann::Cosine> index(dim, params);
    index.build(set);
    ann::BruteForceIndex<ann::Cosine> exact(dim);
    exact.add(set);
    for (ann::Id qi = 0; qi < queries.size(); ++qi) {
        const std::vector<ann::Neighbor> got = index.search(queries[qi], 5, 16);
        const std::vector<ann::Neighbor> truth = exact.search(queries[qi], 5);
        ASSERT_EQ(got.size(), 5u);
        for (std::size_t i = 0; i < 5; ++i) {
            EXPECT_EQ(got[i].id, truth[i].id);
            EXPECT_NEAR(got[i].dist, truth[i].dist, 1e-5f);
        }
    }
}
