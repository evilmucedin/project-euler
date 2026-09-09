# graphs — a generic graph library in modern Fortran

A small, efficient graph library (Fortran 2008) with a CSR (compressed sparse
row) core and classic algorithms on top.

## Design

- `graphs.f90` — module `graphs`: the `graph_t` type. Edges are appended to a
  dynamic buffer with `add_edge` (amortized O(1)), then `build()` converts the
  buffer into CSR adjacency in O(n + m) via counting sort. Graphs may be
  directed or undirected, weighted or unweighted. Vertices are 1-based; the
  vertex count grows automatically when `add_edge` references a new id.
- `graph_algorithms.f90` — module `graph_algorithms`:

  | procedure              | complexity  | notes                                  |
  |------------------------|-------------|----------------------------------------|
  | `bfs`                  | O(n + m)    | hop distances + parent tree            |
  | `dfs_preorder`         | O(n + m)    | iterative, no recursion                |
  | `dijkstra`             | O(m log m)  | binary min-heap with lazy deletion     |
  | `topological_sort`     | O(n + m)    | Kahn's algorithm, reports cycles       |
  | `connected_components` | O(n + m)    | undirected graphs                      |
  | `kruskal_mst`          | O(m log m)  | union-find with path halving           |
  | `build_path`           | O(path)     | reconstruct path from a parent array   |

Index and weight kinds are library-wide parameters (`gk` = int32,
`wk` = real64); switch `gk` to int64 for graphs beyond 2^31 edges.

## Usage

```fortran
use graphs
use graph_algorithms

type(graph_t) :: g
real(wk), allocatable :: dist(:)

call g%init(6_gk, directed=.true., weighted=.true.)
call g%add_edge(1_gk, 2_gk, 7.0_wk)
! ... more edges ...
call g%build()
call dijkstra(g, 1_gk, dist)
```

## Build, test, and run

```sh
make        # builds the library objects and all examples (gfortran)
make test   # builds and runs the unit tests (test_graphs.f90, 27 assertions)
make run    # runs every example
make clean
```

Platform scripts install gfortran if needed, then run `make test` and
`make run`:

```sh
./test_macos.sh    # macOS (Homebrew)
./test_ubuntu.sh   # Ubuntu (apt)
```

## Examples

- `example_shortest_paths.f90` — Dijkstra + BFS + path reconstruction on a
  weighted digraph.
- `example_components_mst.f90` — connected components and a Kruskal minimum
  spanning forest on a two-component undirected graph.
- `example_toposort.f90` — topological ordering of a task DAG and cycle
  detection.
- `example_grid_benchmark.f90` — BFS on a 1000×1000 grid (10^6 vertices,
  ~2·10^6 edges) and Dijkstra on a 500×500 randomly weighted grid, timed.
