// Compares lib/boosting against the official CatBoost library on identical
// data and matched hyperparameters: test-set quality and train/apply speed.
// CatBoost is driven through tests/catboost_reference.py; when python3 with
// the catboost package is not available the tests skip instead of failing.

#include "lib/boosting/trainer.h"

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include "gtest/gtest.h"

using namespace boosting;

namespace {

struct TCatBoostEnv {
    std::string python;
    std::string script;
};

std::string quoted(const std::string& s) {
    return "'" + s + "'";
}

std::optional<TCatBoostEnv> detectCatBoost() {
    std::vector<std::string> candidates;
    if (const char* env = std::getenv("CATBOOST_PYTHON")) {
        candidates.push_back(env);
    }
    candidates.push_back("python3");
    candidates.push_back("/usr/bin/python3");

    std::string python;
    for (const std::string& candidate : candidates) {
        const std::string check = candidate + " -c 'import catboost' >/dev/null 2>&1";
        if (std::system(check.c_str()) == 0) {
            python = candidate;
            break;
        }
    }
    if (python.empty()) {
        return std::nullopt;
    }

    std::string script;
    if (const char* env = std::getenv("CATBOOST_REFERENCE_SCRIPT")) {
        script = env;
    } else {
        // __FILE__ is relative to the repo root under both the ninja and the
        // buck2 build, so this works when tests run from the repo root.
        script = __FILE__;
        const size_t slash = script.find_last_of('/');
        script = script.substr(0, slash + 1) + "catboost_reference.py";
    }
    if (!std::ifstream(script).good()) {
        return std::nullopt;
    }
    return TCatBoostEnv{python, script};
}

void writeCsv(const std::string& path, const TDataset& dataset) {
    std::ofstream out(path, std::ios::trunc);
    ASSERT_TRUE(out.good()) << "cannot write " << path;
    for (size_t i = 0; i < dataset.rowCount(); ++i) {
        const float* row = dataset.row(i);
        for (size_t feature = 0; feature < dataset.featureCount(); ++feature) {
            out << row[feature] << ",";
        }
        out << dataset.target(i) << "\n";
    }
}

// Runs the reference script and parses its `key=value` stdout lines.
std::map<std::string, double> runCatBoost(const TCatBoostEnv& env, const std::string& trainCsv,
                                          const std::string& testCsv, const std::string& loss,
                                          const TTrainParams& params) {
    std::ostringstream command;
    command << quoted(env.python) << " " << quoted(env.script) << " --train-csv " << quoted(trainCsv)
            << " --test-csv " << quoted(testCsv) << " --loss " << loss << " --iterations " << params.iterations
            << " --depth " << params.depth << " --learning-rate " << params.learningRate << " --l2-leaf-reg "
            << params.l2LeafReg << " --border-count " << params.borderCount << " --seed " << params.seed
            << " 2>&1";

    FILE* pipe = popen(command.str().c_str(), "r");
    EXPECT_NE(pipe, nullptr) << "popen failed for: " << command.str();
    std::string output;
    if (pipe != nullptr) {
        char buffer[4096];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            output += buffer;
        }
        const int status = pclose(pipe);
        EXPECT_EQ(status, 0) << "catboost_reference.py failed:\n" << output;
    }

    std::map<std::string, double> metrics;
    std::istringstream lines(output);
    std::string line;
    while (std::getline(lines, line)) {
        const size_t eq = line.find('=');
        if (eq == std::string::npos) {
            continue;  // catboost warnings etc.
        }
        try {
            metrics[line.substr(0, eq)] = std::stod(line.substr(eq + 1));
        } catch (const std::exception&) {
        }
    }
    return metrics;
}

double millisecondsSince(std::chrono::steady_clock::time_point start) {
    return std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();
}

// Time of one pass over the test set with the realtime row-by-row apply,
// averaged over several repetitions.
double measureApplyMs(const TModel& model, const TDataset& test) {
    constexpr int kRepeats = 20;
    double sink = 0.0;
    const auto start = std::chrono::steady_clock::now();
    for (int repeat = 0; repeat < kRepeats; ++repeat) {
        for (size_t i = 0; i < test.rowCount(); ++i) {
            sink += model.apply(test.row(i));
        }
    }
    const double totalMs = millisecondsSince(start);
    EXPECT_TRUE(std::isfinite(sink));
    return totalMs / kRepeats;
}

TTrainParams comparisonParams(ELoss loss) {
    TTrainParams params;
    params.iterations = 200;
    params.depth = 6;
    params.learningRate = 0.1;
    params.l2LeafReg = 3.0;
    params.borderCount = 128;
    params.seed = 42;
    params.loss = loss;
    return params;
}

}  // namespace

