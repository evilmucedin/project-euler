#include "glog/logging.h"

#include "lib/header.h"
#include "lib/init.h"
#include "lib/io/csv.h"
#include "lib/io/fstream.h"
#include "lib/io/zstream.h"
#include "lib/matrix.h"
#include "lib/thread-pool/threadPool.h"
#include "lib/timer.h"
#include "tiny_dnn/tiny_dnn.h"

struct Row {
    int label_;
    FloatVector features_;
};
using Rows = vector<Row>;

struct Data {
    Rows rows;
};

static constexpr size_t N_FEATURES = 784;

Data readRows(const std::string& filename, bool label) {
    auto fstream = std::make_shared<IFStream>(filename);
    auto zfstream = std::make_shared<ZIStream>(fstream);
    auto csv = std::make_shared<CsvParser>(zfstream);
    ALWAYS_ASSERT(csv->readHeader());
    Rows rows;
    int labelIndex = -1;
    if (label) {
        labelIndex = csv->getIndexOrDie("label");
    }
    std::vector<int> pixelIndices;
    for (size_t i = 0; i < N_FEATURES; ++i) {
        pixelIndices.emplace_back(csv->getIndexOrDie("pixel" + std::to_string(i)));
    }
    while (csv->readLine()) {
        Row row;
        if (label) {
            row.label_ = csv->getInt(labelIndex);
        }
        row.features_.resize(pixelIndices.size());
        for (size_t iPixel = 0; iPixel < pixelIndices.size(); ++iPixel) {
            row.features_[iPixel] = log(csv->getFloat(pixelIndices[iPixel]) + 1.f);
        }
        rows.emplace_back(std::move(row));
    }
    return {rows};
}

class LinearRegressionClassifier {
   public:
    LinearRegressionClassifier() : xtx_(N_FEATURES), xty_(N_FEATURES), bPrime_(N_FEATURES) {}

    void addSample(float y, const FloatVector& features) {
        assert(features.size() == N_FEATURES);
        for (size_t i = 0; i < features.size(); ++i) {
            for (size_t j = 0; j < features.size(); ++j) {
                xtx_.data_[i][j] += features[i] * features[j];
            }
        }
        for (size_t i = 0; i < features.size(); ++i) {
            xty_[i] += features[i] * y;
        }

    }

    void train() {
        auto inv = xtx_.invert();
        bPrime_ = inv * xty_;
    }

    float classify(const Row& row) const {
        float result = 0;
        for (size_t i = 0; i < N_FEATURES; ++i) {
            result += bPrime_[i] * row.features_[i];
        }
        return result;
    }

   private:
    Matrix xtx_;
    Vector xty_;
    Vector bPrime_;
};

class DNNRegressionClassifier {
   public:
    DNNRegressionClassifier() {
        const auto backend_type = tiny_dnn::core::backend_t::avx;
        nn_ << tiny_dnn::convolutional_layer(32, 32, 5, 1, 6, tiny_dnn::padding::valid, true, 1, 1, backend_type)
            << tiny_dnn::tanh_layer(28, 28, 6) << tiny_dnn::average_pooling_layer(28, 28, 6, 2)
            << tiny_dnn::tanh_layer(14, 14, 6)
            << tiny_dnn::convolutional_layer(14, 14, 5, 6, 16, tiny_dnn::padding::valid, true, 1, 1, backend_type)
            << tiny_dnn::tanh_layer(10, 10, 16) << tiny_dnn::average_pooling_layer(10, 10, 16, 2)
            << tiny_dnn::tanh_layer(5, 5, 16)
            << tiny_dnn::convolutional_layer(5, 5, 5, 16, 120, tiny_dnn::padding::valid, true, 1, 1, backend_type)
            << tiny_dnn::tanh_layer(1, 1, 120) << tiny_dnn::fully_connected_layer(120, 1, true, backend_type)
            << tiny_dnn::tanh_layer(1);
    }

    tiny_dnn::vec_t floatVectorToVector(const FloatVector& features) {
        tiny_dnn::vec_t fFeatures(32 * 32);
        for (size_t i = 0; i < 32; ++i) {
            if (i >= 2 && i < 30) {
                for (size_t j = 0; j < 32; ++j) {
                    if (j >= 2 && j < 30) {
                        fFeatures[i * 32 + j] = features[(i - 2) * 28 + (j - 2)];
                    }
                }
            }
        }
        return fFeatures;
    }

    tiny_dnn::tensor_t floatVectorToTensor(const FloatVector& features) {
        tiny_dnn::tensor_t input;
        input.emplace_back(floatVectorToVector(features));
        return input;
    }

    void addSample(float y, const FloatVector& features) {
        features_.emplace_back(floatVectorToTensor(features));
        y_.emplace_back(tiny_dnn::vec_t(1, y));
    }

    void train() {
        for (size_t i = 0; i < 10; ++i) {
            Timer t(std::string("Train DNN it ") + to_string(i));
            optimizer_.alpha *= 0.8;
            nn_.fit<tiny_dnn::mse>(optimizer_, features_, y_);
            LOG(INFO) << "Loss on " << i << " = " << nn_.get_loss_b<tiny_dnn::mse>(features_, y_);
        }
    }

    float classify(const Row& row) { return nn_.predict(row.features_)[0]; }

   private:
    tiny_dnn::network<tiny_dnn::sequential> nn_;
    tiny_dnn::adagrad optimizer_;
    std::vector<tiny_dnn::tensor_t> features_;
    std::vector<tiny_dnn::vec_t> y_;
};

template <typename T>
vector<T> train(const Data& data) {
    Timer tTrain(std::string("Train ") + typeid(T).name());
    vector<T> result(10);

    auto train_i = [&](int index) {
        for (const auto& row : data.rows) {
            result[index].addSample(row.label_ == index, row.features_);
        }
        result[index].train();
        LOG_EVERY_MS(INFO, 5000) << "Processing " << index;
    };

    tp::ThreadPool p;
    std::vector<int> indices;
    for (size_t i = 0; i < 10; ++i) {
        indices.emplace_back(i);
    }
    p.run(train_i, indices.begin(), indices.end());

    return result;
}

template <typename T>
void test(vector<T>& classifiers, const std::string& filename) {
    Timer tTrain(std::string("Test ") + typeid(T).name());
    OFStream fOut(filename);
    fOut << "ImageId,Label" << endl;
    auto test = readRows("test.gz", false);
    for (size_t i = 0; i < test.rows.size(); ++i) {
        float max = -100.0;
        int maxIndex = -1;
        for (size_t j = 0; j < 10; ++j) {
            auto prob = classifiers[j].classify(test.rows[i]);
            if (prob > max) {
                max = prob;
                maxIndex = j;
            }
        }
        fOut << (i + 1) << "," << maxIndex << endl;
    }
}

int main(int argc, char* argv[]) {
    standardInit(argc, argv);
    auto trainData = readRows("train.gz", true);
    LOG(INFO) << OUT(trainData.rows.size());
    {
        auto dnn = train<DNNRegressionClassifier>(trainData);
        test(dnn, "dnn.csv");
    }
    {
        auto linear = train<LinearRegressionClassifier>(trainData);
        test(linear, "linear.csv");
    }
    return 0;
}
