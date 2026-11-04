#pragma once

// Static k-d tree over L2 for low- to mid-dimensional geometry (2 to ~20 dims).
//
// Points are reordered so that each leaf is one contiguous block of rows, the
// tree is a flat node array, and the search uses incremental distance bounds
// (Arya & Mount) so a far subtree is visited only when its box can still beat
// the current k-th best. Set `eps` or `maxLeaves` for approximate search.

#include "lib/ann/common.h"
#include "lib/ann/distance.h"
#include "lib/ann/topk.h"
#include "lib/ann/vectorSet.h"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <vector>

namespace ann {

struct KdTreeParams {
    std::size_t leafSize = 16;
};

struct KdTreeSearchParams {
    // Allowed relative error: a returned neighbour is within (1 + eps) of the true one.
    float eps = 0.0f;
    // Stop after visiting this many leaves (0 = unlimited). Makes search time predictable.
    std::size_t maxLeaves = 0;
};

class KdTree {
public:
    using Params = KdTreeParams;
    using SearchParams = KdTreeSearchParams;

    KdTree() = default;

    explicit KdTree(const VectorSet& points, Params params = {}) { build(points, params); }

    void build(const VectorSet& input, Params params = {}) {
        if (params.leafSize == 0) {
            throw std::invalid_argument("KdTree: leafSize must be positive");
        }
        leafSize_ = params.leafSize;
        const std::size_t n = input.size();
        std::vector<Id> perm(n);
        std::iota(perm.begin(), perm.end(), 0u);
        nodes_.clear();
        nodes_.reserve(n == 0 ? 1 : 2 * (n / leafSize_ + 1));
        buildNode(input, perm, 0, n);
        points_ = VectorSet(input.dim());
        points_.reserve(n);
        ids_ = std::move(perm);
        for (Id id : ids_) {
            points_.add(input[id]);
        }
    }

    std::size_t size() const noexcept { return points_.size(); }
    std::size_t dim() const noexcept { return points_.dim(); }
    std::size_t nodeCount() const noexcept { return nodes_.size(); }
    std::size_t leafCount() const noexcept {
        std::size_t c = 0;
        for (const Node& node : nodes_) {
            c += node.isLeaf();
        }
        return c;
    }

    std::vector<Neighbor> search(const float* q, std::size_t k, SearchParams sp = {}) const {
        if (k == 0 || points_.empty()) {
            return {};
        }
        const AlignedQuery query(q, points_.dim(), points_.stride(), false);
        TopK top(k);
        // Bounds are compared in squared distance. A far subtree is skipped unless
        // its box could beat the current k-th best by a factor of (1 + eps), which
        // is what makes every returned neighbour (1 + eps)-approximate.
        const float scale = (1.0f + sp.eps) * (1.0f + sp.eps);
        const std::size_t budget = sp.maxLeaves == 0 ? std::numeric_limits<std::size_t>::max() : sp.maxLeaves;
        Cursor cursor{query.data(), top, scale, budget, std::vector<float>(points_.dim(), 0.0f)};
        searchNode(0, 0.0f, cursor);
        std::vector<Neighbor> out = top.takeSorted();
        for (Neighbor& n : out) {
            n.id = ids_[n.id];
        }
        return out;
    }

private:
    struct Node {
        std::uint32_t begin = 0;  // leaf: first row; internal: unused
        std::uint32_t end = 0;    // leaf: one past last row; internal: unused
        std::uint32_t right = kLeafMarker;  // internal: index of right child; left child is index + 1
        std::uint32_t splitDim = 0;
        float split = 0.0f;

        static constexpr std::uint32_t kLeafMarker = std::numeric_limits<std::uint32_t>::max();
        bool isLeaf() const noexcept { return right == kLeafMarker; }
    };

    struct Cursor {
        const float* q;
        TopK& top;
        float scale;
        std::size_t leavesLeft;
        std::vector<float> offsets;  // squared per-dimension distance from q to the current box
    };

    std::uint32_t buildNode(const VectorSet& input, std::vector<Id>& perm, std::size_t begin, std::size_t end) {
        const std::uint32_t index = static_cast<std::uint32_t>(nodes_.size());
        nodes_.emplace_back();
        if (end - begin <= leafSize_) {
            nodes_[index].begin = static_cast<std::uint32_t>(begin);
            nodes_[index].end = static_cast<std::uint32_t>(end);
            return index;
        }
        const std::size_t dim = input.dim();
        std::vector<float> lo(dim, std::numeric_limits<float>::infinity());
        std::vector<float> hi(dim, -std::numeric_limits<float>::infinity());
        for (std::size_t i = begin; i < end; ++i) {
            const float* p = input[perm[i]];
            for (std::size_t d = 0; d < dim; ++d) {
                lo[d] = std::min(lo[d], p[d]);
                hi[d] = std::max(hi[d], p[d]);
            }
        }
        std::size_t splitDim = 0;
        float spread = -1.0f;
        for (std::size_t d = 0; d < dim; ++d) {
            if (hi[d] - lo[d] > spread) {
                spread = hi[d] - lo[d];
                splitDim = d;
            }
        }
        if (!(spread > 0.0f)) {
            // All points identical: a bigger leaf beats an endless split.
            nodes_[index].begin = static_cast<std::uint32_t>(begin);
            nodes_[index].end = static_cast<std::uint32_t>(end);
            return index;
        }
        const std::size_t mid = begin + (end - begin) / 2;
        std::nth_element(perm.begin() + begin, perm.begin() + mid, perm.begin() + end,
                         [&](Id a, Id b) { return input[a][splitDim] < input[b][splitDim]; });
        const float split = input[perm[mid]][splitDim];
        nodes_[index].splitDim = static_cast<std::uint32_t>(splitDim);
        nodes_[index].split = split;
        buildNode(input, perm, begin, mid);  // left child lands at index + 1
        nodes_[index].right = buildNode(input, perm, mid, end);
        return index;
    }

    // Returns false once the leaf budget is exhausted.
    bool searchNode(std::uint32_t index, float lowerBound, Cursor& c) const {
        const Node& node = nodes_[index];
        if (node.isLeaf()) {
            if (c.leavesLeft == 0) {
                return false;
            }
            --c.leavesLeft;
            const std::size_t stride = points_.stride();
            const float* row = points_[node.begin];
            for (std::uint32_t i = node.begin; i < node.end; ++i, row += stride) {
                c.top.push(i, l2Sq(c.q, row, stride));
            }
            return true;
        }
        const float diff = c.q[node.splitDim] - node.split;
        const std::uint32_t near = diff < 0.0f ? index + 1 : node.right;
        const std::uint32_t far = diff < 0.0f ? node.right : index + 1;
        if (!searchNode(near, lowerBound, c)) {
            return false;
        }
        float& offset = c.offsets[node.splitDim];
        const float saved = offset;
        const float farBound = lowerBound - saved + diff * diff;
        bool ok = true;
        if (farBound * c.scale < c.top.worst()) {
            offset = diff * diff;
            ok = searchNode(far, farBound, c);
            offset = saved;
        }
        return ok;
    }

    std::size_t leafSize_{16};
    std::vector<Node> nodes_;
    VectorSet points_;
    std::vector<Id> ids_;
};

}  // namespace ann
