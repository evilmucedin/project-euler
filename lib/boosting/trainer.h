#pragma once

// Gradient-boosting trainer over oblivious trees, following CatBoost's design:
// quantile feature binarization, level-wise symmetric tree growth by histogram
// split search, and second-order (Newton) leaf estimates with L2 leaf
// regularization. Supports RMSE regression and LogLoss binary classification.

#include <cstdint>

#include "lib/boosting/dataset.h"
#include "lib/boosting/model.h"

namespace boosting {

struct TTrainParams {
    uint32_t iterations = 100;
    uint32_t depth = 6;
    double learningRate = 0.1;
    double l2LeafReg = 3.0;
    uint32_t borderCount = 128;
    ELoss loss = ELoss::RMSE;
    double subsample = 1.0;  // Bernoulli row sampling rate per tree
    uint32_t seed = 42;
};

class TTrainer {
   public:
    explicit TTrainer(const TTrainParams& params);

    TModel train(const TDataset& dataset) const;

   private:
    TTrainParams params_;
};

}  // namespace boosting
