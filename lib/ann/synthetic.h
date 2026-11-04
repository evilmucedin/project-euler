#pragma once

// Deterministic synthetic datasets for tests and benchmarks.

#include "lib/ann/vectorSet.h"

#include <cstdint>
#include <random>
#include <vector>

namespace ann {

inline VectorSet randomUniform(std::size_t n, std::size_t dim, std::uint64_t seed, float lo = 0.0f, float hi = 1.0f) {
    std::mt19937_64 rng(seed);
    std::uniform_real_distribution<float> dist(lo, hi);
    VectorSet set(dim);
    set.reserve(n);
    std::vector<float> row(dim);
    for (std::size_t i = 0; i < n; ++i) {
        for (float& x : row) {
            x = dist(rng);
        }
        set.add(row.data());
    }
    return set;
}

inline VectorSet randomGaussian(std::size_t n, std::size_t dim, std::uint64_t seed, float mean = 0.0f, float stddev = 1.0f) {
    std::mt19937_64 rng(seed);
    std::normal_distribution<float> dist(mean, stddev);
    VectorSet set(dim);
    set.reserve(n);
    std::vector<float> row(dim);
    for (std::size_t i = 0; i < n; ++i) {
        for (float& x : row) {
            x = dist(rng);
        }
        set.add(row.data());
    }
    return set;
}

// `clusters` Gaussian blobs with centres drawn uniformly from [-10, 10]^dim.
// Row i belongs to cluster i % clusters.
inline VectorSet randomClustered(std::size_t n, std::size_t dim, std::size_t clusters, std::uint64_t seed, float spread = 0.5f) {
    std::mt19937_64 rng(seed);
    std::uniform_real_distribution<float> centre(-10.0f, 10.0f);
    std::normal_distribution<float> noise(0.0f, spread);
    std::vector<std::vector<float>> centres(clusters, std::vector<float>(dim));
    for (auto& c : centres) {
        for (float& x : c) {
            x = centre(rng);
        }
    }
    VectorSet set(dim);
    set.reserve(n);
    std::vector<float> row(dim);
    for (std::size_t i = 0; i < n; ++i) {
        const std::vector<float>& c = centres[i % clusters];
        for (std::size_t d = 0; d < dim; ++d) {
            row[d] = c[d] + noise(rng);
        }
        set.add(row.data());
    }
    return set;
}

}  // namespace ann
