#pragma once

// Inverted-file index with flat (uncompressed) lists.
//
// k-means partitions the space into `nlist` cells; each vector is stored in
// the cell of its nearest centroid as a contiguous aligned block. A query
// scans only the `nprobe` closest cells, trading recall for speed linearly.
// Good when the data is clustered and for high dimensions where trees fail.

#include "lib/ann/common.h"
#include "lib/ann/distance.h"
#include "lib/ann/kmeans.h"
#include "lib/ann/topk.h"
#include "lib/ann/vectorSet.h"

#include <algorithm>
#include <stdexcept>
#include <vector>

namespace ann {

template <class Metric = L2>
class IvfFlatIndex {
public:
    using MetricType = Metric;

    struct Params {
        std::size_t nlist = 64;   // number of cells
        std::size_t nprobe = 8;   // default cells visited per query
        KMeansParams kmeans;
    };

    explicit IvfFlatIndex(std::size_t dim, Params params = {}) : params_(params), dim_(dim), centroids_(dim) {
        if (params.nlist == 0) {
            throw std::invalid_argument("IvfFlatIndex: nlist must be positive");
        }
    }

    std::size_t dim() const noexcept { return dim_; }
    std::size_t size() const noexcept { return size_; }
    std::size_t nlist() const noexcept { return params_.nlist; }
    bool trained() const noexcept { return !centroids_.empty(); }
    const Params& params() const noexcept { return params_; }
    const VectorSet& centroids() const noexcept { return centroids_; }
    std::size_t listSize(std::size_t cell) const noexcept { return lists_[cell].ids.size(); }

    // Learns the cells from a (representative) sample. Must run before add().
    void train(const VectorSet& sample) {
        if (sample.dim() != dim_) {
            throw std::invalid_argument("IvfFlatIndex::train: dimension mismatch");
        }
        VectorSet prepared(dim_);
        const VectorSet* source = &sample;
        if (Metric::kNormalizeOnInsert) {
            prepared.reserve(sample.size());
            for (std::size_t i = 0; i < sample.size(); ++i) {
                const Id id = prepared.add(sample[static_cast<Id>(i)]);
                normalize(prepared.mutableRow(id), dim_);
            }
            source = &prepared;
        }
        const std::size_t k = std::min(params_.nlist, source->size());
        if (k == 0) {
            throw std::invalid_argument("IvfFlatIndex::train: empty sample");
        }
        centroids_ = kmeans(*source, k, params_.kmeans).centroids;
        lists_.clear();
        lists_.resize(centroids_.size());
        for (List& list : lists_) {
            list.vectors = VectorSet(dim_);
        }
        size_ = 0;
    }

    // Trains on the whole set and then adds it.
    void build(const VectorSet& set) {
        train(set);
        add(set);
    }

    Id add(const float* v) {
        if (!trained()) {
            throw std::logic_error("IvfFlatIndex::add: train() first");
        }
        const AlignedQuery prepared(v, dim_, centroids_.stride(), Metric::kNormalizeOnInsert);
        const std::uint32_t cell = nearestCentroid(centroids_, prepared.data());
        List& list = lists_[cell];
        list.vectors.add(prepared.data());
        const Id id = static_cast<Id>(size_++);
        list.ids.push_back(id);
        return id;
    }

    void add(const VectorSet& set) {
        for (std::size_t i = 0; i < set.size(); ++i) {
            add(set[static_cast<Id>(i)]);
        }
    }

    // `nprobe` of 0 means params().nprobe.
    std::vector<Neighbor> search(const float* q, std::size_t k, std::size_t nprobe = 0) const {
        if (k == 0 || size_ == 0) {
            return {};
        }
        nprobe = std::min(nprobe == 0 ? params_.nprobe : nprobe, centroids_.size());
        const AlignedQuery query(q, dim_, centroids_.stride(), Metric::kNormalizeOnInsert);
        const float* qp = query.data();
        const std::size_t stride = centroids_.stride();

        TopK cells(nprobe);
        {
            const float* row = centroids_.data();
            for (std::size_t c = 0; c < centroids_.size(); ++c, row += stride) {
                cells.push(static_cast<Id>(c), l2Sq(qp, row, stride));
            }
        }

        TopK top(k);
        for (const Neighbor& cell : cells.items()) {
            const List& list = lists_[cell.id];
            const float* row = list.vectors.data();
            const std::size_t n = list.ids.size();
            for (std::size_t i = 0; i < n; ++i, row += stride) {
                prefetch(row + 4 * stride);
                top.push(list.ids[i], Metric::distance(qp, row, stride));
            }
        }
        return top.takeSorted();
    }

private:
    struct List {
        VectorSet vectors;
        std::vector<Id> ids;
    };

    Params params_;
    std::size_t dim_;
    std::size_t size_{0};
    VectorSet centroids_;
    std::vector<List> lists_;
};

}  // namespace ann
