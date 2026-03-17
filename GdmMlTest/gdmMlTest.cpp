#include <LightGBM/c_api.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

static void check(int status, const std::string &msg) {
    if (status != 0) {
        std::cerr << "LightGBM error: " << msg << " (code " << status << ")\n";
        std::exit(status);
    }
}

void saveDataset(const std::string &path,
                 const std::vector<float> &data,
                 const std::vector<float> &label,
                 int num_rows,
                 int num_features) {
    std::ofstream out(path);
    if (!out.is_open()) {
        std::cerr << "Failed to open " << path << " for writing\n";
        std::exit(1);
    }
    for (int i = 0; i < num_rows; ++i) {
        for (int j = 0; j < num_features; ++j) {
            out << data[i * num_features + j] << ",";
        }
        out << label[i] << "\n";
    }
}

void loadDataset(const std::string &path,
                 std::vector<float> &data,
                 std::vector<float> &label,
                 int &num_rows,
                 int &num_features) {
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
}

int main() {
    const std::string path = "data.data";
    const int num_data = 200;
    const int num_feature = 2;
    std::vector<float> data(num_data * num_feature);
    std::vector<float> label(num_data);
    for (int i = 0; i < num_data; ++i) {
        float x = static_cast<float>(i) / num_data;
        float y = static_cast<float>(i % 30) / 30.0f;
        data[i * num_feature + 0] = x;
        data[i * num_feature + 1] = y;
        label[i] = ((x + y) > 0.8f) ? 1.0f : 0.0f;
    }

    saveDataset(path, data, label, num_data, num_feature);

    std::vector<float> loaded_data;
    std::vector<float> loaded_label;
    int nrows;
    int ncols;
    loadDataset(path, loaded_data, loaded_label, nrows, ncols);

    if (nrows != num_data || ncols != num_feature) {
        std::cerr << "Loaded data shape mismatch\n";
        return 1;
    }

    std::vector<int> indices(nrows);
    std::iota(indices.begin(), indices.end(), 0);
    std::mt19937 rng(42);
    std::shuffle(indices.begin(), indices.end(), rng);

    int train_rows = static_cast<int>(nrows * 0.8);
    int test_rows = nrows - train_rows;

    std::vector<float> train_data;
    std::vector<float> train_label;
    std::vector<float> test_data;
    std::vector<float> test_label;

    train_data.reserve(train_rows * ncols);
    train_label.reserve(train_rows);
    test_data.reserve(test_rows * ncols);
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

    DatasetHandle train_dataset;
    check(LGBM_DatasetCreateFromMat(
              train_data.data(), C_API_DTYPE_FLOAT32,
              train_rows, ncols,
              1, "", nullptr,
              &train_dataset),
          "DatasetCreateFromMat");
    check(LGBM_DatasetSetField(train_dataset, "label", train_label.data(), train_rows, C_API_DTYPE_FLOAT32),
          "DatasetSetField(label)");

    BoosterHandle booster;
    const char *params = "objective=binary learning_rate=0.1 num_leaves=31 metric=binary_logloss verbosity=1";
    check(LGBM_BoosterCreate(train_dataset, params, &booster), "BoosterCreate");

    for (int iter = 0; iter < 100; ++iter) {
        int is_finished;
        check(LGBM_BoosterUpdateOneIter(booster, &is_finished), "BoosterUpdateOneIter");
        if (is_finished) break;
    }

    auto computeAccuracy = [&](const std::vector<float> &dataset,
                               const std::vector<float> &labels,
                               int rows) {
        std::vector<double> preds(rows);
        int64_t out_len = 0;
        check(LGBM_BoosterPredictForMat(
                  booster,
                  dataset.data(),
                  C_API_DTYPE_FLOAT32,
                  rows,
                  ncols,
                  1,
                  C_API_PREDICT_NORMAL,
                  0,
                  -1,
                  "",
                  &out_len,
                  preds.data()),
              "BoosterPredictForMat");

        if (out_len != rows) {
            std::cerr << "Unexpected prediction length: " << out_len << " expected " << rows << "\n";
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

    std::cout << "Train accuracy: " << train_acc << "\n";
    std::cout << "Test accuracy: " << test_acc << "\n";

    check(LGBM_BoosterFree(booster), "BoosterFree");
    check(LGBM_DatasetFree(train_dataset), "DatasetFree");

    return 0;
}
