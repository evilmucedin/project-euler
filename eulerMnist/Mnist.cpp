#include "lib/header.h"

#include "glog/logging.h"

#include "lib/init.h"
#include "lib/io/csv.h"
#include "lib/io/fstream.h"
#include "lib/io/zstream.h"
#include "lib/matrix.h"

struct Row {
    int label_;
    FloatVector features_;
};
using Rows = vector<Row>;

struct Data {
    Rows rows;
};

static constexpr size_t N_FEATURES = 784;

Data readRows(const std::string& filename) {
    auto fstream = std::make_shared<IFStream>(filename);
    auto zfstream = std::make_shared<ZIStream>(fstream);
    auto csv = std::make_shared<CsvParser>(zfstream);
    ALWAYS_ASSERT(csv->readHeader());
    Rows rows;
    auto labelIndex = csv->getIndexOrDie("label");
    std::vector<int> pixelIndices;
    for (size_t i = 0; i < N_FEATURES; ++i) {
        pixelIndices.emplace_back(csv->getIndexOrDie("pixel" + std::to_string(i)));
    }
    while (csv->readLine()) {
        Row row;
        row.label_ = csv->getInt(labelIndex);
        row.features_.resize(pixelIndices.size());
        for (size_t iPixel = 0; iPixel < pixelIndices.size(); ++iPixel) {
            row.features_[iPixel] = csv->getFloat(pixelIndices[iPixel]);
        }
        rows.emplace_back(std::move(row));
    }
    return {rows};
}

class LinearRegressionClassifier {
   public:
    LinearRegressionClassifier() : xtx_(N_FEATURES), xty_((u32)N_FEATURES) {}

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

   private:
    Matrix xtx_;
    Vector xty_;
    Matrix inv_;
    Vector bPrime_;
};

template <typename T>
vector<T> train(const Data& data) {
    vector<T> result(10);
    for (const auto& row : data.rows) {
        for (size_t i = 0; i < 10; ++i) {
            result[i].addSample(row.label_ == i, row.features_);
        }
    }
    for (auto& c : result) {
        c.train();
    }
    return result;
}

int main(int argc, char* argv[]) {
    standardInit(argc, argv);
    auto trainData = readRows("train.gz");
    LOG(INFO) << OUT(trainData.rows.size());
    auto linear = train<LinearRegressionClassifier>(trainData);
    return 0;
}
