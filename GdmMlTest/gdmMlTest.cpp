#include <LightGBM/c_api.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
#include <sstream>
#include <string>
#include <vector>

static const int MIN_FEATURES = 100;

static void check(int status, const std::string& msg) {
  if (status != 0) {
    std::cerr << "LightGBM error: " << msg << " (code " << status << ")\n";
    std::exit(status);
  }
}

void loadDataset(const std::string& path,
                 std::vector<float>& data,
                 std::vector<float>& label,
                 int& num_rows,
                 int& num_features) {
  std::ifstream in(path);
  if (!in.is_open()) {
    std::cerr << "Failed to open " << path << " for reading\n";
    std::exit(1);
  }
  data.clear();
  label.clear();
  std::string line;
  num_rows = 0;
  num_features = -1;

  while (std::getline(in, line)) {
    if (line.empty()) continue;
    std::stringstream ss(line);
    std::string tok;
    std::vector<float> row;
    while (std::getline(ss, tok, ',')) {
      if (!tok.empty()) {
        row.push_back(std::stof(tok));
      }
    }
    if (row.empty()) continue;
    if (num_features < 0) {
      num_features = static_cast<int>(row.size()) - 1;
    }
    if (static_cast<int>(row.size()) != num_features + 1) {
      std::cerr << "Invalid row length in data file\n";
      std::exit(1);
    }
    for (int j = 0; j < num_features; ++j) {
      data.push_back(row[j]);
    }
    label.push_back(row[num_features]);
    ++num_rows;
  }

  if (num_features < MIN_FEATURES) {
    std::cerr << "Data must have at least " << MIN_FEATURES << " features, got "
              << num_features << "\n";
    std::exit(1);
  }
}

int main(int argc, char** argv) {
  std::string path = "data.data";
  if (argc >= 2) {
    path = argv[1];
  }

  std::vector<float> loaded_data;
  std::vector<float> loaded_label;
  int nrows = 0;
  int ncols = 0;
  loadDataset(path, loaded_data, loaded_label, nrows, ncols);

  std::vector<int> indices(nrows);
  std::iota(indices.begin(), indices.end(), 0);
  std::mt19937 rng(42);
  std::shuffle(indices.begin(), indices.end(), rng);

  int train_rows = static_cast<int>(nrows * 0.8);
  int test_rows = nrows - train_rows;

  std::vector<float> train_data;
  train_data.reserve(train_rows * ncols);
  std::vector<float> train_label;
  train_label.reserve(train_rows);
  std::vector<float> test_data;
  test_data.reserve(test_rows * ncols);
  std::vector<float> test_label;
  test_label.reserve(test_rows);

  for (int i = 0; i < train_rows; ++i) {
    int idx = indices[i];
    for (int j = 0; j < ncols; ++j) {
      train_data.push_back(loaded_data[idx * ncols + j]);
    }
    train_label.push_back(loaded_label[idx]);
  }
  for (int i = train_rows; i < nrows; ++i) {
    int idx = indices[i];
    for (int j = 0; j < ncols; ++j) {
      test_data.push_back(loaded_data[idx * ncols + j]);
    }
    test_label.push_back(loaded_label[idx]);
  }

  DatasetHandle train_dataset = nullptr;
  check(LGBM_DatasetCreateFromMat(train_data.data(), C_API_DTYPE_FLOAT32,
                                  train_rows, ncols, 1, "", nullptr,
                                  &train_dataset),
        "DatasetCreateFromMat");
  check(LGBM_DatasetSetField(train_dataset, "label", train_label.data(),
                             train_rows, C_API_DTYPE_FLOAT32),
        "DatasetSetField(label)");

  BoosterHandle booster = nullptr;
  const char* params =
      "objective=binary learning_rate=0.1 num_leaves=31 metric=binary_logloss "
      "verbosity=1";
  check(LGBM_BoosterCreate(train_dataset, params, &booster), "BoosterCreate");

  for (int iter = 0; iter < 500; ++iter) {
    int is_finished = 0;
    check(LGBM_BoosterUpdateOneIter(booster, &is_finished),
          "BoosterUpdateOneIter");
    if (is_finished) break;
  }

  auto computeAccuracy = [&](const std::vector<float>& dataset,
                             const std::vector<float>& labels, int rows) {
    std::vector<double> preds(rows);
    int64_t out_len = 0;
    check(LGBM_BoosterPredictForMat(booster, dataset.data(), C_API_DTYPE_FLOAT32,
                                    rows, ncols, 1, C_API_PREDICT_NORMAL, 0, -1,
                                    "", &out_len, preds.data()),
          "BoosterPredictForMat");

    if (out_len != static_cast<int64_t>(rows)) {
      std::cerr << "Unexpected prediction length: " << out_len << " expected "
                << rows << "\n";
      std::exit(1);
    }

    int correct = 0;
    for (int i = 0; i < rows; ++i) {
      int pr = (preds[i] > 0.5) ? 1 : 0;
      if (pr == static_cast<int>(labels[i])) ++correct;
    }
    return static_cast<double>(correct) / rows;
  };

  double train_acc = computeAccuracy(train_data, train_label, train_rows);
  double test_acc = computeAccuracy(test_data, test_label, test_rows);

  std::cout << "=== ML model quality ===\n";
  std::cout << "Train accuracy: " << train_acc << "\n";
  std::cout << "Test accuracy:  " << test_acc << "\n";

  check(LGBM_BoosterFree(booster), "BoosterFree");
  check(LGBM_DatasetFree(train_dataset), "DatasetFree");

  return 0;
}
