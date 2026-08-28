#include "lib/boosting/dataset.h"

#include <algorithm>
#include <stdexcept>

namespace boosting {

TDataset::TDataset(size_t featureCount) : featureCount_(featureCount) {
    if (featureCount == 0) {
        throw std::runtime_error("boosting::TDataset: featureCount must be positive");
    }
}

void TDataset::addRow(const std::vector<float>& features, float target) {
    if (features.size() != featureCount_) {
        throw std::runtime_error("boosting::TDataset::addRow: wrong feature count");
    }
    features_.insert(features_.end(), features.begin(), features.end());
    targets_.push_back(target);
}

TQuantizedDataset quantize(const TDataset& dataset, uint32_t borderCount) {
    if (borderCount == 0 || borderCount > 255) {
        throw std::runtime_error("boosting::quantize: borderCount must be in [1, 255]");
    }
    const size_t rowCount = dataset.rowCount();
    const size_t featureCount = dataset.featureCount();

    TQuantizedDataset result;
    result.featureCount = featureCount;
    result.rowCount = rowCount;
    result.borders.resize(featureCount);
    result.bins.resize(featureCount);

    std::vector<float> values(rowCount);
    for (size_t feature = 0; feature < featureCount; ++feature) {
        for (size_t rowIndex = 0; rowIndex < rowCount; ++rowIndex) {
            values[rowIndex] = dataset.row(rowIndex)[feature];
        }
        std::sort(values.begin(), values.end());

        // A border can only go between two distinct adjacent sorted values;
        // pick them at evenly spaced quantile positions.
        std::vector<float>& borders = result.borders[feature];
        for (uint32_t candidate = 1; candidate <= borderCount; ++candidate) {
            const size_t position = candidate * rowCount / (borderCount + 1);
            if (position == 0 || position >= rowCount) {
                continue;
            }
            const float left = values[position - 1];
            const float right = values[position];
            if (left < right) {
                const float border = left + (right - left) / 2;
                if (borders.empty() || borders.back() < border) {
                    borders.push_back(border);
                }
            }
        }

        std::vector<uint8_t>& bins = result.bins[feature];
        bins.resize(rowCount);
        for (size_t rowIndex = 0; rowIndex < rowCount; ++rowIndex) {
            const float value = dataset.row(rowIndex)[feature];
            const auto it = std::lower_bound(borders.begin(), borders.end(), value);
            bins[rowIndex] = static_cast<uint8_t>(it - borders.begin());
        }
    }
    return result;
}

}  // namespace boosting
