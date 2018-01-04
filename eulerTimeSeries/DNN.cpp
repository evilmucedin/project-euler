#include "DNN.h"

#include "tiny_dnn/tiny_dnn.h"

namespace {
tiny_dnn::tensor_t doubleVectorToTensor(const DoubleVector& features) {
    tiny_dnn::tensor_t input;
    tiny_dnn::vec_t fFeatures(features.size());
    for (size_t i = 0; i < features.size(); ++i) {
        fFeatures[i] = features[i];
    }
    input.emplace_back(std::move(fFeatures));
    return input;
}
}  // namespace

class DNNModel::Impl {
   public:
    Impl() {
        using fc = tiny_dnn::layers::fc;
        using tanh = tiny_dnn::activation::tanh;
        using relu = tiny_dnn::activation::relu;

        const auto backend_type = tiny_dnn::core::backend_t::internal;

        nn_ << fc(kDNNWindow * kDNNFeatures, 5, true, backend_type) << relu() << fc(5, 1, true, backend_type) << tanh();
    }

    Impl(const Impl& impl) { nn_ = impl.nn_; }

    double predict(const DoubleVector& features) {
        return nn_.predict(doubleVectorToTensor(features))[0][0];
    }

    using NN = tiny_dnn::network<tiny_dnn::sequential>;

    NN& getNN() { return nn_; }

   private:
    NN nn_;
};

DNNModel::DNNModel() : impl_(make_unique<DNNModel::Impl>()) {}

DNNModel::DNNModel(const DNNModel::Impl& impl) : impl_(make_unique<DNNModel::Impl>(impl)) {}

DNNModel::~DNNModel() {}

double DNNModel::predict(const DoubleVector& features) { return impl_->predict(features); }

class DNNModelTrainer::Impl {
   public:
    Impl() { optimizer_.alpha *= 0.01; }

    PDNNModel getModel() { return make_shared<DNNModel>(model_); }

    void train(const DoubleVector& features, double label) {
        ENFORCE_EQ(features.size(), kDNNFeatures * kDNNWindow);
        vector<tiny_dnn::tensor_t> vInput;
        vInput.emplace_back(doubleVectorToTensor(features));
        vector<tiny_dnn::label_t> output;
        output.emplace_back(label);
        model_.getNN().fit<tiny_dnn::mse>(optimizer_, vInput, output, 1, 1, [] {}, [] {});
    }

   private:
    DNNModel::Impl model_;
    tiny_dnn::adagrad optimizer_;
};

DNNModelTrainer::DNNModelTrainer() : impl_(make_unique<DNNModelTrainer::Impl>()) {}

PDNNModel DNNModelTrainer::getModel() { return impl_->getModel(); }

void DNNModelTrainer::train(const DoubleVector& features, double label) { impl_->train(features, label); }

DNNModelTrainer::~DNNModelTrainer() {}