TEST(CatBoostCompare, RegressionQualityAndSpeed) {
    const std::optional<TCatBoostEnv> env = detectCatBoost();
    if (!env) {
        GTEST_SKIP() << "python3 with the catboost package is not available";
    }

    // y = x0 * x1 + x2 with an irrelevant feature and mild noise.
    std::mt19937 rng(1);
    std::uniform_real_distribution<float> uniform(-1.0f, 1.0f);
    std::normal_distribution<float> noise(0.0f, 0.05f);
    TDataset train(4);
    TDataset test(4);
    for (int i = 0; i < 8000; ++i) {
        std::vector<float> row = {uniform(rng), uniform(rng), uniform(rng), uniform(rng)};
        const float target = row[0] * row[1] + row[2] + noise(rng);
        (i % 4 == 0 ? test : train).addRow(row, target);
    }

    const TTrainParams params = comparisonParams(ELoss::RMSE);
    const auto trainStart = std::chrono::steady_clock::now();
    const TModel model = TTrainer(params).train(train);
    const double ourTrainMs = millisecondsSince(trainStart);

    double ourMse = 0.0;
    for (size_t i = 0; i < test.rowCount(); ++i) {
        const double diff = model.apply(test.row(i)) - test.target(i);
        ourMse += diff * diff;
    }
    ourMse /= test.rowCount();
    const double ourApplyMs = measureApplyMs(model, test);

    const std::string trainCsv = ::testing::TempDir() + "boostingCompareRegressionTrain.csv";
    const std::string testCsv = ::testing::TempDir() + "boostingCompareRegressionTest.csv";
    writeCsv(trainCsv, train);
    writeCsv(testCsv, test);
    const auto metrics = runCatBoost(*env, trainCsv, testCsv, "RMSE", params);
    std::remove(trainCsv.c_str());
    std::remove(testCsv.c_str());

    ASSERT_TRUE(metrics.count("test_mse")) << "reference script produced no test_mse";
    const double catboostMse = metrics.at("test_mse");

    std::cout << "[ compare ] regression, " << train.rowCount() << " train rows, " << test.rowCount()
              << " test rows, " << params.iterations << " trees of depth " << params.depth << "\n"
              << "[ compare ]   test MSE:        ours=" << ourMse << " catboost=" << catboostMse << "\n"
              << "[ compare ]   train time, ms:  ours=" << ourTrainMs << " catboost=" << metrics.at("train_time_ms")
              << "\n"
              << "[ compare ]   apply test set:  ours=" << ourApplyMs << " ms ("
              << ourApplyMs * 1000.0 / test.rowCount() << " us/row) catboost batch="
              << metrics.at("predict_time_ms") << " ms\n";

    // Quality must stay in the same league as CatBoost on this smooth task.
    EXPECT_LT(ourMse, 3.0 * catboostMse);
    // And be far better than predicting the mean (variance is ~0.45 here).
    EXPECT_LT(ourMse, 0.05);
    // Realtime apply budget: 200 depth-6 trees per row.
    EXPECT_LT(ourApplyMs * 1000.0 / test.rowCount(), 20.0);
}

TEST(CatBoostCompare, ClassificationQualityAndSpeed) {
    const std::optional<TCatBoostEnv> env = detectCatBoost();
    if (!env) {
        GTEST_SKIP() << "python3 with the catboost package is not available";
    }

    // Noisy circle: label = 1 inside radius sqrt(0.5), 3% labels flipped.
    std::mt19937 rng(2);
    std::uniform_real_distribution<float> uniform(-1.0f, 1.0f);
    std::uniform_real_distribution<float> flip(0.0f, 1.0f);
    TDataset train(3);
    TDataset test(3);
    for (int i = 0; i < 6000; ++i) {
        std::vector<float> row = {uniform(rng), uniform(rng), uniform(rng)};
        bool inside = row[0] * row[0] + row[1] * row[1] < 0.5f;
        if (flip(rng) < 0.03f) {
            inside = !inside;
        }
        (i % 4 == 0 ? test : train).addRow(row, inside ? 1.0f : 0.0f);
    }

    const TTrainParams params = comparisonParams(ELoss::LogLoss);
    const auto trainStart = std::chrono::steady_clock::now();
    const TModel model = TTrainer(params).train(train);
    const double ourTrainMs = millisecondsSince(trainStart);

    size_t correct = 0;
    double logloss = 0.0;
    for (size_t i = 0; i < test.rowCount(); ++i) {
        const double p = std::min(std::max(model.applyProbability(test.row(i)), 1e-15), 1.0 - 1e-15);
        const bool label = test.target(i) > 0.5f;
        correct += (p > 0.5) == label;
        logloss -= label ? std::log(p) : std::log(1.0 - p);
    }
    const double ourAccuracy = static_cast<double>(correct) / test.rowCount();
    logloss /= test.rowCount();
    const double ourApplyMs = measureApplyMs(model, test);

    const std::string trainCsv = ::testing::TempDir() + "boostingCompareClassificationTrain.csv";
    const std::string testCsv = ::testing::TempDir() + "boostingCompareClassificationTest.csv";
    writeCsv(trainCsv, train);
    writeCsv(testCsv, test);
    const auto metrics = runCatBoost(*env, trainCsv, testCsv, "Logloss", params);
    std::remove(trainCsv.c_str());
    std::remove(testCsv.c_str());

    ASSERT_TRUE(metrics.count("test_accuracy")) << "reference script produced no test_accuracy";
    const double catboostAccuracy = metrics.at("test_accuracy");
    const double catboostLogloss = metrics.at("test_logloss");

    std::cout << "[ compare ] classification, " << train.rowCount() << " train rows, " << test.rowCount()
              << " test rows, " << params.iterations << " trees of depth " << params.depth << "\n"
              << "[ compare ]   test accuracy:   ours=" << ourAccuracy << " catboost=" << catboostAccuracy << "\n"
              << "[ compare ]   test logloss:    ours=" << logloss << " catboost=" << catboostLogloss << "\n"
              << "[ compare ]   train time, ms:  ours=" << ourTrainMs << " catboost=" << metrics.at("train_time_ms")
              << "\n"
              << "[ compare ]   apply test set:  ours=" << ourApplyMs << " ms ("
              << ourApplyMs * 1000.0 / test.rowCount() << " us/row) catboost batch="
              << metrics.at("predict_time_ms") << " ms\n";

    EXPECT_GE(ourAccuracy, catboostAccuracy - 0.05);
    EXPECT_GT(ourAccuracy, 0.9);
    EXPECT_LT(logloss, 2.0 * catboostLogloss);
    EXPECT_LT(ourApplyMs * 1000.0 / test.rowCount(), 20.0);
}
