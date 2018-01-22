#include "DNN.h"

#include "glog/logging.h"
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
        construct();
    }

    void construct() {
        nn_ = make_shared<NN>();
        using fc = tiny_dnn::layers::fc;
        using pc = tiny_dnn::pc;
        using pc2 = tiny_dnn::partial_connected;
        using tanh = tiny_dnn::activation::tanh;
        using relu = tiny_dnn::activation::relu;
        using lrelu = tiny_dnn::activation::leaky_relu;
        using ll = tiny_dnn::linear_layer;
        using conv = tiny_dnn::layers::conv;
        using ave_pool = tiny_dnn::layers::ave_pool;
        using padding = tiny_dnn::padding;
        using mul = tiny_dnn::mul_layer;
        using cc = tiny_dnn::layers::concat;

        const auto backend_type = tiny_dnn::core::backend_t::avx;

        static constexpr size_t kFeatures = kDNNWindow * kDNNFeatures;

        static constexpr size_t kPCFeatures = kDNNFeatures;

        vector<size_t> connections2;
        for (size_t i = 0; i < kPCFeatures; ++i) {
            connections2.emplace_back(kFeatures - kPCFeatures + i);
        }
        /*
        */

        vector<tiny_dnn::partial_connection> connections;
        for (size_t i = 0; i < kDNNWindow; ++i) {
            for (size_t j = 0; j < kDNNFeatures; ++j) {
                tiny_dnn::partial_connection c;
                c.in_index_ = i*kDNNFeatures + j;
                c.out_index_ = i;
                c.weight_index_ = c.in_index_;
                connections.emplace_back(std::move(c));
            }
        }
        /*
        */

        // nn_ << pc2(kFeatures, kDNNWindow, kFeatures, connections) << fc(kDNNWindow, 6*6, false, backend_type) << conv(6, 6, 3, 1, 6) << tanh() << fc(16*6, 1, false, backend_type) << tanh() << fc(1, 1, false, backend_type);
        // nn_ << pc2(kFeatures, kDNNWindow, kFeatures, connections) << fc(kDNNWindow, 5*5, false, backend_type) << lrelu() << fc(5*5, 5, false, backend_type) << lrelu() << fc(5, 1, false, backend_type);

        static constexpr size_t kHidden1 = 5;
        static constexpr size_t kHidden2 = 5;

        vector<tiny_dnn::partial_connection> connections3;
        for (size_t j = 0; j < kHidden2; ++j) {
            for (size_t i = 0; i < kDNNWindow; ++i) {
                tiny_dnn::partial_connection c;
                c.in_index_ = i;
                c.out_index_ = (i * kHidden1) / kDNNWindow + j * kHidden1;
                c.weight_index_ = c.in_index_;
                connections3.emplace_back(std::move(c));
            }
        }

        auto in = make_shared<tiny_dnn::layers::input>(tiny_dnn::shape3d(kFeatures, 1, 1));
        auto partial1 = make_shared<pc2>(kFeatures, kDNNWindow, kFeatures, connections);
        auto partial3 = make_shared<pc2>(kDNNWindow, kHidden1*kHidden2, kDNNWindow, connections3);
        auto fc1 = make_shared<fc>(kHidden1 * kHidden2, kHidden1, false, backend_type);
        auto relu1 = make_shared<relu>();
        *in << *partial1 << *partial3 << *fc1 << *relu1;
        auto partial2 = make_shared<pc>(kFeatures, connections2.size(), connections2);
        auto fc2 = make_shared<fc>(connections2.size(), connections2.size(), false, backend_type);
        auto relu2 = make_shared<relu>();
        // auto fc3 = make_shared<fc>(connections2.size(), connections2.size(), false, backend_type);
        // auto relu3 = make_shared<relu>();
        *in << *partial2 << *fc2 << *relu2;
        auto c =
            shared_ptr<cc>(new cc({tiny_dnn::shape3d(1, 1, kHidden1), tiny_dnn::shape3d(1, 1, connections2.size())}));
        (*relu1, *relu2) << *c;
        auto out = make_shared<fc>(connections2.size() + kHidden1, 1, false, backend_type);
        *c << *out;
        // auto out = make_shared<fc>(1, 1, false, backend_type);
        // *fc1 << *out;
        tiny_dnn::construct_graph(*nn_, {in.get()}, {out.get()});

        layers_.emplace_back(in);
        layers_.emplace_back(partial1);
        layers_.emplace_back(partial3);
        layers_.emplace_back(fc1);
        layers_.emplace_back(relu1);
        layers_.emplace_back(partial2);
        layers_.emplace_back(fc2);
        layers_.emplace_back(relu2);
        // layers_.emplace_back(fc3);
        // layers_.emplace_back(relu3);
        layers_.emplace_back(c);
        layers_.emplace_back(out);

        // nn_ << pc2() << fc(kDNNWindow, 1, false, backend_type);
        // nn_ << pc2(kFeatures, kDNNWindow, kFeatures, connections) << fc(kDNNWindow, 2, false, backend_type) << lrelu() << mul(2) << fc(2*2, 1, false, backend_type);
        // nn_ << fc(kFeatures, 80, false, backend_type) << lrelu() << fc(80, 1, false, backend_type);
        // nn_ << pc(kFeatures, kPCFeatures, connections) << fc(kPCFeatures, 4, false, backend_type) << lrelu() << fc(4, 1, false, backend_type);
        // nn_ << pc(kFeatures, kPCFeatures, connections) << fc(kPCFeatures, 1, false, backend_type);
        // nn_ << pc(kFeatures, kDNNFeatures, connections) << fc(kDNNFeatures, 1, false, backend_type);
        // nn_ << pc(kFeatures, kDNNFeatures, connections) << fc(kDNNFeatures, 4, false, backend_type) << lrelu() << fc(4, 1, false, backend_type) << tanh() << fc(1, 1, false, backend_type);
        // nn_ << pc(kFeatures, kDNNFeatures, connections) << fc(kDNNFeatures, kDNNFeatures*kDNNFeatures, true, backend_type) << lrelu() << fc(kDNNFeatures*kDNNFeatures, 1, true, backend_type) << tanh();
        // nn_ << fc(kFeatures, 100, true, backend_type) << relu() << fc(100, 10, true, backend_type) << tanh() << fc(10, 1, true, backend_type) << tanh();
        // nn_ << fc(kFeatures, 100, true, backend_type) << relu() << fc(100, 10, true, backend_type) << tanh() << fc(10, 1, true, backend_type) << tanh();
        // nn_ << fc(kFeatures, 10, true, backend_type) << ll(10);
        // nn_ << fc(kFeatures, 10, true, backend_type) << lrelu() << fc(10, 1, true, backend_type) << tanh() << fc(1, 1, true, backend_type);
        // nn_ << fc(kFeatures, 1, true, backend_type) << tanh() << fc(1, 1, true, backend_type);
        // nn_ << fc(kFeatures, 4, false, backend_type) << relu() << fc(4, 1, false, backend_type);
        // nn_ << fc(kFeatures, 1, true, backend_type) << ll(1, 10);
        // nn_ << fc(kFeatures, 6*6, false, backend_type) << relu() << conv(6, 6, 3, 1, 6) << tanh() << fc(16*6, 1, false, backend_type) << tanh() << fc(1, 1, false, backend_type);

        /*
        nn_ << fc(kFeatures, 32 * 32, true, backend_type) << conv(32, 32, 5, 1, 6,  // C1, 1@32x32-in, 6@28x28-out
                                                                  padding::valid, true, 1, 1, backend_type)
            << tanh() << ave_pool(28, 28, 6, 2)  // S2, 6@28x28-in, 6@14x14-out
            << tanh() << conv(14, 14, 5, 6, 16,  // C3, 6@14x14-in, 16@10x10-out
                                                 // connection_table(tbl, 6, 16),
                              padding::valid, true, 1, 1, backend_type)
            << tanh() << ave_pool(10, 10, 16, 2)  // S4, 16@10x10-in, 16@5x5-out
            << tanh() << conv(5, 5, 5, 16, 120,   // C5, 16@5x5-in, 120@1x1-out
                              padding::valid, true, 1, 1, backend_type)
            << tanh() << fc(120, 10, true, backend_type)  // F6, 120-in, 10-out
            << tanh() << fc(10, 1, true, backend_type);
        */

        // nn_.weight_init(tiny_dnn::weight_init::he(1e-3));
        // nn_.weight_init(tiny_dnn::weight_init::constant(0));
        nn_->weight_init(tiny_dnn::weight_init::uniform(1e-7, 1e-6));
        // nn_->weight_init(tiny_dnn::weight_init::constant(1e-7));
        // nn_.bias_init(tiny_dnn::weight_init::constant(1e-7));
        // nn_.bias_init(tiny_dnn::weight_init::constant(0));
        // nn_.weight_init(tiny_dnn::weight_init::gaussian(0.0000001));
        // nn_->bias_init(tiny_dnn::weight_init::xavier(0.000001));
        // nn_->weight_init(tiny_dnn::weight_init::xavier(0.000001));
        // nn_->weight_init(tiny_dnn::weight_init::xavier());
        // nn_.bias_init(tiny_dnn::weight_init::xavier());
        nn_->init_weight();

        /*
        {
        auto w = nn_[0]->weights();
        auto w0 = *(w[0]);
        w0[kFeatures - kDNNFeatures + FI_LAST] = 1.0;
        }
        */

        /*
        auto w = nn_[0]->weights();
        auto w0 = *(w[0]);
        for (size_t i = 0; i < w0.size(); ++i) {
            if (oneIn(100)) {
                w0[i] = 1.0;
            }
        }
        */

        /*
        {
        auto w = nn_[nn_.layer_size() - 1]->weights();
        ENFORCE_EQ(w.size(), 2);
        auto& w0 = *(w[0]);
        w0[0] = 0.5;
        auto& w1 = *(w[1]);
        w1[0] = 1.0;
        }
        */
    }

    Impl(const Impl& impl) : nn_(make_shared<NN>()) {
        construct();
        stringstream ss;
        ss << *(impl.nn_);
        ss >> *nn_;
    }

    double predict(const DoubleVector& features) {
        return nn_->predict(doubleVectorToVector(features))[0];
    }

    void scale(double alpha, double value, size_t samples) {
        if (value == 0.0) {
            return;
        }

        const double regMul = 1.0 - alpha * value;
        LOG_EVERY_MS(INFO, 1000) << OUT(regMul) << OUT(alpha);
        for (size_t i = 0; i < nn_->layer_size(); ++i) {
            auto weights = (*nn_)[i]->weights();
            for (auto& pv: weights) {
                for (auto& x: *pv) {
                    x *= regMul;
                    /*
                    if (abs(x) < 0.01) {
                        x = 0;
                    }
                    */
                }
            }
        }
    }

    void save(const std::string& filename) {
        nn_->save(filename);
    }

    void saveJson(const std::string& filename) {
        auto json = nn_->to_json(tiny_dnn::content_type::weights_and_model);
        OFStream ofs(filename);
        ofs << json;
    }

    using NN = tiny_dnn::network<tiny_dnn::graph>;

    NN& getNN() { return *nn_; }

   private:
    shared_ptr<NN> nn_;
    vector<shared_ptr<tiny_dnn::layer>> layers_;
};

