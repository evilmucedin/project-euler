#pragma once

#include "lib/header.h"

template<typename T, typename Aggr>
struct SegmentTree {
    SegmentTree(size_t n, const T& identity = T(), Aggr aggr = Aggr())
        : identity_(identity)
        , aggr_(aggr)
    {
        size_ = 1;
        while (size_ < n) {
            size_ <<= 1;
        }
        storage_.resize(2 * size_, identity);
    }

    SegmentTree(const vector<T>& data, const T& identity = T(), Aggr aggr = Aggr())
        : SegmentTree(data.size(), identity, aggr) {
        for (size_t i = 0; i < data.size(); ++i) {
            set(i, data[i]);
        }
    }

    void set(size_t index, const T& value) {
        index += size_;
        while (index) {
            storage_[index] = aggr_(storage_[index], value);
            index >>= 1;
        }
    }

    T get(size_t index) const {
        return storage_[index + size_];
    }

    T getAggr(size_t left, size_t right) const {
        T result = identity_;
        left += size_;
        right += size_;
        while (left < right) {
            if (left & 1) {
                result = aggr_(result, storage_[left]);
                ++left;
            }
            if (right & 1) {
                --right;
                result = aggr_(result, storage_[right]);
            }
            left >>= 1;
            right >>= 1;
        }
        return result;
    }

    size_t size_;
    vector<T> storage_;
    T identity_;
    Aggr aggr_;
};

template<typename T>
struct Sum {
    T operator()(const T& a, const T& b) const {
        return a + b;
    }
};

template<typename T>
struct Max {
    T operator()(const T& a, const T& b) const {
        return max(a, b);
    }
};

template<typename T>
struct Min {
    T operator()(const T& a, const T& b) const {
        return min(a, b);
    }
};
