#pragma once

#include <unordered_map>

struct UnionFindDense {
   public:
    UnionFindDense(size_t n) : parent_(n), parentSize_(n) {
        for (size_t i = 0; i < n; ++i) {
            parent_[i] = i;
        }
        for (size_t i = 0; i < n; ++i) {
            parentSize_[i] = 1;
        }
    }

    size_t find(size_t first) {
        size_t i = first;
        auto parent = parent_[i];
        int count = 0;
        while (parent != i) {
            i = parent;
            parent = parent_[i];
            ++count;
        }

        if (count > 2) {
            size_t j = first;
            parent = parent_[j];
            while (parent != j) {
                parent_[j] = i;
                j = parent;
                parent = parent_[j];
            }
        }

        return i;
    }

    void unite(size_t first, size_t second) {
        auto firstParent = find(first);
        auto secondParent = find(second);
        if (parentSize_[firstParent] > parentSize_[secondParent]) {
            swap(first, second);
            swap(firstParent, secondParent);
        }
        parent_[firstParent] = secondParent;
        parentSize_[secondParent] += parentSize_[firstParent];
        parentSize_[firstParent] = 0;
    }

    struct Component {
        size_t first;
        size_t size;
    };
    using Components = vector<Component>;

    Components components() {
        unordered_map<T, size_t> parentSizes;
        for (const auto& kv : parent_) {
            ++parentSizes[find(kv.first)];
        }

        Components result;
        for (const auto& kv: parentSizes) {
            result.emplace_back(Component{kv.first, kv.second});
        }
        return result;
    }

   private:
    vector<size_t> parent_;
    vector<size_t> parentSize_;
};
