#include <LightGBM/c_api.h>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

static void check(int status, const std::string &msg) {
    if (status != 0) {
        std::cerr << "LightGBM error: " << msg << " (code " << status << ")\n";
        std::exit(status);
    }
}

int main() {
    // 1) Generate simple synthetic dataset (binary classification)
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

    // 2) Create LightGBM Dataset in memory
    DatasetHandle train_data;
    check(LGBM_DatasetCreateFromMat(
              data.data(), C_API_DTYPE_FLOAT32,
              num_data, num_feature,
              1,  // row major
              "", nullptr,
              &train_data),
          "DatasetCreateFromMat");

    check(LGBM_DatasetSetField(train_data, "label", label.data(), num_data, C_API_DTYPE_FLOAT32),
          "DatasetSetField(label)");

    // 3) Create Booster and train
    BoosterHandle booster;
    const char *params = "objective=binary learning_rate=0.1 num_leaves=31 metric=binary_logloss verbosity=1";
    check(LGBM_BoosterCreate(train_data, params, &booster), "BoosterCreate");

    for (int iter = 0; iter < 100; ++iter) {
        int is_finished;
        check(LGBM_BoosterUpdateOneIter(booster, &is_finished), "BoosterUpdateOneIter");
        if (is_finished) {
            break;
        }
    }

    // 4) Evaluate the model on the same data (for demonstration) and compute binary accuracy
    std::vector<double> predictions(num_data);
    int64_t out_len = 0;
    check(LGBM_BoosterPredictForMat(
              data.data(), C_API_DTYPE_FLOAT32,
              num_data, num_feature,
              1,  // row major
              C_API_PREDICT_NORMAL,
              -1,  // use all trees
              "", booster,
              predictions.data(), &out_len),
          "BoosterPredictForMat");

    if (out_len != num_data) {
        std::cerr << "Unexpected prediction length: " << out_len << " expected " << num_data << "\n";
    }

    int correct = 0;
    for (int i = 0; i < num_data; ++i) {
        int pred = (predictions[i] > 0.5) ? 1 : 0;
        if (pred == static_cast<int>(label[i])) {
            ++correct;
        }
    }

    double accuracy = static_cast<double>(correct) / num_data;
    std::cout << "Model accuracy on training data: " << accuracy << "\n";

    // 5) Cleanup
    check(LGBM_BoosterFree(booster), "BoosterFree");
    check(LGBM_DatasetFree(train_data), "DatasetFree");

    return 0;
}
