!> Example: topological sort of a task-dependency DAG, plus cycle detection.
program example_toposort
  use graphs
  use graph_algorithms
  implicit none

  character(len=12), parameter :: task_names(8) = [character(len=12) :: &
    "fetch", "parse", "typecheck", "codegen", "link", "test", "package", "publish"]
  type(graph_t) :: g
  integer(gk), allocatable :: order(:)
  integer(gk) :: i, norder
  logical :: ok

  ! Edge u -> v means "u must run before v".
  call g%init(8_gk, directed=.true.)
  call g%add_edge(1_gk, 2_gk)  ! fetch     -> parse
  call g%add_edge(2_gk, 3_gk)  ! parse     -> typecheck
  call g%add_edge(3_gk, 4_gk)  ! typecheck -> codegen
  call g%add_edge(4_gk, 5_gk)  ! codegen   -> link
  call g%add_edge(5_gk, 6_gk)  ! link      -> test
  call g%add_edge(5_gk, 7_gk)  ! link      -> package
  call g%add_edge(6_gk, 8_gk)  ! test      -> publish
  call g%add_edge(7_gk, 8_gk)  ! package   -> publish
  call g%build()

  print '(a)', "== Topological order of build tasks =="
  call topological_sort(g, order, ok)
  if (ok) then
    do i = 1, g%num_vertices()
      print '(a,i0,a,a)', "  step ", i, ": ", trim(task_names(order(i)))
    end do
  end if

  ! Now a graph with a cycle: 1 -> 2 -> 3 -> 1.
  call g%init(3_gk, directed=.true.)
  call g%add_edge(1_gk, 2_gk)
  call g%add_edge(2_gk, 3_gk)
  call g%add_edge(3_gk, 1_gk)
  call g%build()

  print '(a)', "== Cycle detection =="
  call topological_sort(g, order, ok, norder)
  if (.not. ok) then
    print '(a,i0,a)', "  cycle detected: only ", norder, " of 3 vertices could be ordered"
  end if
end program example_toposort
