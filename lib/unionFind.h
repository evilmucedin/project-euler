#pragma once

#include <unordered_map>
#include <vector>

template <typename T>
struct UnionFind {
   public:
    void add(T first) { parent_.emplace(first, first); }

    bool has(T first) const { return parent_.find(first) != parent_.end(); }

    T find(T first) {
        T i = first;
        auto toParent = parent_.find(i);
        int count = 0;
        while (toParent != parent_.end() && toParent->second != i) {
            i = toParent->second;
            toParent = parent_.find(i);
            ++count;
        }

        if (count > 2) {
            T j = first;
            toParent = parent_.find(j);
            while (toParent != parent_.end() && toParent->second != j) {
                parent_[j] = i;
                j = toParent->second;
                toParent = parent_.find(j);
            }
        }

        return i;
    }

    void unite(T first, T second) {
        auto toSecond = parent_.find(second);
        if (toSecond != parent_.end()) {
            parent_[find(first)] = find(toSecond->second);
        }
    }

    struct Component {
        T first;
        size_t size;
    };
    using Components = std::vector<Component>;

    Components components() {
        unordered_map<T, size_t> parentSizes;
        for (const auto& kv : parent_) {
            ++parentSizes[find(kv.first)];
        }

        Components result;
        for (const auto& kv : parentSizes) {
            result.emplace_back(Component{kv.first, kv.second});
        }
        return result;
    }

   private:
    std::unordered_map<T, T> parent_;
};

namespace std {

template <typename T>
inline string to_string(const typename UnionFind<T>::Component& c) {
    return string("{") + to_string(c.first) + ", " + to_string(c.size) + "}";
}

}  // namespace std
