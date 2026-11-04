#pragma once

// Exact linear scan. The reference every approximate index is measured
// against, and the fastest option for small sets.

#include "lib/ann/common.h"
#include "lib/ann/distance.h"
#include "lib/ann/topk.h"
#include "lib/ann/vectorSet.h"

#include <vector>

namespace ann {

template <class Metric = L2>
class BruteForceIndex {
public:
    using MetricType = Metric;

    explicit BruteForceIndex(std::size_t dim) : vectors_(dim) {}

    std::size_t dim() const noexcept { return vectors_.dim(); }
    std::size_t size() const noexcept { return vectors_.size(); }
    const VectorSet& vectors() const noexcept { return vectors_; }

    void reserve(std::size_t n) { vectors_.reserve(n); }

    Id add(const float* v) {
        const Id id = vectors_.add(v);
        if (Metric::kNormalizeOnInsert) {
            normalize(vectors_.mutableRow(id), vectors_.dim());
        }
        return id;
    }

    void add(const VectorSet& set) {
        vectors_.reserve(vectors_.size() + set.size());
        for (std::size_t i = 0; i < set.size(); ++i) {
            add(set[static_cast<Id>(i)]);
        }
    }

    std::vector<Neighbor> search(const float* q, std::size_t k) const {
        if (k == 0 || vectors_.empty()) {
            return {};
        }
        const AlignedQuery query = AlignedQuery::forMetric<Metric>(q, vectors_);
        return searchPrepared(query.data(), k);
    }

    // `q` must already be padded/normalised like the stored rows (see AlignedQuery).
    std::vector<Neighbor> searchPrepared(const float* q, std::size_t k) const {
        TopK top(k);
        const std::size_t stride = vectors_.stride();
        const std::size_t n = vectors_.size();
        const float* row = vectors_.data();
        for (std::size_t i = 0; i < n; ++i, row += stride) {
            prefetch(row + 4 * stride);
            top.push(static_cast<Id>(i), Metric::distance(q, row, stride));
        }
        return top.takeSorted();
    }

private:
    VectorSet vectors_;
};

}  // namespace ann
