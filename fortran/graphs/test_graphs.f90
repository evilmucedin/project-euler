!> Unit tests for the graphs library. Exits with a nonzero status via
!> error stop on the first failure; prints "ALL TESTS PASSED" on success.
program test_graphs
  use graphs
  use graph_algorithms
  implicit none

  integer :: ntests = 0

  call test_dijkstra_classic()
  call test_bfs_and_unreachable()
  call test_dfs_preorder()
  call test_toposort_dag()
  call test_toposort_cycle()
  call test_components()
  call test_kruskal()
  call test_autogrow_and_degree()
  call test_empty_graph()

  print '(a,i0,a)', "ALL TESTS PASSED (", ntests, " tests)"

contains

  subroutine check(cond, label)
    logical, intent(in) :: cond
    character(len=*), intent(in) :: label
    ntests = ntests + 1
    if (cond) then
      print '(a,a)', "  ok: ", label
    else
      print '(a,a)', "  FAILED: ", label
      error stop 1
    end if
  end subroutine check

  subroutine test_dijkstra_classic()
    type(graph_t) :: g
    real(wk), allocatable :: dist(:)
    integer(gk), allocatable :: parent(:), path(:)
    integer(gk) :: npath

    print '(a)', "test_dijkstra_classic"
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
    call dijkstra(g, 1_gk, dist, parent)
    call check(abs(dist(5) - 20.0_wk) < 1e-9_wk, "dist(1->5) == 20")
    call check(abs(dist(6) - 11.0_wk) < 1e-9_wk, "dist(1->6) == 11")
    call check(abs(dist(1)) < 1e-9_wk, "dist(1->1) == 0")
    call build_path(parent, 1_gk, 5_gk, path, npath)
    call check(npath == 4, "path 1->5 has 4 vertices")
    call check(all(path == [1_gk, 3_gk, 6_gk, 5_gk]), "path 1->5 is 1,3,6,5")
  end subroutine test_dijkstra_classic

  subroutine test_bfs_and_unreachable()
    type(graph_t) :: g
    integer(gk), allocatable :: dist(:)

    print '(a)', "test_bfs_and_unreachable"
    ! 1 -> 2 -> 3, vertex 4 isolated.
    call g%init(4_gk, directed=.true.)
    call g%add_edge(1_gk, 2_gk)
    call g%add_edge(2_gk, 3_gk)
    call g%build()
    call bfs(g, 1_gk, dist)
    call check(dist(1) == 0, "bfs dist(1) == 0")
    call check(dist(2) == 1, "bfs dist(2) == 1")
    call check(dist(3) == 2, "bfs dist(3) == 2")
    call check(dist(4) == -1, "bfs dist(4) == -1 (unreachable)")
  end subroutine test_bfs_and_unreachable

  subroutine test_dfs_preorder()
    type(graph_t) :: g
    integer(gk), allocatable :: order(:)
    integer(gk) :: norder

    print '(a)', "test_dfs_preorder"
    ! 1 -> {2, 4}, 2 -> 3. Preorder visiting low neighbors first: 1 2 3 4.
    call g%init(4_gk, directed=.true.)
    call g%add_edge(1_gk, 2_gk)
    call g%add_edge(1_gk, 4_gk)
    call g%add_edge(2_gk, 3_gk)
    call g%build()
    call dfs_preorder(g, 1_gk, order, norder)
    call check(norder == 4, "dfs reaches 4 vertices")
    call check(all(order(1:4) == [1_gk, 2_gk, 3_gk, 4_gk]), "dfs preorder is 1,2,3,4")
  end subroutine test_dfs_preorder

  subroutine test_toposort_dag()
    type(graph_t) :: g
    integer(gk), allocatable :: order(:), pos(:)
    integer(gk) :: i
    logical :: ok

    print '(a)', "test_toposort_dag"
    call g%init(6_gk, directed=.true.)
    call g%add_edge(1_gk, 3_gk)
    call g%add_edge(2_gk, 3_gk)
    call g%add_edge(3_gk, 4_gk)
    call g%add_edge(3_gk, 5_gk)
    call g%add_edge(4_gk, 6_gk)
    call g%add_edge(5_gk, 6_gk)
    call g%build()
    call topological_sort(g, order, ok)
    call check(ok, "toposort succeeds on a DAG")
    allocate (pos(g%num_vertices()))
    do i = 1, g%num_vertices()
      pos(order(i)) = i
    end do
    ! Every edge must go forward in the order.
    call check(all([(pos(g%esrc(i)) < pos(g%edst(i)), i=1, g%num_edges())]), &
               "every edge respects the topological order")
  end subroutine test_toposort_dag

  subroutine test_toposort_cycle()
    type(graph_t) :: g
    integer(gk), allocatable :: order(:)
    integer(gk) :: norder
    logical :: ok

    print '(a)', "test_toposort_cycle"
    call g%init(3_gk, directed=.true.)
    call g%add_edge(1_gk, 2_gk)
    call g%add_edge(2_gk, 3_gk)
    call g%add_edge(3_gk, 1_gk)
    call g%build()
    call topological_sort(g, order, ok, norder)
    call check(.not. ok, "toposort reports a cycle")
    call check(norder == 0, "no vertex of a pure cycle can be ordered")
  end subroutine test_toposort_cycle

  subroutine test_components()
    type(graph_t) :: g
    integer(gk), allocatable :: comp(:)
    integer(gk) :: ncomp

    print '(a)', "test_components"
    ! {1,2,3} connected, {4,5} connected, 6 isolated.
    call g%init(6_gk, directed=.false.)
    call g%add_edge(1_gk, 2_gk)
    call g%add_edge(2_gk, 3_gk)
    call g%add_edge(4_gk, 5_gk)
    call g%build()
    call connected_components(g, comp, ncomp)
    call check(ncomp == 3, "3 components found")
    call check(comp(1) == comp(2) .and. comp(2) == comp(3), "1,2,3 share a component")
    call check(comp(4) == comp(5), "4,5 share a component")
    call check(comp(6) /= comp(1) .and. comp(6) /= comp(4), "6 is its own component")
  end subroutine test_components

  subroutine test_kruskal()
    type(graph_t) :: g
    integer(gk), allocatable :: mst_edges(:)
    integer(gk) :: nmst
    real(wk) :: total

    print '(a)', "test_kruskal"
    ! Square 1-2-3-4 with cheap sides (1) and expensive diagonals (10).
    call g%init(4_gk, directed=.false., weighted=.true.)
    call g%add_edge(1_gk, 2_gk, 1.0_wk)
    call g%add_edge(2_gk, 3_gk, 1.0_wk)
    call g%add_edge(3_gk, 4_gk, 1.0_wk)
    call g%add_edge(4_gk, 1_gk, 1.0_wk)
    call g%add_edge(1_gk, 3_gk, 10.0_wk)
    call g%add_edge(2_gk, 4_gk, 10.0_wk)
    call g%build()
    call kruskal_mst(g, mst_edges, nmst, total)
    call check(nmst == 3, "spanning tree has n-1 edges")
    call check(abs(total - 3.0_wk) < 1e-9_wk, "MST weight == 3")
  end subroutine test_kruskal

  subroutine test_autogrow_and_degree()
    type(graph_t) :: g
    integer(gk) :: i

    print '(a)', "test_autogrow_and_degree"
    ! Start empty; vertex count grows from edge ids. Star: 1 -> 2..50.
    call g%init(0_gk, directed=.true.)
    do i = 2, 50
      call g%add_edge(1_gk, i)
    end do
    call g%build()
    call check(g%num_vertices() == 50, "vertex count auto-grows to 50")
    call check(g%num_edges() == 49, "49 edges stored")
    call check(g%degree(1_gk) == 49, "degree(1) == 49")
    call check(g%degree(2_gk) == 0, "degree(2) == 0")
  end subroutine test_autogrow_and_degree

  subroutine test_empty_graph()
    type(graph_t) :: g
    integer(gk), allocatable :: dist(:)

    print '(a)', "test_empty_graph"
    call g%init(3_gk, directed=.false.)
    call g%build()
    call bfs(g, 2_gk, dist)
    call check(dist(2) == 0, "edgeless graph: source dist is 0")
    call check(dist(1) == -1 .and. dist(3) == -1, "edgeless graph: others unreachable")
  end subroutine test_empty_graph

end program test_graphs
