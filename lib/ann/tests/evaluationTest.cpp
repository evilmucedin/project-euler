#include "lib/ann/evaluation.h"
#include "lib/ann/synthetic.h"

#include "gtest/gtest.h"

#include <cmath>
#include <vector>

TEST(Evaluation, Recall) {
    using ann::Neighbor;
    const std::vector<Neighbor> truth = {Neighbor(1, 0.1f), Neighbor(2, 0.2f), Neighbor(3, 0.3f), Neighbor(4, 0.4f)};
    EXPECT_DOUBLE_EQ(ann::recall({Neighbor(4, 0.4f), Neighbor(9, 0.0f), Neighbor(1, 0.1f)}, truth), 0.5);
    EXPECT_DOUBLE_EQ(ann::recall(truth, truth), 1.0);
    EXPECT_DOUBLE_EQ(ann::recall({}, truth), 0.0);
    EXPECT_DOUBLE_EQ(ann::recall({}, {}), 1.0);
    EXPECT_DOUBLE_EQ(ann::meanRecall({truth, {}}, {truth, truth}), 0.5);
    EXPECT_DOUBLE_EQ(ann::meanRecall({truth}, {truth, truth}), 0.5);  // missing query counts as zero
}

TEST(Synthetic, ShapesAndDeterminism) {
    const ann::VectorSet a = ann::randomUniform(10, 3, 1);
    const ann::VectorSet b = ann::randomUniform(10, 3, 1);
    const ann::VectorSet c = ann::randomUniform(10, 3, 2);
    EXPECT_EQ(a.size(), 10u);
    EXPECT_EQ(a.dim(), 3u);
    EXPECT_EQ(a[5][2], b[5][2]);
    EXPECT_NE(a[5][2], c[5][2]);
    for (ann::Id i = 0; i < a.size(); ++i) {
        for (std::size_t d = 0; d < 3; ++d) {
            EXPECT_GE(a[i][d], 0.0f);
            EXPECT_LT(a[i][d], 1.0f);
        }
    }
    const ann::VectorSet g = ann::randomGaussian(5000, 2, 3);
    double mean = 0.0;
    for (ann::Id i = 0; i < g.size(); ++i) {
        mean += g[i][0];
    }
    EXPECT_NEAR(mean / g.size(), 0.0, 0.1);
    const ann::VectorSet clustered = ann::randomClustered(40, 2, 4, 4, 0.01f);
    // Rows i and i + 4 belong to the same blob and sit close together.
    EXPECT_LT(ann::l2Sq(clustered[0], clustered[4], 2), 0.01f);
}
