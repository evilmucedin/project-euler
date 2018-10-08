#include "lib/header.h"

#include "lib/init.h"
#include "lib/io/csv.h"
#include "lib/io/fstream.h"
#include "lib/io/zstream.h"

#include "glog/logging.h"

struct Row {
    int label_;
    FloatVector features_;
};
using Rows = vector<Row>;

struct Data {
    Rows rows;
};

Rows readRows(const std::string& filename) {
    auto fstream = std::make_shared<IFStream>(filename);
    auto zfstream = std::make_shared<ZIStream>(fstream);
    auto csv = std::make_shared<CsvParser>(zfstream);
    ALWAYS_ASSERT(csv->readHeader());
    Rows rows;
    auto labelIndex = csv->getIndexOrDie("label");
    std::vector<int> pixelIndices;
    for (size_t i = 0; i < 784; ++i) {
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
    return rows;
}

int main(int argc, char* argv[]) {
    standardInit(argc, argv);
    LOG(INFO) << OUT(readRows("train.gz").size());
    return 0;
}