DNNModel::DNNModel() : impl_(make_unique<DNNModel::Impl>()) {}

DNNModel::DNNModel(const DNNModel::Impl& impl) : impl_(make_unique<DNNModel::Impl>(impl)) {}

DNNModel::~DNNModel() {}

double DNNModel::predict(const DoubleVector& features) { return impl_->predict(features); }

void DNNModel::save(const std::string& filename) { impl_->save(filename); }

void DNNModel::saveJson(const std::string& filename) { impl_->saveJson(filename); }

class DNNModelTrainer::Impl {
   public:
    Impl(double learningRate, double scaleRate, size_t samples) : samples_(samples), iteration_(0) {
        optimizer_.alpha *= learningRate;
        optimizer_.mu = 1.0 - 0.00005 * (1.0 - optimizer_.mu);
        LOG(INFO) << OUT(optimizer_.alpha) << OUT(1.0 - optimizer_.mu);
        alpha0_ = optimizer_.alpha;
        scaleRate_ = scaleRate;
    }

    PDNNModel getModel() { return make_shared<DNNModel>(model_); }

    void slowdown() {
        model_.scale(optimizer_.alpha, scaleRate_, samples_);
        ++iteration_;
        optimizer_.alpha = (iteration_ <= 5) ? alpha0_ : alpha0_ / (1.0 + 0.1 * (iteration_ - 5));
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
        ENFORCE(model_.getNN().fit<tiny_dnn::mse>(optimizer_, vInput, output, min<size_t>(features.size(), 16), 1, tiny_dnn::nop, tiny_dnn::nop));
    }

   private:
    DNNModel::Impl model_;
    tiny_dnn::rms_prop optimizer_;
    double scaleRate_;
    size_t samples_;
    double alpha0_;
    size_t iteration_;
};

DNNModelTrainer::DNNModelTrainer(double learningRate, double scaleRate, size_t samples)
    : impl_(make_unique<DNNModelTrainer::Impl>(learningRate, scaleRate, samples)) {
}

DNNModelTrainer::~DNNModelTrainer() {}

PDNNModel DNNModelTrainer::getModel() { return impl_->getModel(); }

void DNNModelTrainer::train(const DoubleVector& features, double label) { impl_->train(features, label); }

void DNNModelTrainer::train(const vector<DoubleVector>& features, const DoubleVector& label) {
    impl_->train(features, label);
}

void DNNModelTrainer::slowdown() { impl_->slowdown(); }


