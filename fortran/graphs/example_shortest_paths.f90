!> Example: shortest paths on a small weighted directed graph.
!> Runs Dijkstra for weighted distances and BFS for hop counts, then
!> reconstructs the shortest weighted path from vertex 1 to vertex 6.
program example_shortest_paths
  use graphs
  use graph_algorithms
  implicit none

  type(graph_t) :: g
  real(wk), allocatable :: dist(:)
  integer(gk), allocatable :: parent(:), hops(:), path(:)
  integer(gk) :: v, npath

  ! A classic 6-vertex weighted digraph.
  call g%init(6_gk, directed=.true., weighted=.true.)
  call g%add_edge(1_gk, 2_gk, 7.0_wk)
  call g%add_edge(1_gk, 3_gk, 9.0_wk)
  call g%add_edge(1_gk, 6_gk, 14.0_wk)
  call g%add_edge(2_gk, 3_gk, 10.0_wk)
  call g%add_edge(2_gk, 4_gk, 15.0_wk)
  call g%add_edge(3_gk, 4_gk, 11.0_wk)
  call g%add_edge(3_gk, 6_gk, 2.0_wk)
  call g%add_edge(4_gk, 5_gk, 6.0_wk)
  call g%add_edge(6_gk, 5_gk, 9.0_wk)
  call g%build()

  print '(a)', "== Dijkstra from vertex 1 =="
  call dijkstra(g, 1_gk, dist, parent)
  do v = 1, g%num_vertices()
    if (dist(v) < GRAPH_INF) then
      print '(a,i0,a,f6.1)', "  dist(1 -> ", v, ") = ", dist(v)
    else
      print '(a,i0,a)', "  dist(1 -> ", v, ") = unreachable"
    end if
  end do

  call build_path(parent, 1_gk, 5_gk, path, npath)
  write (*, '(a)', advance='no') "  shortest path 1 -> 5:"
  do v = 1, npath
    write (*, '(a,i0)', advance='no') " ", path(v)
  end do
  print '(a,f6.1,a)', "   (weight", dist(5), ")"

  print '(a)', "== BFS hop counts from vertex 1 =="
  call bfs(g, 1_gk, hops)
  do v = 1, g%num_vertices()
    print '(a,i0,a,i0)', "  hops(1 -> ", v, ") = ", hops(v)
  end do
end program example_shortest_paths
