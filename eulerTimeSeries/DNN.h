#pragma once

#include "lib/header.h"

class DNNModel {
public:
    class Impl;

    DNNModel(bool lstm);
    DNNModel(const Impl& impl);
    ~DNNModel();

    double predict(const DoubleVector& features);
    double predictLSTM(const vector<DoubleVector>& features);
    void save(const std::string& filename);
    void saveJson(const std::string& filename);
    void set(shared_ptr<DNNModel> model);
    Impl& getImpl();

    unique_ptr<Impl> impl_;
};

using PDNNModel = shared_ptr<DNNModel>;

struct DNNModelTrainer {
public:
    DNNModelTrainer(double learningRate, double scaleRate, size_t samples, bool lstm);
    ~DNNModelTrainer();

    PDNNModel getModel();
    void setModel(PDNNModel model);
    void train(const DoubleVector& features, double label);
    void train(const vector<DoubleVector>& features, const DoubleVector& label);
    void slowdown();
    void scale(double value);

    void startTrainLSTM();
    void trainLSTM(const vector<DoubleVector>& features, const DoubleVector& labels);
    void stopTrainLSTM();

    class Impl;
    unique_ptr<Impl> impl_;
};

using StockFeatures = vector<DoubleVector>;
static constexpr size_t kDNNFeatures = 11;
static constexpr size_t kQuants = 24 * 60;
static constexpr size_t kDNNWindow = 1;
static constexpr size_t kDNNHorizon = 10;

enum DNNFeature {
    FI_VOLUME = 0,
    FI_PRICE = 1,
    FI_TRADES = 2,
    FI_LAST = 3,
    FI_TDF = 4,
    FI_BID = 5,
    FI_ASK = 6,
    FI_BIDSIZE = 7,
    FI_ASKSIZE = 8,
    FI_CURRENT_PRICE = 9,
    FI_INTERCEPT = 10,
};
