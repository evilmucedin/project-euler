#pragma once

// Lloyd's k-means over squared Euclidean distance, used as the coarse
// quantiser of IvfFlatIndex and handy on its own.

#include "lib/ann/common.h"
#include "lib/ann/distance.h"
#include "lib/ann/vectorSet.h"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

namespace ann {

struct KMeansParams {
    std::size_t iterations = 20;
    std::uint64_t seed = 42;
};

struct KMeansResult {
    VectorSet centroids;
    std::vector<std::uint32_t> assignment;  // cluster of every input row
    std::size_t iterationsRun = 0;
    double inertia = 0.0;                   // sum of squared distances to the assigned centroid
};

// Index of the centroid closest to `v` (both padded to the centroid stride).
inline std::uint32_t nearestCentroid(const VectorSet& centroids, const float* v, float* bestDistOut = nullptr) noexcept {
    const std::size_t stride = centroids.stride();
    std::uint32_t best = 0;
    float bestDist = std::numeric_limits<float>::infinity();
    const float* row = centroids.data();
    for (std::size_t c = 0; c < centroids.size(); ++c, row += stride) {
        const float d = l2Sq(v, row, stride);
        if (d < bestDist) {
            bestDist = d;
            best = static_cast<std::uint32_t>(c);
        }
    }
    if (bestDistOut) {
        *bestDistOut = bestDist;
    }
    return best;
}

inline KMeansResult kmeans(const VectorSet& data, std::size_t k, KMeansParams params = {}) {
    if (k == 0) {
        throw std::invalid_argument("kmeans: k must be positive");
    }
    if (data.size() < k) {
        throw std::invalid_argument("kmeans: fewer points than clusters");
    }
    const std::size_t n = data.size();
    const std::size_t dim = data.dim();
    const std::size_t stride = data.stride();
    std::mt19937_64 rng(params.seed);

    KMeansResult result;
    result.centroids = VectorSet(dim);
    result.centroids.reserve(k);
    {
        // k-means++ seeding: each new centre is drawn with probability
        // proportional to its squared distance from the centres chosen so far,
        // which keeps the seeds spread across the data. O(n * k) distances.
        std::uniform_int_distribution<std::size_t> firstPick(0, n - 1);
        result.centroids.add(data[static_cast<Id>(firstPick(rng))]);
        std::vector<double> minDist(n);
        for (std::size_t c = 1; c < k; ++c) {
            const float* last = result.centroids[static_cast<Id>(c - 1)];
            double total = 0.0;
            for (std::size_t i = 0; i < n; ++i) {
                const double d = l2Sq(data[static_cast<Id>(i)], last, stride);
                minDist[i] = c == 1 ? d : std::min(minDist[i], d);
                total += minDist[i];
            }
            std::size_t chosen = n - 1;
            if (total > 0.0) {
                std::uniform_real_distribution<double> pick(0.0, total);
                double target = pick(rng);
                for (std::size_t i = 0; i < n; ++i) {
                    target -= minDist[i];
                    if (target <= 0.0) {
                        chosen = i;
                        break;
                    }
                }
            } else {
                // Every row coincides with a centre already; any row will do.
                chosen = firstPick(rng);
            }
            result.centroids.add(data[static_cast<Id>(chosen)]);
        }
    }

    result.assignment.assign(n, 0u);
    std::vector<double> sums(k * dim);
    std::vector<std::size_t> counts(k);
    std::uniform_int_distribution<std::size_t> anyRow(0, n - 1);

    for (std::size_t iter = 0; iter < params.iterations; ++iter) {
        bool changed = false;
        double inertia = 0.0;
        for (std::size_t i = 0; i < n; ++i) {
            float d = 0.0f;
            const std::uint32_t c = nearestCentroid(result.centroids, data[static_cast<Id>(i)], &d);
            inertia += d;
            if (c != result.assignment[i]) {
                result.assignment[i] = c;
                changed = true;
            }
        }
        result.inertia = inertia;
        result.iterationsRun = iter + 1;
        if (!changed && iter > 0) {
            break;
        }
        std::fill(sums.begin(), sums.end(), 0.0);
        std::fill(counts.begin(), counts.end(), 0u);
        for (std::size_t i = 0; i < n; ++i) {
            const std::uint32_t c = result.assignment[i];
            const float* p = data[static_cast<Id>(i)];
            double* s = sums.data() + c * dim;
            for (std::size_t d = 0; d < dim; ++d) {
                s[d] += p[d];
            }
            ++counts[c];
        }
        for (std::size_t c = 0; c < k; ++c) {
            float* centroid = result.centroids.mutableRow(static_cast<Id>(c));
            if (counts[c] == 0) {
                // Empty cluster: restart it on a random row so k stays honest.
                const float* p = data[static_cast<Id>(anyRow(rng))];
                std::copy(p, p + stride, centroid);
                changed = true;
                continue;
            }
            const double inv = 1.0 / static_cast<double>(counts[c]);
            const double* s = sums.data() + c * dim;
            for (std::size_t d = 0; d < dim; ++d) {
                centroid[d] = static_cast<float>(s[d] * inv);
            }
        }
    }
    return result;
}

}  // namespace ann
