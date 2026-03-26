#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
#include <sstream>
#include <string>
#include <vector>
#include <cstdio>

#include <LightGBM/c_api.h>
#include <catboost/libs/model_interface/model_calcer_wrapper.h>
#include <filesystem>

static const int MIN_FEATURES = 100;
static const int N_IT = 5;

static void check(int status, const std::string& msg) {
  if (status != 0) {
    std::cerr << "LightGBM error: " << msg << " (code " << status << ")\n";
    std::exit(status);
  }
}

static void printModelQuality(const std::string& name, double train_acc, double test_acc) {
  std::cout << "=== " << name << " model quality ===\n";
  std::cout << "Train accuracy: " << train_acc << "\n";
  std::cout << "Test accuracy:  " << test_acc << "\n";
}

struct DatasetSplit {
  std::vector<float> train_data;
  std::vector<float> train_label;
  std::vector<float> test_data;
  std::vector<float> test_label;
  int train_rows = 0;
  int test_rows = 0;
};

static DatasetSplit splitDataset(const std::vector<float>& loaded_data,
                                 const std::vector<float>& loaded_label,
                                 int nrows,
                                 int ncols,
                                 unsigned seed = 42) {
  std::vector<int> indices(nrows);
  std::iota(indices.begin(), indices.end(), 0);
  std::mt19937 rng(seed);
  std::shuffle(indices.begin(), indices.end(), rng);

  int train_rows = static_cast<int>(nrows * 0.8);
  int test_rows = nrows - train_rows;

  DatasetSplit split;
  split.train_rows = train_rows;
  split.test_rows = test_rows;

  split.train_data.reserve(train_rows * ncols);
  split.test_data.reserve(test_rows * ncols);
  split.train_label.reserve(train_rows);
  split.test_label.reserve(test_rows);

  for (int i = 0; i < train_rows; ++i) {
    int idx = indices[i];
    for (int j = 0; j < ncols; ++j) {
      split.train_data.push_back(loaded_data[idx * ncols + j]);
    }
    split.train_label.push_back(loaded_label[idx]);
  }
  for (int i = train_rows; i < nrows; ++i) {
    int idx = indices[i];
    for (int j = 0; j < ncols; ++j) {
      split.test_data.push_back(loaded_data[idx * ncols + j]);
    }
    split.test_label.push_back(loaded_label[idx]);
  }

  return split;
}

