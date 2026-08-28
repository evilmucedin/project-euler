#include "lib/boosting/trainer.h"

#include <cmath>
#include <cstdio>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include "gtest/gtest.h"

using namespace boosting;

namespace {

double meanSquaredError(const TModel& model, const TDataset& dataset) {
    double sum = 0.0;
    for (size_t i = 0; i < dataset.rowCount(); ++i) {
        const double diff = model.apply(dataset.row(i)) - dataset.target(i);
        sum += diff * diff;
    }
    return sum / dataset.rowCount();
}

// y = x0 * x1 + x2 with one irrelevant feature and a bit of noise.
void makeRegressionData(TDataset& train, TDataset& test, uint32_t seed) {
    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> uniform(-1.0f, 1.0f);
    std::normal_distribution<float> noise(0.0f, 0.05f);
    for (int i = 0; i < 3000; ++i) {
        std::vector<float> row = {uniform(rng), uniform(rng), uniform(rng), uniform(rng)};
        const float target = row[0] * row[1] + row[2] + noise(rng);
        (i % 4 == 0 ? test : train).addRow(row, target);
    }
}

}  // namespace

TEST(ObliviousTree, LeafIndexIsBitmaskOfSplits) {
    TObliviousTree tree;
    tree.splits = {{0, 0.5f}, {1, -1.0f}};
    tree.leafValues = {10.0, 11.0, 12.0, 13.0};

    const float bothBelow[] = {0.0f, -2.0f};
    const float firstAbove[] = {1.0f, -2.0f};
    const float secondAbove[] = {0.0f, 0.0f};
    const float bothAbove[] = {1.0f, 0.0f};
    EXPECT_EQ(tree.leafIndex(bothBelow), 0u);
    EXPECT_EQ(tree.leafIndex(firstAbove), 1u);
    EXPECT_EQ(tree.leafIndex(secondAbove), 2u);
    EXPECT_EQ(tree.leafIndex(bothAbove), 3u);
    EXPECT_DOUBLE_EQ(tree.apply(bothAbove), 13.0);
}

TEST(Trainer, LearnsNonlinearRegression) {
    TDataset train(4);
    TDataset test(4);
    makeRegressionData(train, test, 1);

    TTrainParams params;
    params.iterations = 200;
    params.depth = 4;
    params.learningRate = 0.3;
    params.loss = ELoss::RMSE;
    const TModel model = TTrainer(params).train(train);

    // Baseline: predicting the mean scores the target variance (~0.45 here).
    double mean = 0.0;
    for (size_t i = 0; i < test.rowCount(); ++i) {
        mean += test.target(i);
    }
    mean /= test.rowCount();
    double variance = 0.0;
    for (size_t i = 0; i < test.rowCount(); ++i) {
        variance += (test.target(i) - mean) * (test.target(i) - mean);
    }
    variance /= test.rowCount();

    const double testMse = meanSquaredError(model, test);
    EXPECT_LT(testMse, 0.1 * variance);
    EXPECT_LT(meanSquaredError(model, train), testMse * 1.5);
}

TEST(Trainer, LearnsBinaryClassification) {
    std::mt19937 rng(2);
    std::uniform_real_distribution<float> uniform(-1.0f, 1.0f);
    TDataset train(2);
    TDataset test(2);
    for (int i = 0; i < 2000; ++i) {
        std::vector<float> row = {uniform(rng), uniform(rng)};
        const float label = row[0] + row[1] > 0.0f ? 1.0f : 0.0f;
        (i % 4 == 0 ? test : train).addRow(row, label);
    }

    TTrainParams params;
    params.iterations = 100;
    params.depth = 4;
    params.learningRate = 0.2;
    params.loss = ELoss::LogLoss;
    const TModel model = TTrainer(params).train(train);

    size_t correct = 0;
    for (size_t i = 0; i < test.rowCount(); ++i) {
        const double p = model.applyProbability(test.row(i));
        ASSERT_GE(p, 0.0);
        ASSERT_LE(p, 1.0);
        correct += (p > 0.5) == (test.target(i) > 0.5f);
    }
    EXPECT_GT(static_cast<double>(correct) / test.rowCount(), 0.93);
}

TEST(Trainer, SubsampleAndDeterminism) {
    TDataset train(4);
    TDataset test(4);
    makeRegressionData(train, test, 3);

    TTrainParams params;
    params.iterations = 100;
    params.depth = 3;
    params.learningRate = 0.3;
    params.subsample = 0.7;
    const TModel a = TTrainer(params).train(train);
    const TModel b = TTrainer(params).train(train);

    for (size_t i = 0; i < test.rowCount(); ++i) {
        EXPECT_DOUBLE_EQ(a.apply(test.row(i)), b.apply(test.row(i)));
    }
    EXPECT_LT(meanSquaredError(a, test), 0.1);
}

TEST(Model, SaveLoadStreamRoundtrip) {
    TDataset train(4);
    TDataset test(4);
    makeRegressionData(train, test, 4);

    TTrainParams params;
    params.iterations = 50;
    params.depth = 4;
    const TModel model = TTrainer(params).train(train);

    std::stringstream buffer;
    model.save(buffer);
    const TModel loaded = TModel::load(buffer);

    EXPECT_EQ(loaded.featureCount(), model.featureCount());
    EXPECT_EQ(loaded.loss(), model.loss());
    EXPECT_EQ(loaded.trees().size(), model.trees().size());
    for (size_t i = 0; i < test.rowCount(); ++i) {
        EXPECT_DOUBLE_EQ(loaded.apply(test.row(i)), model.apply(test.row(i)));
    }
}

TEST(Model, SaveLoadFileRoundtrip) {
    TDataset train(4);
    TDataset test(4);
    makeRegressionData(train, test, 5);

    TTrainParams params;
    params.iterations = 30;
    params.depth = 3;
    params.loss = ELoss::RMSE;
    const TModel model = TTrainer(params).train(train);

    const std::string path = ::testing::TempDir() + "boostingModelTest.cbm";
    model.save(path);
    const TModel loaded = TModel::load(path);
    std::remove(path.c_str());

    for (size_t i = 0; i < test.rowCount(); ++i) {
        EXPECT_DOUBLE_EQ(loaded.apply(test.row(i)), model.apply(test.row(i)));
    }
}

TEST(Model, LoadRejectsGarbage) {
    std::stringstream buffer;
    buffer << "definitely not a model";
    EXPECT_THROW(TModel::load(buffer), std::runtime_error);
}
