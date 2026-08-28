#pragma once

// Gradient-boosting model built from oblivious (symmetric) decision trees, the
// tree structure CatBoost uses. This header is the only one needed at serving
// time: a loaded model is a flat, pointer-free structure that scores a feature
// vector with `depth` comparisons per tree and no allocation.

#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <string>
#include <vector>

namespace boosting {

enum class ELoss : uint32_t {
    RMSE = 0,
    LogLoss = 1,
};

// One split condition: features[featureIndex] > border.
struct TSplit {
    uint32_t featureIndex = 0;
    float border = 0.0f;
};

// Oblivious tree: every node on a level tests the same split, so a tree of
// depth D is D splits plus 1 << D leaf values, and the leaf of a sample is
// the D-bit mask of its split outcomes.
struct TObliviousTree {
    std::vector<TSplit> splits;
    std::vector<double> leafValues;

    size_t depth() const {
        return splits.size();
    }

    size_t leafIndex(const float* features) const {
        size_t index = 0;
        for (size_t level = 0; level < splits.size(); ++level) {
            const TSplit& split = splits[level];
            index |= static_cast<size_t>(features[split.featureIndex] > split.border) << level;
        }
        return index;
    }

    double apply(const float* features) const {
        return leafValues[leafIndex(features)];
    }
};

class TModel {
   public:
    TModel() = default;
    TModel(uint32_t featureCount, ELoss loss, double bias, std::vector<TObliviousTree> trees);

    // Raw score: bias plus the sum of tree leaf values. `features` must hold
    // at least featureCount() values.
    double apply(const float* features) const;
    double apply(const std::vector<float>& features) const;

    // Probability of class 1 for a LogLoss model (sigmoid of the raw score).
    double applyProbability(const float* features) const;
    double applyProbability(const std::vector<float>& features) const;

    std::vector<double> applyBatch(const std::vector<std::vector<float>>& rows) const;

    uint32_t featureCount() const {
        return featureCount_;
    }
    ELoss loss() const {
        return loss_;
    }
    double bias() const {
        return bias_;
    }
    const std::vector<TObliviousTree>& trees() const {
        return trees_;
    }

    void save(std::ostream& out) const;
    void save(const std::string& path) const;
    static TModel load(std::istream& in);
    static TModel load(const std::string& path);

   private:
    uint32_t featureCount_ = 0;
    ELoss loss_ = ELoss::RMSE;
    double bias_ = 0.0;
    std::vector<TObliviousTree> trees_;
};

double sigmoid(double x);

}  // namespace boosting
