#include "lib/boosting/trainer.h"

#include <algorithm>
#include <cmath>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

namespace boosting {

namespace {

struct TGradients {
    std::vector<double> grad;
    std::vector<double> hess;
};

// First and second derivatives of the loss with respect to the raw score.
void computeGradients(ELoss loss, const std::vector<float>& targets, const std::vector<double>& approx,
                      TGradients& out) {
    const size_t rowCount = targets.size();
    out.grad.resize(rowCount);
    out.hess.resize(rowCount);
    switch (loss) {
        case ELoss::RMSE:
            for (size_t i = 0; i < rowCount; ++i) {
                out.grad[i] = approx[i] - targets[i];
                out.hess[i] = 1.0;
            }
            break;
        case ELoss::LogLoss:
            for (size_t i = 0; i < rowCount; ++i) {
                const double p = sigmoid(approx[i]);
                out.grad[i] = p - targets[i];
                out.hess[i] = std::max(p * (1.0 - p), 1e-16);
            }
            break;
    }
}

double startingBias(ELoss loss, const std::vector<float>& targets) {
    double sum = 0.0;
    for (float target : targets) {
        sum += target;
    }
    const double mean = targets.empty() ? 0.0 : sum / targets.size();
    if (loss == ELoss::RMSE) {
        return mean;
    }
    const double p = std::min(std::max(mean, 1e-6), 1.0 - 1e-6);
    return std::log(p / (1.0 - p));
}

// Split of the quantized data: bins[feature][row] > binIndex sends the row
// into the "right" (high bit) child.
struct TBinSplit {
    uint32_t feature = 0;
    uint32_t binIndex = 0;
};

struct TLevelStats {
    std::vector<double> sumGrad;
    std::vector<double> sumHess;
};

double leafScore(double gradSum, double hessSum, double l2Reg) {
    return gradSum * gradSum / (hessSum + l2Reg);
}

}  // namespace

TTrainer::TTrainer(const TTrainParams& params) : params_(params) {
    if (params.iterations == 0) {
        throw std::runtime_error("boosting::TTrainer: iterations must be positive");
    }
    if (params.depth == 0 || params.depth > 16) {
        throw std::runtime_error("boosting::TTrainer: depth must be in [1, 16]");
    }
    if (params.learningRate <= 0.0) {
        throw std::runtime_error("boosting::TTrainer: learningRate must be positive");
    }
    if (params.l2LeafReg < 0.0) {
        throw std::runtime_error("boosting::TTrainer: l2LeafReg must be non-negative");
    }
    if (params.subsample <= 0.0 || params.subsample > 1.0) {
        throw std::runtime_error("boosting::TTrainer: subsample must be in (0, 1]");
    }
}

TModel TTrainer::train(const TDataset& dataset) const {
    const size_t rowCount = dataset.rowCount();
    const size_t featureCount = dataset.featureCount();
    if (rowCount == 0) {
        throw std::runtime_error("boosting::TTrainer::train: empty dataset");
    }
    if (params_.loss == ELoss::LogLoss) {
        for (float target : dataset.targets()) {
            if (target != 0.0f && target != 1.0f) {
                throw std::runtime_error("boosting::TTrainer::train: LogLoss targets must be 0 or 1");
            }
        }
    }

    const TQuantizedDataset quantized = quantize(dataset, params_.borderCount);

    const double bias = startingBias(params_.loss, dataset.targets());
    std::vector<double> approx(rowCount, bias);
    std::vector<TObliviousTree> trees;
    trees.reserve(params_.iterations);

    std::mt19937 rng(params_.seed);
    std::bernoulli_distribution sampleRow(params_.subsample);
    std::vector<char> sampled(rowCount, 1);

    TGradients gradients;
    std::vector<uint32_t> leafOf(rowCount);
    TLevelStats stats;

    for (uint32_t iteration = 0; iteration < params_.iterations; ++iteration) {
        computeGradients(params_.loss, dataset.targets(), approx, gradients);

        if (params_.subsample < 1.0) {
            for (size_t i = 0; i < rowCount; ++i) {
                sampled[i] = sampleRow(rng) ? 1 : 0;
            }
        }

        std::fill(leafOf.begin(), leafOf.end(), 0);
        std::vector<TBinSplit> treeSplits;

        for (uint32_t level = 0; level < params_.depth; ++level) {
            const size_t leafCount = size_t(1) << level;

            // Score of the current partition; a split must beat it to be taken.
            stats.sumGrad.assign(leafCount, 0.0);
            stats.sumHess.assign(leafCount, 0.0);
            for (size_t i = 0; i < rowCount; ++i) {
                if (!sampled[i]) {
                    continue;
                }
                stats.sumGrad[leafOf[i]] += gradients.grad[i];
                stats.sumHess[leafOf[i]] += gradients.hess[i];
            }
            double currentScore = 0.0;
            for (size_t leaf = 0; leaf < leafCount; ++leaf) {
                currentScore += leafScore(stats.sumGrad[leaf], stats.sumHess[leaf], params_.l2LeafReg);
            }

            double bestScore = currentScore + 1e-9;
            TBinSplit bestSplit;
            bool found = false;

            std::vector<double> histGrad;
            std::vector<double> histHess;
            for (uint32_t feature = 0; feature < featureCount; ++feature) {
                const std::vector<float>& borders = quantized.borders[feature];
                if (borders.empty()) {
                    continue;  // constant feature
                }
                const size_t binCount = borders.size() + 1;
                const std::vector<uint8_t>& bins = quantized.bins[feature];

                histGrad.assign(leafCount * binCount, 0.0);
                histHess.assign(leafCount * binCount, 0.0);
                for (size_t i = 0; i < rowCount; ++i) {
                    if (!sampled[i]) {
                        continue;
                    }
                    const size_t cell = leafOf[i] * binCount + bins[i];
                    histGrad[cell] += gradients.grad[i];
                    histHess[cell] += gradients.hess[i];
                }

                // Sweep borders left to right, accumulating per-leaf prefix
                // sums; rows with bin > binIndex go right.
                std::vector<double> leftGrad(leafCount, 0.0);
                std::vector<double> leftHess(leafCount, 0.0);
                for (uint32_t binIndex = 0; binIndex + 1 < binCount; ++binIndex) {
                    double score = 0.0;
                    for (size_t leaf = 0; leaf < leafCount; ++leaf) {
                        leftGrad[leaf] += histGrad[leaf * binCount + binIndex];
                        leftHess[leaf] += histHess[leaf * binCount + binIndex];
                        score += leafScore(leftGrad[leaf], leftHess[leaf], params_.l2LeafReg);
                        score += leafScore(stats.sumGrad[leaf] - leftGrad[leaf],
                                           stats.sumHess[leaf] - leftHess[leaf], params_.l2LeafReg);
                    }
                    if (score > bestScore) {
                        bestScore = score;
                        bestSplit = {feature, binIndex};
                        found = true;
                    }
                }
            }

            if (!found) {
                break;  // no split improves the loss, stop growing this tree
            }

            treeSplits.push_back(bestSplit);
            const std::vector<uint8_t>& bins = quantized.bins[bestSplit.feature];
            for (size_t i = 0; i < rowCount; ++i) {
                leafOf[i] |= static_cast<uint32_t>(bins[i] > bestSplit.binIndex) << level;
            }
        }

        // Newton leaf estimates over the sampled rows: -G / (H + l2).
        const size_t leafCount = size_t(1) << treeSplits.size();
        stats.sumGrad.assign(leafCount, 0.0);
        stats.sumHess.assign(leafCount, 0.0);
        for (size_t i = 0; i < rowCount; ++i) {
            if (!sampled[i]) {
                continue;
            }
            stats.sumGrad[leafOf[i]] += gradients.grad[i];
            stats.sumHess[leafOf[i]] += gradients.hess[i];
        }

        TObliviousTree tree;
        tree.splits.reserve(treeSplits.size());
        for (const TBinSplit& split : treeSplits) {
            tree.splits.push_back({split.feature, quantized.borders[split.feature][split.binIndex]});
        }
        tree.leafValues.resize(leafCount);
        for (size_t leaf = 0; leaf < leafCount; ++leaf) {
            tree.leafValues[leaf] =
                -params_.learningRate * stats.sumGrad[leaf] / (stats.sumHess[leaf] + params_.l2LeafReg);
        }

        // Update predictions for every row (sampled or not) via bin splits so
        // training stays consistent with the stored float borders.
        for (size_t i = 0; i < rowCount; ++i) {
            approx[i] += tree.leafValues[leafOf[i]];
        }

        trees.push_back(std::move(tree));
    }

    return TModel(static_cast<uint32_t>(featureCount), params_.loss, bias, std::move(trees));
}

}  // namespace boosting
