!> Example: connected components and a minimum spanning forest of an
!> undirected weighted graph with two components.
program example_components_mst
  use graphs
  use graph_algorithms
  implicit none

  type(graph_t) :: g
  integer(gk), allocatable :: comp(:), mst_edges(:)
  integer(gk) :: ncomp, nmst, v, i, e
  real(wk) :: total

  call g%init(9_gk, directed=.false., weighted=.true.)
  ! Component A: vertices 1..6 (a weighted mesh).
  call g%add_edge(1_gk, 2_gk, 4.0_wk)
  call g%add_edge(1_gk, 3_gk, 3.0_wk)
  call g%add_edge(2_gk, 3_gk, 1.0_wk)
  call g%add_edge(2_gk, 4_gk, 2.0_wk)
  call g%add_edge(3_gk, 4_gk, 4.0_wk)
  call g%add_edge(4_gk, 5_gk, 2.0_wk)
  call g%add_edge(5_gk, 6_gk, 1.0_wk)
  call g%add_edge(4_gk, 6_gk, 7.0_wk)
  ! Component B: vertices 7..9 (a triangle).
  call g%add_edge(7_gk, 8_gk, 5.0_wk)
  call g%add_edge(8_gk, 9_gk, 6.0_wk)
  call g%add_edge(7_gk, 9_gk, 2.0_wk)
  call g%build()

  print '(a)', "== Connected components =="
  call connected_components(g, comp, ncomp)
  print '(a,i0)', "  number of components: ", ncomp
  do v = 1, g%num_vertices()
    print '(a,i0,a,i0)', "  vertex ", v, " -> component ", comp(v)
  end do

  print '(a)', "== Kruskal minimum spanning forest =="
  call kruskal_mst(g, mst_edges, nmst, total)
  print '(a,i0,a,f6.1)', "  edges chosen: ", nmst, ", total weight: ", total
  do i = 1, nmst
    e = mst_edges(i)
    print '(a,i0,a,i0,a,f5.1,a)', "  edge ", g%esrc(e), " -- ", g%edst(e), &
      "  (w = ", g%ewgt(e), ")"
  end do
end program example_components_mst