static double binaryAccuracy(const std::vector<double>& preds,
                             const std::vector<float>& labels) {
  int n = static_cast<int>(preds.size());
  if (static_cast<int>(labels.size()) != n) {
    std::cerr << "binaryAccuracy: label/prediction size mismatch\n";
    std::exit(1);
  }
  int correct = 0;
  for (int i = 0; i < n; ++i) {
    int pred = (preds[i] > 0.5) ? 1 : 0;
    if (pred == static_cast<int>(labels[i])) {
      ++correct;
    }
  }
  return n > 0 ? static_cast<double>(correct) / n : 0.0;
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

void testLGBM(const std::string& path, const std::vector<float>& loaded_data, const std::vector<float>& loaded_label, const int& nrows, const int& ncols) {
  DatasetSplit split = splitDataset(loaded_data, loaded_label, nrows, ncols, 42);

  DatasetHandle train_dataset = nullptr;
  check(LGBM_DatasetCreateFromMat(split.train_data.data(), C_API_DTYPE_FLOAT32,
                                  split.train_rows, ncols, 1, "", nullptr,
                                  &train_dataset),
        "DatasetCreateFromMat");
  check(LGBM_DatasetSetField(train_dataset, "label", split.train_label.data(),
                             split.train_rows, C_API_DTYPE_FLOAT32),
        "DatasetSetField(label)");

  BoosterHandle booster = nullptr;
  const char* params =
      "objective=binary learning_rate=0.1 num_leaves=31 metric=binary_logloss "
      "verbosity=1";
  check(LGBM_BoosterCreate(train_dataset, params, &booster), "BoosterCreate");

  int actual_iters = 0;
  for (int iter = 0; iter < 500; ++iter) {
    int is_finished = 0;
    check(LGBM_BoosterUpdateOneIter(booster, &is_finished),
          "BoosterUpdateOneIter");
    actual_iters = iter + 1;
    if (is_finished) break;
  }

  const char* lgbm_model_file = "lightgbm_model.txt";
  // LightGBM C API signature: (BoosterHandle, start_iteration, num_iteration, feature_importance_type, filename)
  // to save all built trees, use start_iteration=0, num_iteration=actual_iters
  check(LGBM_BoosterSaveModel(booster, 0, actual_iters, 0, lgbm_model_file), "BoosterSaveModel");
  std::cout << "Saved LightGBM model to " << lgbm_model_file << "\n";

  auto computeAccuracy = [&](const std::vector<float>& dataset,
                             const std::vector<float>& labels, int rows) {
    std::vector<double> preds(rows);
    int64_t out_len = 0;

    for (int it = 0; it < N_IT; ++it) {
        auto t0 = std::chrono::high_resolution_clock::now();
        check(LGBM_BoosterPredictForMat(booster, dataset.data(), C_API_DTYPE_FLOAT32,
                                        rows, ncols, 1, C_API_PREDICT_NORMAL, 0, -1,
                                        "", &out_len, preds.data()),
              "BoosterPredictForMat");
        auto t1 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = t1 - t0;
        std::cout << "LGBM_BoosterPredictForMat time: " << elapsed.count() << " ms " << path << ", nrows: " << nrows << "\n";
    }

    if (out_len != static_cast<int64_t>(rows)) {
      std::cerr << "Unexpected prediction length: " << out_len << " expected "
                << rows << "\n";
      std::exit(1);
    }

    return binaryAccuracy(preds, labels);
  };

  double train_acc = computeAccuracy(split.train_data, split.train_label, split.train_rows);
  double test_acc = computeAccuracy(split.test_data, split.test_label, split.test_rows);

  printModelQuality("ML", train_acc, test_acc);

  check(LGBM_BoosterFree(booster), "BoosterFree");
  check(LGBM_DatasetFree(train_dataset), "DatasetFree");
}

#include <iostream>
#include <vector>
#include <dlfcn.h>
#include <tl2cgen/detail/predictor/shared_library.h> // Included in generated files

void testTl2cgen(const std::string& path, const std::vector<float>& loaded_data, const std::vector<float>& loaded_label, const int& nrows, const int& ncols) {
  DatasetSplit split = splitDataset(loaded_data, loaded_label, nrows, ncols, 42);

  DatasetHandle train_dataset = nullptr;
  check(LGBM_DatasetCreateFromMat(split.train_data.data(), C_API_DTYPE_FLOAT32,
                                  split.train_rows, ncols, 1, "", nullptr,
                                  &train_dataset),
        "DatasetCreateFromMat");
  check(LGBM_DatasetSetField(train_dataset, "label", split.train_label.data(),
                             split.train_rows, C_API_DTYPE_FLOAT32),
        "DatasetSetField(label)");

  BoosterHandle booster = nullptr;
  const char* params =
      "objective=binary learning_rate=0.1 num_leaves=31 metric=binary_logloss "
      "verbosity=1";
  check(LGBM_BoosterCreate(train_dataset, params, &booster), "BoosterCreate");

  int actual_iters = 0;
  for (int iter = 0; iter < 500; ++iter) {
    int is_finished = 0;
    check(LGBM_BoosterUpdateOneIter(booster, &is_finished),
          "BoosterUpdateOneIter");
    actual_iters = iter + 1;
    if (is_finished) break;
  }

  const char* lgbm_model_file = "lightgbm_model.txt";
  // LightGBM C API signature: (BoosterHandle, start_iteration, num_iteration, feature_importance_type, filename)
  // to save all built trees, use start_iteration=0, num_iteration=actual_iters
  check(LGBM_BoosterSaveModel(booster, 0, actual_iters, 0, lgbm_model_file), "BoosterSaveModel");
  std::cout << "Saved LightGBM model to " << lgbm_model_file << "\n";

  auto computeAccuracy = [&](const std::vector<float>& dataset,
                             const std::vector<float>& labels, int rows) {
    std::vector<double> preds(rows);
    int64_t out_len = 0;

    for (int it = 0; it < N_IT; ++it) {
        auto t0 = std::chrono::high_resolution_clock::now();
        check(LGBM_BoosterPredictForMat(booster, dataset.data(), C_API_DTYPE_FLOAT32,
                                        rows, ncols, 1, C_API_PREDICT_NORMAL, 0, -1,
                                        "", &out_len, preds.data()),
              "BoosterPredictForMat");
        auto t1 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = t1 - t0;
        std::cout << "Tl2cgen_BoosterPredictForMat time: " << elapsed.count() << " ms " << path << ", nrows: " << nrows << "\n";
    }

    if (out_len != static_cast<int64_t>(rows)) {
      std::cerr << "Unexpected prediction length: " << out_len << " expected "
                << rows << "\n";
      std::exit(1);
    }

    return binaryAccuracy(preds, labels);
  };

  double train_acc = computeAccuracy(split.train_data, split.train_label, split.train_rows);
  double test_acc = computeAccuracy(split.test_data, split.test_label, split.test_rows);

  printModelQuality("ML", train_acc, test_acc);

  check(LGBM_BoosterFree(booster), "BoosterFree");
  check(LGBM_DatasetFree(train_dataset), "DatasetFree");
}

bool trainCatBoostModel(const std::string& data_path, const std::string& model_path) {
  if (std::filesystem::exists(model_path)) {
    return true;
  }

  const std::string catboost_repo = "/home/denplusplus/Programming/catboost";
  const std::string command =
      "PYTHONPATH=" + catboost_repo + " python3 - <<'PY'\n"
      "import os, random, csv, sys\n"
      "from catboost import CatBoostClassifier\n"
      "data_path = r'" + data_path + "'\n"
      "model_path = r'" + model_path + "'\n"
      "data = []\n"
      "labels = []\n"
      "with open(data_path, 'r') as f:\n"
      "    reader = csv.reader(f)\n"
      "    for row in reader:\n"
      "        if not row: continue\n"
      "        floats = [float(x) for x in row if x != '']\n"
      "        if not floats: continue\n"
      "        data.append(floats[:-1])\n"
      "        labels.append(int(floats[-1]))\n"
      "n = len(data)\n"
      "if n == 0:\n"
      "    sys.exit(1)\n"
      "indices = list(range(n))\n"
      "random.seed(42)\n"
      "random.shuffle(indices)\n"
      "train_n = int(n*0.8)\n"
      "X_train = [data[i] for i in indices[:train_n]]\n"
      "y_train = [labels[i] for i in indices[:train_n]]\n"
      "X_test = [data[i] for i in indices[train_n:]]\n"
      "y_test = [labels[i] for i in indices[train_n:]]\n"
      "model = CatBoostClassifier(iterations=50, learning_rate=0.1, depth=3, verbose=False)\n"
      "model.fit(X_train, y_train)\n"
      "model.save_model(model_path)\n"
      "pred_train = model.predict(X_train)\n"
      "pred_test = model.predict(X_test)\n"
      "acc = lambda p, t: sum(int(x)==int(y) for x,y in zip(p,t))/len(t)\n"
      "print('=== CatBoost model quality (from C++) ===')\n"
      "print('Train accuracy:', acc(pred_train, y_train))\n"
      "print('Test accuracy:', acc(pred_test, y_test))\n"
      "PY";

  std::cout << "Running CatBoost training via Python script from C++...\n";
  int rc = std::system(command.c_str());
  if (rc != 0) {
    std::cerr << "CatBoost training failed (exit code " << rc << ")\n";
    return false;
  }
  return std::filesystem::exists(model_path);
}

#include <cstdlib>
#include <iostream>
// #include <catboost/libs/model/model.h>

void testCatBoostCalcer(
    const std::string& file_path,
    ModelCalcerHandle* calcer,
    const int& nrows,
    const int& ncols,
    const DatasetSplit& split) {


  // TFullModel& fullModel = *calcer->FullModel;

  auto evaluate = [&](const std::vector<float>& data, const std::vector<float>& label, int rows) {
    std::vector<const float*> features(rows);
    for (int i = 0; i < rows; ++i) {
      features[i] = &data[i * ncols];
    }

    std::vector<double> preds(rows);


    for (int it = 0; it < N_IT; ++it) {
        auto t0 = std::chrono::high_resolution_clock::now();
        bool ok = CalcModelPredictionFlat(calcer, rows, features.data(), ncols, preds.data(), rows);
        auto t1 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = t1 - t0;
        std::cout << "CalcModelPredictionFlat time: " << elapsed.count() << " ms, path: " << file_path <<   "  rows: " << rows << "\n";

        if (!ok) {
          std::cerr << "CatBoost CalcModelPredictionFlat failed: " << GetErrorString() << " rows: " << rows << "\n";
          return 0.0;
        }
    }

    return binaryAccuracy(preds, label);
  };

  double train_acc = evaluate(split.train_data, split.train_label, split.train_rows);
  double test_acc = evaluate(split.test_data, split.test_label, split.test_rows);

  printModelQuality("CatBoost", train_acc, test_acc);
}

void testCatBoost(const std::string& model_path,
                  const std::vector<float>& loaded_data,
                  const std::vector<float>& loaded_label,
                  const int& nrows,
                  const int& ncols) {
  if (!std::filesystem::exists(model_path)) {
    std::cerr << "CatBoost model file not found: " << model_path << ". Will attempt to train from data.\n";
    // model path not found; attempt training from data splits in C++ via Python
    // (this is the same logic as old catboost_run.py, but launched from C++)
    if (!trainCatBoostModel("data.data", model_path)) {
      std::cerr << "CatBoost training and model creation failed. Skipping CatBoost test.\n";
      return;
    }
  }

  DatasetSplit split = splitDataset(loaded_data, loaded_label, nrows, ncols, 42);

  ModelCalcerHandle* calcer = ModelCalcerCreate();
  if (!calcer) {
    std::cerr << "CatBoost ModelCalcerCreate() failed\n";
    return;
  }

  if (!LoadFullModelFromFile(calcer, model_path.c_str())) {
    std::cerr << "CatBoost LoadFullModelFromFile failed: " << GetErrorString() << "\n";
    ModelCalcerDelete(calcer);
    return;
  }

  if (!SetPredictionType(calcer, APT_PROBABILITY)) {
    std::cerr << "CatBoost SetPredictionType failed: " << GetErrorString() << "\n";
    ModelCalcerDelete(calcer);
    return;
  }

  testCatBoostCalcer(model_path, calcer, nrows, ncols, split);

  ModelCalcerDelete(calcer);
}

#include <onnxruntime/core/session/onnxruntime_cxx_api.h>

void testCatBoostLGBM(const std::string& model_path,
                  const std::vector<float>& loaded_data,
                  const std::vector<float>& loaded_label,
                  const int& nrows,
                  const int& ncols) {
  if (!std::filesystem::exists(model_path)) {
    std::cerr << "CatBoost model file not found: " << model_path << ". Will attempt to train from data.\n";
    // model path not found; attempt training from data splits in C++ via Python
    // (this is the same logic as old catboost_run.py, but launched from C++)
    if (!trainCatBoostModel("data.data", model_path)) {
      std::cerr << "CatBoost training and model creation failed. Skipping CatBoost test.\n";
      return;
    }
  }

  DatasetSplit split = splitDataset(loaded_data, loaded_label, nrows, ncols, 42);

  ModelCalcerHandle* calcer = ModelCalcerCreate();
  if (!calcer) {
    std::cerr << "CatBoost ModelCalcerCreate() failed\n";
    return;
  }

  if (!LoadFullModelFromFile(calcer, model_path.c_str())) {
    std::cerr << "CatBoost LoadFullModelFromFile failed: " << GetErrorString() << "\n";
    ModelCalcerDelete(calcer);
    return;
  }

  if (!SetPredictionType(calcer, APT_PROBABILITY)) {
    std::cerr << "CatBoost SetPredictionType failed: " << GetErrorString() << "\n";
    ModelCalcerDelete(calcer);
    return;
  }

  testCatBoostCalcer(model_path, calcer, nrows, ncols, split);

  ModelCalcerDelete(calcer);
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

  testLGBM(path, loaded_data, loaded_label, nrows, ncols);
  testTl2cgen(path, loaded_data, loaded_label, nrows, ncols);

  const std::string catBoostModelPath = "/home/denplusplus/Programming/project-euler/GdmMlTest/catboostTestModel.bin";
  testCatBoost(catBoostModelPath, loaded_data, loaded_label, nrows, ncols);

  const std::string catBoostModelLGBMPath = "/home/denplusplus/Programming/project-euler/GdmMlTest/lightgbm_model.cbm";
  testCatBoostLGBM(catBoostModelLGBMPath, loaded_data, loaded_label, nrows, ncols);

  return 0;
}
