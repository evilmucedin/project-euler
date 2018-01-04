#pragma once

#include "lib/header.h"

class DNNModel {
public:
    class Impl;

    DNNModel();
    DNNModel(const Impl& impl);
    ~DNNModel();

    double predict(const DoubleVector& features);

    unique_ptr<Impl> impl_;
};

using PDNNModel = shared_ptr<DNNModel>;

struct DNNModelTrainer {
public:
    DNNModelTrainer();
    ~DNNModelTrainer();

    PDNNModel getModel();
    void train(const DoubleVector& features, double label);
    void train(const vector<DoubleVector>& features, const DoubleVector& label);
    void slowdown();

    class Impl;
    unique_ptr<Impl> impl_;
};

using StockFeatures = vector<DoubleVector>;
static constexpr size_t kDNNFeatures = 5;
static constexpr size_t kQuants = 24 * 60;
static constexpr size_t kDNNWindow = 100;
static constexpr size_t kDNNHorizon = 10;
