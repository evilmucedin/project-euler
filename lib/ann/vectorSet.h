#pragma once

// Row-major, cache-line aligned storage for fixed-dimension float vectors.
//
// Each row is padded with zeros to a multiple of kFloatsPerLine so a distance
// kernel can run over `stride()` floats with no tail handling and so no row
// shares a cache line with its neighbour.

#include "lib/ann/common.h"
#include "lib/ann/distance.h"

#include <cstring>
#include <stdexcept>
#include <vector>

namespace ann {

class VectorSet {
public:
    VectorSet() = default;

    explicit VectorSet(std::size_t dim) : dim_(dim), stride_(roundUp(dim, kFloatsPerLine)) {
        if (dim == 0) {
            throw std::invalid_argument("VectorSet: dim must be positive");
        }
    }

    // Copies `n` contiguous, unpadded rows of `dim` floats.
    VectorSet(std::size_t dim, std::size_t n, const float* data) : VectorSet(dim) {
        reserve(n);
        for (std::size_t i = 0; i < n; ++i) {
            add(data + i * dim);
        }
    }

    std::size_t dim() const noexcept { return dim_; }
    std::size_t stride() const noexcept { return stride_; }
    std::size_t size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }
    std::size_t capacity() const noexcept { return stride_ ? data_.capacity() / stride_ : 0; }

    void reserve(std::size_t n) { data_.reserve(n * stride_); }

    void clear() noexcept {
        data_.clear();
        size_ = 0;
    }

    Id add(const float* v) {
        data_.resize(data_.size() + stride_);  // value-initialised, so padding stays zero
        std::memcpy(data_.data() + size_ * stride_, v, dim_ * sizeof(float));
        return static_cast<Id>(size_++);
    }

    Id add(const std::vector<float>& v) {
        if (v.size() != dim_) {
            throw std::invalid_argument("VectorSet::add: wrong dimension");
        }
        return add(v.data());
    }

    void append(const VectorSet& other) {
        if (other.dim_ != dim_) {
            throw std::invalid_argument("VectorSet::append: dimension mismatch");
        }
        reserve(size_ + other.size_);
        for (std::size_t i = 0; i < other.size_; ++i) {
            add(other[static_cast<Id>(i)]);
        }
    }

    const float* operator[](Id i) const noexcept { return data_.data() + static_cast<std::size_t>(i) * stride_; }
    float* mutableRow(Id i) noexcept { return data_.data() + static_cast<std::size_t>(i) * stride_; }

    const float* data() const noexcept { return data_.data(); }

    std::size_t bytes() const noexcept { return data_.size() * sizeof(float); }

private:
    std::size_t dim_{0};
    std::size_t stride_{0};
    std::size_t size_{0};
    AlignedVector<float> data_;
};

// A query copied into an aligned, zero-padded buffer matching a VectorSet's
// stride, optionally normalised. Lets the hot loop use the same padded-length
// kernel for the query as for stored rows.
class AlignedQuery {
public:
    AlignedQuery(const float* q, std::size_t dim, std::size_t stride, bool normalizeQuery) : buf_(stride, 0.0f) {
        std::memcpy(buf_.data(), q, dim * sizeof(float));
        if (normalizeQuery) {
            normalize(buf_.data(), dim);
        }
    }

    template <class Metric>
    static AlignedQuery forMetric(const float* q, const VectorSet& set) {
        return AlignedQuery(q, set.dim(), set.stride(), Metric::kNormalizeOnInsert);
    }

    const float* data() const noexcept { return buf_.data(); }

private:
    AlignedVector<float> buf_;
};

}  // namespace ann
