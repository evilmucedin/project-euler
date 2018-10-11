#include "lib/header.h"

#include "glog/logging.h"

#include "lib/init.h"
#include "lib/io/csv.h"
#include "lib/io/fstream.h"
#include "lib/io/zstream.h"
#include "lib/matrix.h"
#include "lib/timer.h"

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
    LinearRegressionClassifier() : xtx_(N_FEATURES), xty_(N_FEATURES), inv_(N_FEATURES), bPrime_(N_FEATURES) {}

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
        inv_ = xtx_.invert();
        bPrime_ = inv_ * xty_;
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
    Matrix inv_;
    Vector bPrime_;
};

template <typename T>
vector<T> train(const Data& data) {
    Timer tTrain(std::string("Train ") + typeid(T).name());
    vector<T> result(10);
    size_t index = 0;
    for (const auto& row : data.rows) {
        for (int i = 0; i < 10; ++i) {
            result[i].addSample(row.label_ == i, row.features_);
        }
        LOG_EVERY_MS(INFO, 5000) << "Processing " << index;
        ++index;
    }
    for (auto& c : result) {
        c.train();
    }
    return result;
}

template <typename T>
void test(const vector<T>& classifiers, const std::string& filename) {
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
        auto linear = train<LinearRegressionClassifier>(trainData);
        test(linear, "linear.csv");
    }
    return 0;
}
