#include "lib/boosting/model.h"

#include <cmath>
#include <fstream>
#include <istream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace boosting {

namespace {

// Model file layout (little-endian, fixed-width fields):
//   u32 magic 'CBM1', u32 version,
//   u32 loss, u32 featureCount, f64 bias, u32 treeCount,
//   per tree: u32 depth, depth * (u32 featureIndex, f32 border),
//             (1 << depth) * f64 leafValue.
constexpr uint32_t kMagic = 0x314D4243;  // "CBM1"
constexpr uint32_t kVersion = 1;
constexpr uint32_t kMaxTreeDepth = 31;

template <typename T>
void writePod(std::ostream& out, T value) {
    out.write(reinterpret_cast<const char*>(&value), sizeof(value));
}

template <typename T>
T readPod(std::istream& in) {
    T value{};
    in.read(reinterpret_cast<char*>(&value), sizeof(value));
    if (!in) {
        throw std::runtime_error("boosting::TModel::load: unexpected end of stream");
    }
    return value;
}

}  // namespace

TModel::TModel(uint32_t featureCount, ELoss loss, double bias, std::vector<TObliviousTree> trees)
    : featureCount_(featureCount), loss_(loss), bias_(bias), trees_(std::move(trees)) {
}

double TModel::apply(const float* features) const {
    double result = bias_;
    for (const TObliviousTree& tree : trees_) {
        result += tree.apply(features);
    }
    return result;
}

double TModel::apply(const std::vector<float>& features) const {
    if (features.size() < featureCount_) {
        throw std::runtime_error("boosting::TModel::apply: feature vector is too short");
    }
    return apply(features.data());
}

double TModel::applyProbability(const float* features) const {
    return sigmoid(apply(features));
}

double TModel::applyProbability(const std::vector<float>& features) const {
    return sigmoid(apply(features));
}

std::vector<double> TModel::applyBatch(const std::vector<std::vector<float>>& rows) const {
    std::vector<double> result;
    result.reserve(rows.size());
    for (const auto& row : rows) {
        result.push_back(apply(row));
    }
    return result;
}

void TModel::save(std::ostream& out) const {
    writePod(out, kMagic);
    writePod(out, kVersion);
    writePod(out, static_cast<uint32_t>(loss_));
    writePod(out, featureCount_);
    writePod(out, bias_);
    writePod(out, static_cast<uint32_t>(trees_.size()));
    for (const TObliviousTree& tree : trees_) {
        writePod(out, static_cast<uint32_t>(tree.splits.size()));
        for (const TSplit& split : tree.splits) {
            writePod(out, split.featureIndex);
            writePod(out, split.border);
        }
        for (double value : tree.leafValues) {
            writePod(out, value);
        }
    }
    if (!out) {
        throw std::runtime_error("boosting::TModel::save: write failed");
    }
}

void TModel::save(const std::string& path) const {
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (!out) {
        throw std::runtime_error("boosting::TModel::save: cannot open " + path);
    }
    save(static_cast<std::ostream&>(out));
}

TModel TModel::load(std::istream& in) {
    if (readPod<uint32_t>(in) != kMagic) {
        throw std::runtime_error("boosting::TModel::load: bad magic, not a boosting model");
    }
    const uint32_t version = readPod<uint32_t>(in);
    if (version != kVersion) {
        throw std::runtime_error("boosting::TModel::load: unsupported version " + std::to_string(version));
    }
    const uint32_t lossRaw = readPod<uint32_t>(in);
    if (lossRaw > static_cast<uint32_t>(ELoss::LogLoss)) {
        throw std::runtime_error("boosting::TModel::load: unknown loss function");
    }
    const uint32_t featureCount = readPod<uint32_t>(in);
    const double bias = readPod<double>(in);
    const uint32_t treeCount = readPod<uint32_t>(in);

    std::vector<TObliviousTree> trees(treeCount);
    for (TObliviousTree& tree : trees) {
        const uint32_t depth = readPod<uint32_t>(in);
        if (depth > kMaxTreeDepth) {
            throw std::runtime_error("boosting::TModel::load: corrupted tree depth");
        }
        tree.splits.resize(depth);
        for (TSplit& split : tree.splits) {
            split.featureIndex = readPod<uint32_t>(in);
            split.border = readPod<float>(in);
            if (split.featureIndex >= featureCount) {
                throw std::runtime_error("boosting::TModel::load: split feature index out of range");
            }
        }
        tree.leafValues.resize(size_t(1) << depth);
        for (double& value : tree.leafValues) {
            value = readPod<double>(in);
        }
    }
    return TModel(featureCount, static_cast<ELoss>(lossRaw), bias, std::move(trees));
}

TModel TModel::load(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        throw std::runtime_error("boosting::TModel::load: cannot open " + path);
    }
    return load(static_cast<std::istream&>(in));
}

double sigmoid(double x) {
    return 1.0 / (1.0 + std::exp(-x));
}

}  // namespace boosting
