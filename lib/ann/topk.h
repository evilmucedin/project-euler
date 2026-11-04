#pragma once

// Bounded max-heap that keeps the k closest neighbours seen so far.

#include "lib/ann/common.h"

#include <algorithm>
#include <limits>
#include <vector>

namespace ann {

class TopK {
public:
    explicit TopK(std::size_t k) : k_(k) { heap_.reserve(k + 1); }

    std::size_t capacity() const noexcept { return k_; }
    std::size_t size() const noexcept { return heap_.size(); }
    bool empty() const noexcept { return heap_.empty(); }
    bool full() const noexcept { return heap_.size() >= k_; }

    // Distance a candidate must beat to be inserted; +inf while not full.
    float worst() const noexcept { return full() ? threshold() : std::numeric_limits<float>::infinity(); }

    // Largest distance currently held (undefined when empty).
    float threshold() const noexcept { return heap_.front().dist; }

    const Neighbor& top() const noexcept { return heap_.front(); }

    bool push(Id id, float dist) {
        if (heap_.size() < k_) {
            heap_.emplace_back(id, dist);
            std::push_heap(heap_.begin(), heap_.end());
            return true;
        }
        if (k_ == 0 || !(dist < heap_.front().dist)) {
            return false;
        }
        std::pop_heap(heap_.begin(), heap_.end());
        heap_.back() = Neighbor(id, dist);
        std::push_heap(heap_.begin(), heap_.end());
        return true;
    }

    bool push(const Neighbor& n) { return push(n.id, n.dist); }

    void pop() {
        std::pop_heap(heap_.begin(), heap_.end());
        heap_.pop_back();
    }

    void clear() noexcept { heap_.clear(); }

    // Unordered view of the current contents.
    const std::vector<Neighbor>& items() const noexcept { return heap_; }

    // Drains the heap into an ascending vector. The TopK is empty afterwards.
    std::vector<Neighbor> takeSorted() {
        std::sort_heap(heap_.begin(), heap_.end());
        std::vector<Neighbor> out = std::move(heap_);
        heap_.clear();
        return out;
    }

private:
    std::size_t k_;
    std::vector<Neighbor> heap_;
};

}  // namespace ann
