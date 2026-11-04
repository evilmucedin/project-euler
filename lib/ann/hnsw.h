#pragma once

// Hierarchical Navigable Small World graph (Malkov & Yashunin, 2016).
//
// Layout choices that matter for speed:
//  * level-0 adjacency is one flat array with (M0 + 1) slots per node, so a
//    node's neighbours are a single contiguous read;
//  * vectors live in a cache-line aligned VectorSet and neighbour vectors are
//    prefetched one step ahead of the distance computation;
//  * the visited set is epoch based and recycled through a pool, so queries
//    allocate nothing after warm-up and can run concurrently from many threads.
//
// `add` is not thread-safe with respect to itself or to `search`.

#include "lib/ann/common.h"
#include "lib/ann/distance.h"
#include "lib/ann/topk.h"
#include "lib/ann/vectorSet.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <functional>
#include <queue>
#include <random>
#include <stdexcept>
#include <vector>

namespace ann {

template <class Metric = L2>
class HnswIndex {
public:
    using MetricType = Metric;

    struct Params {
        std::size_t M = 16;               // max links per node on levels > 0; level 0 gets 2 * M
        std::size_t efConstruction = 200; // beam width while inserting
        std::size_t efSearch = 64;        // default beam width while searching
        std::uint64_t seed = 42;
    };

    explicit HnswIndex(std::size_t dim, Params params = {})
        : params_(params), vectors_(dim), M0_(2 * params.M),
          levelMultiplier_(1.0 / std::log(static_cast<double>(std::max<std::size_t>(params.M, 2)))),
          rng_(params.seed) {
        if (params.M < 2) {
            throw std::invalid_argument("HnswIndex: M must be at least 2");
        }
        if (params.efConstruction == 0) {
            throw std::invalid_argument("HnswIndex: efConstruction must be positive");
        }
    }

    std::size_t dim() const noexcept { return vectors_.dim(); }
    std::size_t size() const noexcept { return vectors_.size(); }
    const Params& params() const noexcept { return params_; }
    const VectorSet& vectors() const noexcept { return vectors_; }
    int maxLevel() const noexcept { return maxLevel_; }
    Id entryPoint() const noexcept { return entry_; }
    int levelOf(Id id) const noexcept { return levels_[id]; }

    // Neighbour ids of `id` on `level`; for inspection and tests.
    std::vector<Id> neighbors(Id id, int level) const {
        const Id* links = linksAt(id, level);
        return std::vector<Id>(links + 1, links + 1 + links[0]);
    }

    void reserve(std::size_t n) {
        vectors_.reserve(n);
        levels_.reserve(n);
        links0_.reserve(n * (M0_ + 1));
        upper_.reserve(n);
    }

    Id add(const float* v) {
        const Id id = vectors_.add(v);
        if (Metric::kNormalizeOnInsert) {
            normalize(vectors_.mutableRow(id), vectors_.dim());
        }
        const int level = randomLevel();
        levels_.push_back(level);
        links0_.resize(links0_.size() + M0_ + 1, 0u);
        upper_.emplace_back(level > 0 ? static_cast<std::size_t>(level) * (params_.M + 1) : 0u, 0u);
        buildVisited_.resize(vectors_.size());

        if (entry_ == kInvalidId) {
            entry_ = id;
            maxLevel_ = level;
            return id;
        }

        const float* q = vectors_[id];
        Id ep = entry_;
        float epDist = Metric::distance(q, vectors_[ep], vectors_.stride());
        for (int l = maxLevel_; l > level; --l) {
            greedyDescend(q, l, ep, epDist);
        }
        for (int l = std::min(level, maxLevel_); l >= 0; --l) {
            TopK found = searchLayer(q, ep, epDist, params_.efConstruction, l, buildVisited_);
            std::vector<Neighbor> candidates = found.takeSorted();
            std::vector<Id> chosen = selectNeighbors(candidates, params_.M);
            connect(id, l, chosen);
            ep = candidates.front().id;
            epDist = candidates.front().dist;
        }
        if (level > maxLevel_) {
            maxLevel_ = level;
            entry_ = id;
        }
        return id;
    }

    void add(const VectorSet& set) {
        reserve(size() + set.size());
        for (std::size_t i = 0; i < set.size(); ++i) {
            add(set[static_cast<Id>(i)]);
        }
    }

    // `ef` of 0 means params().efSearch; the effective beam is never below k.
    std::vector<Neighbor> search(const float* q, std::size_t k, std::size_t ef = 0) const {
        if (k == 0 || vectors_.empty()) {
            return {};
        }
        const AlignedQuery query = AlignedQuery::forMetric<Metric>(q, vectors_);
        return searchPrepared(query.data(), k, ef);
    }

    std::vector<Neighbor> searchPrepared(const float* q, std::size_t k, std::size_t ef = 0) const {
        if (k == 0 || vectors_.empty()) {
            return {};
        }
        ef = std::max(k, ef == 0 ? params_.efSearch : ef);
        Id ep = entry_;
        float epDist = Metric::distance(q, vectors_[ep], vectors_.stride());
        for (int l = maxLevel_; l > 0; --l) {
            greedyDescend(q, l, ep, epDist);
        }
        VisitedPool::Lease visited = visitedPool_.acquire(vectors_.size());
        TopK found = searchLayer(q, ep, epDist, ef, 0, *visited);
        std::vector<Neighbor> out = found.takeSorted();
        if (out.size() > k) {
            out.resize(k);
        }
        return out;
    }

private:
    using MinHeap = std::priority_queue<Neighbor, std::vector<Neighbor>, std::greater<Neighbor>>;

