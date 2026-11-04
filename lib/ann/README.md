# lib/ann

Header-only, dependency-free C++17 library for exact and approximate nearest
neighbour (ANN) search over float vectors. Everything lives in `namespace ann`.

```cpp
#include "lib/ann/hnsw.h"

ann::HnswIndex<ann::L2> index(/*dim=*/32);
index.add(row);                                   // const float*, returns Id
std::vector<ann::Neighbor> hits = index.search(q, /*k=*/10);
// hits[i].id, hits[i].dist (squared L2), ascending by distance
```

Buck2 target: `//lib/ann:ann`. Tests: `//lib/ann/tests/...`. Benchmark:
`buck2 run @mode/opt //lib/ann/bench:annBench -- [n] [dim] [queries] [k]`.

## Components

| Header | What it gives you |
| --- | --- |
| `common.h` | `Id`, `Neighbor`, cache-line `AlignedAllocator`, epoch-based `VisitedSet`, `VisitedPool` |
| `distance.h` | `l2Sq`, `dot`, `norm`, `normalize`; metric policies `L2`, `InnerProduct`, `Cosine` |
| `vectorSet.h` | `VectorSet`: row-major, 64-byte aligned, zero-padded storage; `AlignedQuery` |
| `topk.h` | `TopK`: bounded max-heap for the k best candidates |
| `bruteForce.h` | `BruteForceIndex<Metric>`: exact linear scan, ground truth for everything else |
| `kdTree.h` | `KdTree`: static k-d tree for L2 in low dimension, exact or `(1 + eps)`-approximate, optional leaf budget |
| `hnsw.h` | `HnswIndex<Metric>`: hierarchical navigable small world graph, thread-safe search |
| `kmeans.h` | `kmeans`: k-means++ seeding plus Lloyd iterations, `nearestCentroid` |
| `ivfFlat.h` | `IvfFlatIndex<Metric>`: inverted file with flat lists, `nprobe` recall/speed knob |
| `evaluation.h` | `recall`, `meanRecall` against ground truth |
| `synthetic.h` | `randomUniform`, `randomGaussian`, `randomClustered` datasets for tests and benchmarks |

All indexes share the same shape: construct with `dim`, `add(const float*)`
returns a dense `Id`, `search(q, k, ...)` returns `std::vector<Neighbor>` sorted
ascending. Distances are "smaller is closer": squared L2 for `L2`, `-dot` for
`InnerProduct`, `1 - cos` for `Cosine`. `Cosine` indexes normalise rows on
insert and queries on search, so the hot loop is always a dot product.

## Choosing an index

- **2 to about 20 dimensions, static data** (geometry, point clouds, spatial
  queries): `KdTree`. Exact search is already two orders of magnitude faster
  than brute force; `eps` or `maxLeaves` buys more speed when you can tolerate
  approximate results.
- **Higher dimensions or incremental inserts**: `HnswIndex`. Highest recall per
  query time; `ef` trades speed for recall at search time without rebuilding.
- **Clustered data, memory-tight, batch-built**: `IvfFlatIndex`. Cheapest build
  after the k-d tree and a linear `nprobe` knob.
- **Under a few thousand points, or to compute ground truth**: `BruteForceIndex`.

## Why it is fast

- Distance kernels use explicit AVX2/FMA or NEON intrinsics with four
  independent accumulators when the compiler targets them (`-march=native` in
  the Buck2 toolchain) and a multi-accumulator scalar loop otherwise. Define
  `ANN_NO_SIMD` to force the scalar path; the tests run both.
- Every row is padded to a multiple of 16 floats on a 64-byte boundary, so the
  kernels run over `stride()` without tail handling and rows never straddle
  cache lines. Queries are copied into the same layout once per search.
- The k-d tree stores points in leaf order so a leaf scan is one contiguous
  block, and prunes with incremental per-dimension bounds (Arya & Mount).
- HNSW keeps level-0 adjacency in one flat array, prefetches the next
  neighbour's vector while computing the current distance, and recycles
  epoch-based visited sets through a pool, so searches allocate nothing after
  warm-up and can run on many threads at once.

## Benchmark

`annBench` on an Apple M4 Pro, clang `-O2`, single thread, NEON kernels,
`randomClustered` data (200 blobs), 500 queries, k = 10:

| index | n = 50 000, dim = 32 | | n = 200 000, dim = 3 | |
| --- | ---: | ---: | ---: | ---: |
| | qps | recall@10 | qps | recall@10 |
| brute force | 6 200 | 1.000 | 1 900 | 1.000 |
| kd-tree exact | 114 000 | 1.000 | 634 000 | 1.000 |
| kd-tree eps = 0.5 | 232 000 | 1.000 | 1 030 000 | 0.975 |
| kd-tree 64 leaves | 217 000 | 0.924 | 875 000 | 1.000 |
| hnsw ef = 16 | 255 000 | 0.994 | 248 000 | 1.000 |
| hnsw ef = 64 | 90 000 | 1.000 | 76 000 | 1.000 |
| ivf-flat nprobe = 1 | 416 000 | 0.922 | 232 000 | 0.907 |
| ivf-flat nprobe = 8 | 102 000 | 1.000 | 44 000 | 1.000 |

Build times for the 50 000 x 32 set: k-d tree 10 ms, IVF-Flat 270 ms, HNSW
850 ms (M = 16, efConstruction = 100).

## Thread safety

`search` on any index is safe to call from many threads at once as long as no
thread is calling `add`, `build` or `train`. Mutating calls are not
synchronised; wrap them in your own lock or build first and query afterwards.

## Non-goals (so far)

Serialisation, deletes, quantised (PQ/SQ) storage and multi-threaded HNSW
construction are not implemented.
