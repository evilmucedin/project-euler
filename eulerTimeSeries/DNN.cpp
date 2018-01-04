#include "DNN.h"

#include "tiny_dnn/tiny_dnn.h"

class DNNModel::Impl {
   public:
    Impl() {
        // construct nets
        //
        // C : convolution
        // S : sub-sampling
        // F : fully connected
        using fc = tiny_dnn::layers::fc;
        using conv = tiny_dnn::layers::conv;
        using ave_pool = tiny_dnn::layers::ave_pool;
        using tanh = tiny_dnn::activation::tanh;

        using tiny_dnn::core::connection_table;
        using padding = tiny_dnn::padding;

        const auto backend_type = tiny_dnn::core::backend_t::internal;

        nn_ << conv(100, 3, 5, 1, 6,  // C1, 1@100x3-in, 6@28x28-out
                    padding::valid, true, 1, 1, backend_type)
            << tanh() << ave_pool(28, 28, 6, 2)  // S2, 6@28x28-in, 6@14x14-out
            << tanh() << conv(14, 14, 5, 6, 16,  // C3, 6@14x14-in, 16@10x10-out
                                                 // connection_table(tbl, 6, 16),
                              padding::valid, true, 1, 1, backend_type)
            << tanh() << ave_pool(10, 10, 16, 2)  // S4, 16@10x10-in, 16@5x5-out
            << tanh() << conv(5, 5, 5, 16, 120,   // C5, 16@5x5-in, 120@1x1-out
                              padding::valid, true, 1, 1, backend_type)
            << tanh() << fc(120, 1, true, backend_type)  // F6, 120-in, 1-out
            << tanh();
    }

   private:
    using NN = tiny_dnn::network<tiny_dnn::sequential>;
    NN nn_;
};

DNNModel::DNNModel() : impl_(make_unique<DNNModel::Impl>()) {}

class DNNModelTrainer::Impl {
   public:
    Impl() : model_(make_shared<DNNModel>()) {}

    PDNNModel getModel() { return model_; }

   private:
    PDNNModel model_;
};

DNNModelTrainer::DNNModelTrainer() : impl_(make_unique<DNNModelTrainer::Impl>()) {}

PDNNModel DNNModelTrainer::getModel() { return impl_->getModel(); }
