#pragma once

template <typename T>
struct UnionFind {
   public:
    void add(T first) { parent_.emplace(first, first); }

    bool has(T first) const {
        return parent_.find(first) != parent_.end();
    }

    T find(T first) {
        auto toParent = parent_.find(first);
        while (toParent != parent_.end() && toParent->second != first) {
            first = toParent->second;
            toParent = parent_.find(first);
        }
        return first;
    }

    void unite(T first, T second) {
        auto toSecond = parent_.find(second);
        if (toSecond != parent_.end()) {
            parent_[find(first)] = toSecond->second;
        }
    }

    struct Component {
        T first;
        size_t size;
    };
    using Components = vector<Component>;

    Components components() {
        unordered_map<T, size_t> parentSizes;
        for (const auto& kv : parent_) {
            T j = kv.first;
            auto toParent = parent_.find(j);
            while (toParent != parent_.end() && toParent->second != j) {
                j = toParent->second;
                toParent = parent_.find(j);
            }
            ++parentSizes[j];

            T k = kv.first;
            toParent = parent_.find(k);
            while (toParent != parent_.end() && toParent->second != k) {
                parent_[k] = j;
                k = toParent->second;
                toParent = parent_.find(k);
            }
        }

        Components result;
        for (const auto& kv: parentSizes) {
            result.emplace_back(Component{kv.first, kv.second});
        }
        return result;
    }

   private:
    unordered_map<T, T> parent_;
};
