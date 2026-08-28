#pragma once

// Training-time data containers. Raw float rows are quantized once into
// per-feature bins over quantile borders (CatBoost-style feature
// binarization), and all split search during training happens on the bins.

#include <cstddef>
#include <cstdint>
#include <vector>

namespace boosting {

class TDataset {
   public:
    explicit TDataset(size_t featureCount);

    void addRow(const std::vector<float>& features, float target);

    size_t featureCount() const {
        return featureCount_;
    }
    size_t rowCount() const {
        return targets_.size();
    }
    const float* row(size_t index) const {
        return features_.data() + index * featureCount_;
    }
    float target(size_t index) const {
        return targets_[index];
    }
    const std::vector<float>& targets() const {
        return targets_;
    }

   private:
    size_t featureCount_;
    std::vector<float> features_;  // row-major, rowCount x featureCount
    std::vector<float> targets_;
};

// Bin values are counts of borders below the raw value, so the float split
// "x > borders[j]" is exactly the bin split "bin > j".
struct TQuantizedDataset {
    size_t featureCount = 0;
    size_t rowCount = 0;
    std::vector<std::vector<float>> borders;  // per feature, strictly ascending
    std::vector<std::vector<uint8_t>> bins;   // per feature, one bin per row
};

// borderCount is the maximum number of borders per feature and must fit a
// uint8_t bin, i.e. be at most 255. Borders are taken at quantiles of the
// observed values, midway between adjacent distinct values.
TQuantizedDataset quantize(const TDataset& dataset, uint32_t borderCount);

}  // namespace boosting
