// Builds every index over the same synthetic dataset and prints build time,
// queries per second and recall@k against brute force.
//
//   buck2 run //lib/ann/bench:annBench -- [n] [dim] [queries] [k]

#include "lib/ann/bruteForce.h"
#include "lib/ann/evaluation.h"
#include "lib/ann/hnsw.h"
#include "lib/ann/ivfFlat.h"
#include "lib/ann/kdTree.h"
#include "lib/ann/synthetic.h"

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <string>
#include <vector>

namespace {

using Clock = std::chrono::steady_clock;

double millisSince(Clock::time_point start) {
    return std::chrono::duration<double, std::milli>(Clock::now() - start).count();
}

struct Row {
    std::string name;
    double buildMs;
    double qps;
    double recall;
};

template <class Search>
Row measure(const std::string& name, double buildMs, const ann::VectorSet& queries, std::size_t k,
            const std::vector<std::vector<ann::Neighbor>>& truth, Search&& search) {
    std::vector<std::vector<ann::Neighbor>> results(queries.size());
    // Warm up once so page faults and pool allocations are not timed.
    results[0] = search(queries[0], k);
    const Clock::time_point start = Clock::now();
    for (ann::Id qi = 0; qi < queries.size(); ++qi) {
        results[qi] = search(queries[qi], k);
    }
    const double ms = millisSince(start);
    return Row{name, buildMs, queries.size() / (ms / 1000.0), ann::meanRecall(results, truth)};
}

}  // namespace

int main(int argc, char** argv) {
    const std::size_t n = argc > 1 ? std::strtoul(argv[1], nullptr, 10) : 50000;
    const std::size_t dim = argc > 2 ? std::strtoul(argv[2], nullptr, 10) : 32;
    const std::size_t nq = argc > 3 ? std::strtoul(argv[3], nullptr, 10) : 500;
    const std::size_t k = argc > 4 ? std::strtoul(argv[4], nullptr, 10) : 10;

    std::printf("ann benchmark: n=%zu dim=%zu queries=%zu k=%zu simd=%s\n", n, dim, nq, k,
                ann::kSimdEnabled ? "on" : "off");
    const ann::VectorSet data = ann::randomClustered(n, dim, 200, 1, 1.0f);
    const ann::VectorSet queries = ann::randomClustered(nq, dim, 200, 1, 1.0f);

    std::vector<Row> rows;

    Clock::time_point start = Clock::now();
    ann::BruteForceIndex<ann::L2> brute(dim);
    brute.add(data);
    const double bruteBuild = millisSince(start);
    std::vector<std::vector<ann::Neighbor>> truth(nq);
    for (ann::Id qi = 0; qi < nq; ++qi) {
        truth[qi] = brute.search(queries[qi], k);
    }
    rows.push_back(measure("brute-force", bruteBuild, queries, k, truth,
                           [&](const float* q, std::size_t kk) { return brute.search(q, kk); }));

    start = Clock::now();
    ann::KdTree tree(data, ann::KdTree::Params{16});
    const double treeBuild = millisSince(start);
    rows.push_back(measure("kd-tree exact", treeBuild, queries, k, truth,
                           [&](const float* q, std::size_t kk) { return tree.search(q, kk); }));
    for (float eps : {0.5f, 2.0f}) {
        ann::KdTree::SearchParams sp;
        sp.eps = eps;
        rows.push_back(measure("kd-tree eps=" + std::to_string(eps).substr(0, 3), treeBuild, queries, k, truth,
                               [&](const float* q, std::size_t kk) { return tree.search(q, kk, sp); }));
    }
    {
        ann::KdTree::SearchParams sp;
        sp.maxLeaves = 64;
        rows.push_back(measure("kd-tree 64 leaves", treeBuild, queries, k, truth,
                               [&](const float* q, std::size_t kk) { return tree.search(q, kk, sp); }));
    }

    start = Clock::now();
    ann::HnswIndex<ann::L2>::Params hp;
    hp.M = 16;
    hp.efConstruction = 100;
    ann::HnswIndex<ann::L2> hnsw(dim, hp);
    hnsw.reserve(n);
    hnsw.add(data);
    const double hnswBuild = millisSince(start);
    for (std::size_t ef : {16u, 64u, 128u}) {
        rows.push_back(measure("hnsw ef=" + std::to_string(ef), hnswBuild, queries, k, truth,
                               [&](const float* q, std::size_t kk) { return hnsw.search(q, kk, ef); }));
    }

    start = Clock::now();
    ann::IvfFlatIndex<ann::L2>::Params ip;
    ip.nlist = 256;
    ip.kmeans.iterations = 10;
    ann::IvfFlatIndex<ann::L2> ivf(dim, ip);
    ivf.build(data);
    const double ivfBuild = millisSince(start);
    for (std::size_t nprobe : {1u, 8u, 32u}) {
        rows.push_back(measure("ivf-flat nprobe=" + std::to_string(nprobe), ivfBuild, queries, k, truth,
                               [&](const float* q, std::size_t kk) { return ivf.search(q, kk, nprobe); }));
    }

    std::printf("%-22s %12s %12s %10s\n", "index", "build ms", "qps", "recall");
    for (const Row& r : rows) {
        std::printf("%-22s %12.1f %12.0f %10.4f\n", r.name.c_str(), r.buildMs, r.qps, r.recall);
    }
    return 0;
}