    std::size_t linkCapacity(int level) const noexcept { return level == 0 ? M0_ : params_.M; }

    Id* linksAt(Id id, int level) noexcept {
        return level == 0 ? &links0_[static_cast<std::size_t>(id) * (M0_ + 1)]
                          : &upper_[id][static_cast<std::size_t>(level - 1) * (params_.M + 1)];
    }

    const Id* linksAt(Id id, int level) const noexcept {
        return level == 0 ? &links0_[static_cast<std::size_t>(id) * (M0_ + 1)]
                          : &upper_[id][static_cast<std::size_t>(level - 1) * (params_.M + 1)];
    }

    int randomLevel() {
        std::uniform_real_distribution<double> uniform(0.0, 1.0);
        const double u = 1.0 - uniform(rng_);  // (0, 1]
        const double level = -std::log(u) * levelMultiplier_;
        return static_cast<int>(std::min(level, 48.0));
    }

    // Hill-climbs on one layer until no neighbour is closer. Used for levels
    // above the target where a beam of one is enough.
    void greedyDescend(const float* q, int level, Id& ep, float& epDist) const {
        const std::size_t stride = vectors_.stride();
        bool improved = true;
        while (improved) {
            improved = false;
            const Id* links = linksAt(ep, level);
            const std::size_t count = links[0];
            for (std::size_t j = 0; j < count; ++j) {
                if (j + 1 < count) {
                    prefetch(vectors_[links[2 + j]]);
                }
                const Id candidate = links[1 + j];
                const float d = Metric::distance(q, vectors_[candidate], stride);
                if (d < epDist) {
                    epDist = d;
                    ep = candidate;
                    improved = true;
                }
            }
        }
    }

    // Beam search on one layer; returns up to `ef` closest nodes as a max-heap.
    TopK searchLayer(const float* q, Id ep, float epDist, std::size_t ef, int level, VisitedSet& visited) const {
        const std::size_t stride = vectors_.stride();
        visited.clear();
        visited.set(ep);
        TopK results(ef);
        results.push(ep, epDist);
        MinHeap candidates;
        candidates.emplace(ep, epDist);
        while (!candidates.empty()) {
            const Neighbor current = candidates.top();
            if (current.dist > results.worst()) {
                break;
            }
            candidates.pop();
            const Id* links = linksAt(current.id, level);
            const std::size_t count = links[0];
            for (std::size_t j = 0; j < count; ++j) {
                if (j + 1 < count) {
                    prefetch(vectors_[links[2 + j]]);
                }
                const Id candidate = links[1 + j];
                if (visited.testAndSet(candidate)) {
                    continue;
                }
                const float d = Metric::distance(q, vectors_[candidate], stride);
                if (d < results.worst()) {
                    candidates.emplace(candidate, d);
                    results.push(candidate, d);
                }
            }
        }
        return results;
    }

    // Heuristic neighbour selection (Algorithm 4 of the paper): keep a
    // candidate only if it is closer to the query than to every neighbour
    // already kept, which spreads links in different directions.
    std::vector<Id> selectNeighbors(const std::vector<Neighbor>& sortedCandidates, std::size_t m) const {
        std::vector<Id> chosen;
        chosen.reserve(std::min(m, sortedCandidates.size()));
        if (sortedCandidates.size() <= m) {
            for (const Neighbor& c : sortedCandidates) {
                chosen.push_back(c.id);
            }
            return chosen;
        }
        const std::size_t stride = vectors_.stride();
        for (const Neighbor& c : sortedCandidates) {
            if (chosen.size() >= m) {
                break;
            }
            bool keep = true;
            for (Id r : chosen) {
                if (Metric::distance(vectors_[c.id], vectors_[r], stride) < c.dist) {
                    keep = false;
                    break;
                }
            }
            if (keep) {
                chosen.push_back(c.id);
            }
        }
        return chosen;
    }

    void connect(Id id, int level, const std::vector<Id>& chosen) {
        const std::size_t cap = linkCapacity(level);
        const std::size_t stride = vectors_.stride();
        Id* own = linksAt(id, level);
        own[0] = static_cast<Id>(chosen.size());
        std::copy(chosen.begin(), chosen.end(), own + 1);

        for (Id nb : chosen) {
            Id* links = linksAt(nb, level);
            if (links[0] < cap) {
                links[1 + links[0]] = id;
                ++links[0];
                continue;
            }
            // Neighbour is full: re-select its links among the old ones plus the new node.
            std::vector<Neighbor> pool;
            pool.reserve(cap + 1);
            const float* nbVec = vectors_[nb];
            pool.emplace_back(id, Metric::distance(nbVec, vectors_[id], stride));
            for (std::size_t j = 0; j < links[0]; ++j) {
                pool.emplace_back(links[1 + j], Metric::distance(nbVec, vectors_[links[1 + j]], stride));
            }
            std::sort(pool.begin(), pool.end());
            const std::vector<Id> kept = selectNeighbors(pool, cap);
            links[0] = static_cast<Id>(kept.size());
            std::copy(kept.begin(), kept.end(), links + 1);
        }
    }

    Params params_;
    VectorSet vectors_;
    std::size_t M0_;
    double levelMultiplier_;
    std::mt19937_64 rng_;
    std::vector<int> levels_;
    std::vector<Id> links0_;               // (M0 + 1) slots per node: count, then ids
    std::vector<std::vector<Id>> upper_;   // per node: level * (M + 1) slots, empty for level-0 nodes
    Id entry_{kInvalidId};
    int maxLevel_{-1};
    VisitedSet buildVisited_;
    mutable VisitedPool visitedPool_;
};

}  // namespace ann
