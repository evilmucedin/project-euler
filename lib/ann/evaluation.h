#pragma once

// Quality metrics for comparing an approximate result against ground truth.

#include "lib/ann/common.h"

#include <algorithm>
#include <vector>

namespace ann {

// Fraction of `exact` ids present in `approx`. An empty `exact` scores 1.
inline double recall(const std::vector<Neighbor>& approx, const std::vector<Neighbor>& exact) {
    if (exact.empty()) {
        return 1.0;
    }
    std::vector<Id> found;
    found.reserve(approx.size());
    for (const Neighbor& n : approx) {
        found.push_back(n.id);
    }
    std::sort(found.begin(), found.end());
    std::size_t hits = 0;
    for (const Neighbor& n : exact) {
        hits += std::binary_search(found.begin(), found.end(), n.id);
    }
    return static_cast<double>(hits) / static_cast<double>(exact.size());
}

// Mean recall over a batch of queries; the two vectors must be aligned by query.
inline double meanRecall(const std::vector<std::vector<Neighbor>>& approx,
                         const std::vector<std::vector<Neighbor>>& exact) {
    if (exact.empty()) {
        return 1.0;
    }
    double total = 0.0;
    for (std::size_t i = 0; i < exact.size(); ++i) {
        total += recall(i < approx.size() ? approx[i] : std::vector<Neighbor>{}, exact[i]);
    }
    return total / static_cast<double>(exact.size());
}

}  // namespace ann
