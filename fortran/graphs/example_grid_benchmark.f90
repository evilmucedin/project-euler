!> Example/benchmark: the library on large grid graphs.
!>   - BFS over a 1000 x 1000 four-connected grid (1e6 vertices, ~2e6 edges)
!>   - Dijkstra over a 500 x 500 grid with pseudo-random weights
program example_grid_benchmark
  use, intrinsic :: iso_fortran_env, only: int64
  use graphs
  use graph_algorithms
  implicit none

  call bench_bfs(1000_gk, 1000_gk)
  call bench_dijkstra(500_gk, 500_gk)

contains

  integer(gk) function vid(i, j, ncols) result(v)
    integer(gk), intent(in) :: i, j, ncols
    v = (i - 1)*ncols + j
  end function vid

  subroutine bench_bfs(nrows, ncols)
    integer(gk), intent(in) :: nrows, ncols
    type(graph_t) :: g
    integer(gk), allocatable :: dist(:)
    integer(gk) :: i, j
    integer(int64) :: t0, t1, rate

    call g%init(nrows*ncols, directed=.false.)
    do i = 1, nrows
      do j = 1, ncols
        if (j < ncols) call g%add_edge(vid(i, j, ncols), vid(i, j + 1, ncols))
        if (i < nrows) call g%add_edge(vid(i, j, ncols), vid(i + 1, j, ncols))
      end do
    end do
    call g%build()

    call system_clock(t0, rate)
    call bfs(g, 1_gk, dist)
    call system_clock(t1)

    print '(a,i0,a,i0,a)', "== BFS on ", nrows, " x ", ncols, " grid =="
    print '(a,i0,a,i0)', "  vertices: ", g%num_vertices(), ", edges: ", g%num_edges()
    print '(a,i0,a,i0,a)', "  distance corner -> corner: ", &
      dist(vid(nrows, ncols, ncols)), "  (expected ", nrows + ncols - 2, ")"
    print '(a,f8.3,a)', "  BFS time: ", real(t1 - t0)/real(rate), " s"
  end subroutine bench_bfs

  subroutine bench_dijkstra(nrows, ncols)
    integer(gk), intent(in) :: nrows, ncols
    type(graph_t) :: g
    real(wk), allocatable :: dist(:)
    real(wk) :: w
    integer(gk) :: i, j
    integer(int64) :: t0, t1, rate

    ! Deterministic pseudo-random weights in [1, 10).
    call seed_rng(42)
    call g%init(nrows*ncols, directed=.false., weighted=.true.)
    do i = 1, nrows
      do j = 1, ncols
        if (j < ncols) then
          call random_number(w)
          call g%add_edge(vid(i, j, ncols), vid(i, j + 1, ncols), 1.0_wk + 9.0_wk*w)
        end if
        if (i < nrows) then
          call random_number(w)
          call g%add_edge(vid(i, j, ncols), vid(i + 1, j, ncols), 1.0_wk + 9.0_wk*w)
        end if
      end do
    end do
    call g%build()

    call system_clock(t0, rate)
    call dijkstra(g, 1_gk, dist)
    call system_clock(t1)

    print '(a,i0,a,i0,a)', "== Dijkstra on ", nrows, " x ", ncols, " weighted grid =="
    print '(a,i0,a,i0)', "  vertices: ", g%num_vertices(), ", edges: ", g%num_edges()
    print '(a,f10.2)', "  weighted distance corner -> corner: ", dist(vid(nrows, ncols, ncols))
    print '(a,f8.3,a)', "  Dijkstra time: ", real(t1 - t0)/real(rate), " s"
  end subroutine bench_dijkstra

  subroutine seed_rng(s)
    integer, intent(in) :: s
    integer :: nseed, k
    integer, allocatable :: seed(:)

    call random_seed(size=nseed)
    allocate (seed(nseed))
    seed = [(s + 37*k, k=1, nseed)]
    call random_seed(put=seed)
  end subroutine seed_rng

end program example_grid_benchmark
