#include "DNN.h"

#include "tiny_dnn/tiny_dnn.h"

#include "lib/random.h"
#include "lib/io/fstream.h"

namespace {
tiny_dnn::vec_t doubleVectorToVector(const DoubleVector& features) {
    tiny_dnn::vec_t fFeatures(features.size());
    for (size_t i = 0; i < features.size(); ++i) {
        fFeatures[i] = features[i];
    }
    return fFeatures;
}

tiny_dnn::tensor_t doubleVectorToTensor(const DoubleVector& features) {
    tiny_dnn::tensor_t input;
    input.emplace_back(doubleVectorToVector(features));
    return input;
}
}  // namespace

class DNNModel::Impl {
   public:
    Impl() {
        using fc = tiny_dnn::layers::fc;
        using tanh = tiny_dnn::activation::tanh;
        using relu = tiny_dnn::activation::relu;
        using ll = tiny_dnn::linear_layer;

        const auto backend_type = tiny_dnn::core::backend_t::avx;

        static constexpr size_t kFeatures = kDNNWindow * kDNNFeatures;

        // nn_ << fc(kFeatures, 100, true, backend_type) << relu() << fc(100, 10, true, backend_type) << tanh() << fc(10, 1, true, backend_type) << tanh() << fc(1, 1, true, backend_type);
        // nn_ << fc(kFeatures, 10, true, backend_type) << ll(10);
        // nn_ << fc(kDNNWindow * kDNNFeatures, 1) << tanh() << fc(1, 1);
        nn_ << fc(kFeatures, 20, true, backend_type) << relu() << fc(20, 1, true, backend_type) << tanh() << fc(1, 1, true, backend_type);
        // nn_.weight_init(tiny_dnn::weight_init::he(1e-3));
        nn_.bias_init(tiny_dnn::weight_init::constant(0));
        nn_.weight_init(tiny_dnn::weight_init::constant(0));
        // nn_.weight_init(tiny_dnn::weight_init::xavier(0.01));
        // nn_.bias_init(tiny_dnn::weight_init::xavier());
        // nn_.init_weight();

        auto w = nn_[0]->weights();
        auto w0 = *(w[0]);
        for (size_t i = 0; i < w0.size(); ++i) {
            if (oneIn(100)) {
                w0[i] = 1.0;
            }
        }
        /*
        */
    }

    Impl(const Impl& impl) { nn_ = impl.nn_; }

    double predict(const DoubleVector& features) {
        return nn_.predict(doubleVectorToTensor(features))[0][0];
    }

    void scale(double value) {
        if (value == 1.0) {
            return;
        }

        for (size_t i = 0; i < nn_.layer_size(); ++i) {
            auto weights = nn_[i]->weights();
            for (auto& pv: weights) {
                for (auto& x: *pv) {
                    x *= value;
                }
            }
        }
    }

    void save(const std::string& filename) {
        nn_.save(filename);
    }

    void saveJson(const std::string& filename) {
        auto json = nn_.to_json(tiny_dnn::content_type::weights_and_model);
        OFStream ofs(filename);
        ofs << json;
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

void DNNModel::save(const std::string& filename) { impl_->save(filename); }

void DNNModel::saveJson(const std::string& filename) { impl_->saveJson(filename); }

class DNNModelTrainer::Impl {
   public:
    Impl(double learningRate, double scaleRate) {
        optimizer_.alpha *= learningRate;
        scaleRate_ = scaleRate;
    }

    PDNNModel getModel() { return make_shared<DNNModel>(model_); }

    void slowdown() {
        optimizer_.alpha *= 0.95;
        model_.scale(scaleRate_);
    }

    void train(const DoubleVector& features, double label) {
        ENFORCE_EQ(features.size(), kDNNFeatures * kDNNWindow);
        vector<tiny_dnn::tensor_t> vInput;
        vInput.emplace_back(doubleVectorToTensor(features));
        vector<tiny_dnn::vec_t> output(1);
        output.back().emplace_back(label);
        ENFORCE(model_.getNN().fit<tiny_dnn::mse>(optimizer_, vInput, output, 1, 1, tiny_dnn::nop, tiny_dnn::nop));
    }

    void train(const vector<DoubleVector>& features, const DoubleVector& label) {
        if (features.empty()) {
            return;
        }

        ENFORCE_EQ(features.size(), label.size());
        ENFORCE_EQ(features[0].size(), kDNNFeatures * kDNNWindow);

        IntVector perm(features.size());
        for (size_t i = 0; i < features.size(); ++i) {
            perm[i] = i;
        }
        shuffle(perm);

        tiny_dnn::tensor_t vInput(features.size());
        for (size_t i = 0; i < features.size(); ++i) {
            vInput[perm[i]] = doubleVectorToVector(features[i]);
        }
        vector<tiny_dnn::vec_t> output(label.size());
        for (size_t i = 0; i < label.size(); ++i) {
            output[perm[i]].emplace_back(label[i]);
        }
        ENFORCE(model_.getNN().fit<tiny_dnn::mse>(optimizer_, vInput, output, min<size_t>(features.size(), 128), 1, tiny_dnn::nop, tiny_dnn::nop));
    }

   private:
    DNNModel::Impl model_;
    tiny_dnn::nesterov_momentum optimizer_;
    double scaleRate_;
};

DNNModelTrainer::DNNModelTrainer(double learningRate, double scaleRate)
    : impl_(make_unique<DNNModelTrainer::Impl>(learningRate, scaleRate)) {
}

DNNModelTrainer::~DNNModelTrainer() {}

PDNNModel DNNModelTrainer::getModel() { return impl_->getModel(); }

void DNNModelTrainer::train(const DoubleVector& features, double label) { impl_->train(features, label); }

void DNNModelTrainer::train(const vector<DoubleVector>& features, const DoubleVector& label) {
    impl_->train(features, label);
}

void DNNModelTrainer::slowdown() { impl_->slowdown(); }


