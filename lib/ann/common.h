#pragma once

// Shared building blocks for the ann library: identifiers, the Neighbor record
// returned by every index, a cache-line aligned allocator, an epoch-based
// visited set, and a few portable compiler hints.

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <mutex>
#include <new>
#include <vector>

#if defined(__GNUC__) || defined(__clang__)
#define ANN_LIKELY(x) __builtin_expect(!!(x), 1)
#define ANN_UNLIKELY(x) __builtin_expect(!!(x), 0)
#define ANN_ALWAYS_INLINE inline __attribute__((always_inline))
#else
#define ANN_LIKELY(x) (x)
#define ANN_UNLIKELY(x) (x)
#define ANN_ALWAYS_INLINE inline
#endif

namespace ann {

using Id = std::uint32_t;

inline constexpr Id kInvalidId = std::numeric_limits<Id>::max();

// One cache line on every mainstream CPU and the widest SIMD register (AVX-512).
inline constexpr std::size_t kAlignment = 64;
inline constexpr std::size_t kFloatsPerLine = kAlignment / sizeof(float);

inline constexpr std::size_t roundUp(std::size_t n, std::size_t multiple) noexcept {
    return (n + multiple - 1) / multiple * multiple;
}

ANN_ALWAYS_INLINE void prefetch(const void* p) noexcept {
#if defined(__GNUC__) || defined(__clang__)
    __builtin_prefetch(p, 0, 3);
#else
    (void)p;
#endif
}

// A search hit. Ordered by distance first so it can live directly in heaps;
// the id breaks ties deterministically.
struct Neighbor {
    Id id{kInvalidId};
    float dist{std::numeric_limits<float>::infinity()};

    constexpr Neighbor() = default;
    constexpr Neighbor(Id i, float d) noexcept : id(i), dist(d) {}

    friend constexpr bool operator<(const Neighbor& a, const Neighbor& b) noexcept {
        return a.dist < b.dist || (a.dist == b.dist && a.id < b.id);
    }
    friend constexpr bool operator>(const Neighbor& a, const Neighbor& b) noexcept { return b < a; }
    friend constexpr bool operator==(const Neighbor& a, const Neighbor& b) noexcept {
        return a.id == b.id && a.dist == b.dist;
    }
    friend constexpr bool operator!=(const Neighbor& a, const Neighbor& b) noexcept { return !(a == b); }
};

// std::allocator replacement that hands out kAlignment-aligned blocks so rows
// can be loaded with aligned SIMD instructions and never straddle cache lines.
template <class T>
struct AlignedAllocator {
    using value_type = T;
    using propagate_on_container_move_assignment = std::true_type;
    using is_always_equal = std::true_type;

    AlignedAllocator() noexcept = default;
    template <class U>
    AlignedAllocator(const AlignedAllocator<U>&) noexcept {}

    template <class U>
    struct rebind {
        using other = AlignedAllocator<U>;
    };

    T* allocate(std::size_t n) {
        if (n > std::numeric_limits<std::size_t>::max() / sizeof(T)) {
            throw std::bad_alloc();
        }
        const std::size_t bytes = roundUp(n * sizeof(T), kAlignment);
        return static_cast<T*>(::operator new(bytes, std::align_val_t(kAlignment)));
    }

    void deallocate(T* p, std::size_t) noexcept { ::operator delete(p, std::align_val_t(kAlignment)); }

    template <class U>
    friend bool operator==(const AlignedAllocator&, const AlignedAllocator<U>&) noexcept {
        return true;
    }
    template <class U>
    friend bool operator!=(const AlignedAllocator&, const AlignedAllocator<U>&) noexcept {
        return false;
    }
};

template <class T>
using AlignedVector = std::vector<T, AlignedAllocator<T>>;

// Marks ids as visited during one graph or tree traversal. Clearing is O(1):
// it bumps an epoch instead of touching memory, so a search over a few
// hundred nodes does not pay for zeroing a bitmap sized to the whole index.
class VisitedSet {
public:
    VisitedSet() = default;
    explicit VisitedSet(std::size_t n) : marks_(n, 0) {}

    std::size_t size() const noexcept { return marks_.size(); }

    void resize(std::size_t n) {
        if (n > marks_.size()) {
            marks_.resize(n, 0);
        }
    }

    void clear() noexcept {
        if (ANN_UNLIKELY(++epoch_ == 0)) {
            std::fill(marks_.begin(), marks_.end(), 0u);
            epoch_ = 1;
        }
    }

    bool test(Id id) const noexcept { return marks_[id] == epoch_; }

    void set(Id id) noexcept { marks_[id] = epoch_; }

    // Returns the previous state and marks the id in one step.
    bool testAndSet(Id id) noexcept {
        if (marks_[id] == epoch_) {
            return true;
        }
        marks_[id] = epoch_;
        return false;
    }

private:
    std::vector<std::uint32_t> marks_;
    std::uint32_t epoch_{1};
};

// Recycles VisitedSets between concurrent searches so a read-only index can be
// queried from many threads without allocating per query.
class VisitedPool {
public:
    class Lease {
    public:
        Lease(VisitedPool& pool, std::unique_ptr<VisitedSet> set) noexcept : pool_(&pool), set_(std::move(set)) {}
        Lease(Lease&&) noexcept = default;
        Lease& operator=(Lease&&) = delete;
        ~Lease() {
            if (set_) {
                pool_->release(std::move(set_));
            }
        }
        VisitedSet& operator*() noexcept { return *set_; }
        VisitedSet* operator->() noexcept { return set_.get(); }

    private:
        VisitedPool* pool_;
        std::unique_ptr<VisitedSet> set_;
    };

    Lease acquire(std::size_t n) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!free_.empty()) {
                std::unique_ptr<VisitedSet> set = std::move(free_.back());
                free_.pop_back();
                set->resize(n);
                return Lease(*this, std::move(set));
            }
        }
        return Lease(*this, std::make_unique<VisitedSet>(n));
    }

private:
    void release(std::unique_ptr<VisitedSet> set) {
        std::lock_guard<std::mutex> lock(mutex_);
        free_.push_back(std::move(set));
    }

    std::mutex mutex_;
    std::vector<std::unique_ptr<VisitedSet>> free_;
};

}  // namespace ann
