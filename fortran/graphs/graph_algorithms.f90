!> graph_algorithms: classic graph algorithms on top of the graphs module.
!>
!>   bfs                   O(n + m)        hop distances + parents
!>   dfs_preorder          O(n + m)        iterative preorder traversal
!>   dijkstra              O(m log m)      shortest weighted paths (lazy heap)
!>   topological_sort      O(n + m)        Kahn's algorithm, detects cycles
!>   connected_components  O(n + m)        undirected graphs
!>   kruskal_mst           O(m log m)      minimum spanning forest, union-find
!>   build_path            O(path length)  reconstruct a path from parents
module graph_algorithms
  use graphs, only: graph_t, gk, wk, GRAPH_INF
  implicit none
  private

  public :: bfs, dfs_preorder, dijkstra, topological_sort
  public :: connected_components, kruskal_mst, build_path

contains

  !> Breadth-first search from source. dist(v) is the hop count from source
  !> (-1 when unreachable); parent(v) is the BFS-tree predecessor (0 for the
  !> source and unreachable vertices).
  subroutine bfs(g, source, dist, parent)
    type(graph_t), intent(in) :: g
    integer(gk), intent(in) :: source
    integer(gk), allocatable, intent(out) :: dist(:)
    integer(gk), allocatable, intent(out), optional :: parent(:)
    integer(gk), allocatable :: queue(:), par(:)
    integer(gk) :: head, tail, u, v, i

    call g%require_built()
    allocate (dist(g%n), queue(g%n), par(g%n))
    dist = -1
    par = 0
    dist(source) = 0
    queue(1) = source
    head = 1
    tail = 1
    do while (head <= tail)
      u = queue(head)
      head = head + 1
      do i = g%xadj(u), g%xadj(u + 1) - 1
        v = g%adjncy(i)
        if (dist(v) < 0) then
          dist(v) = dist(u) + 1
          par(v) = u
          tail = tail + 1
          queue(tail) = v
        end if
      end do
    end do
    if (present(parent)) call move_alloc(par, parent)
  end subroutine bfs

  !> Iterative depth-first preorder from source. order(1:norder) lists the
  !> vertices reachable from source in visit order.
  subroutine dfs_preorder(g, source, order, norder)
    type(graph_t), intent(in) :: g
    integer(gk), intent(in) :: source
    integer(gk), allocatable, intent(out) :: order(:)
    integer(gk), intent(out) :: norder
    integer(gk), allocatable :: stack(:)
    logical, allocatable :: visited(:)
    integer(gk) :: top, u, i

    call g%require_built()
    allocate (order(g%n), visited(g%n))
    ! Worst case every edge pushes one vertex, plus the source itself.
    allocate (stack(size(g%adjncy) + 1))
    visited = .false.
    norder = 0
    top = 1
    stack(1) = source
    do while (top > 0)
      u = stack(top)
      top = top - 1
      if (visited(u)) cycle
      visited(u) = .true.
      norder = norder + 1
      order(norder) = u
      ! Push neighbors in reverse so lower-numbered neighbors are visited first.
      do i = g%xadj(u + 1) - 1, g%xadj(u), -1
        if (.not. visited(g%adjncy(i))) then
          top = top + 1
          stack(top) = g%adjncy(i)
        end if
      end do
    end do
  end subroutine dfs_preorder

  !> Dijkstra shortest paths from source on a weighted graph with
  !> non-negative weights. dist(v) = GRAPH_INF when unreachable; parent(v) is
  !> the shortest-path-tree predecessor (0 for source/unreachable).
  !> Uses a binary min-heap with lazy deletion: O(m log m).
  subroutine dijkstra(g, source, dist, parent)
    type(graph_t), intent(in) :: g
    integer(gk), intent(in) :: source
    real(wk), allocatable, intent(out) :: dist(:)
    integer(gk), allocatable, intent(out), optional :: parent(:)
    integer(gk), allocatable :: par(:), hvert(:)
    real(wk), allocatable :: hkey(:)
    logical, allocatable :: done(:)
    integer(gk) :: hsize, u, v, i
    real(wk) :: du, cand

    call g%require_built()
    if (.not. g%weighted) error stop "graph_algorithms: dijkstra requires a weighted graph"
    allocate (dist(g%n), par(g%n), done(g%n))
    dist = GRAPH_INF
    par = 0
    done = .false.
    allocate (hkey(64), hvert(64))
    hsize = 0
    dist(source) = 0.0_wk
    call heap_push(0.0_wk, source)
    do while (hsize > 0)
      call heap_pop(du, u)
      if (done(u)) cycle
      done(u) = .true.
      do i = g%xadj(u), g%xadj(u + 1) - 1
        v = g%adjncy(i)
        if (g%adjwgt(i) < 0.0_wk) error stop "graph_algorithms: dijkstra requires non-negative weights"
        cand = du + g%adjwgt(i)
        if (cand < dist(v)) then
          dist(v) = cand
          par(v) = u
          call heap_push(cand, v)
        end if
      end do
    end do
    if (present(parent)) call move_alloc(par, parent)

  contains

    subroutine heap_push(key, vert)
      real(wk), intent(in) :: key
      integer(gk), intent(in) :: vert
      integer(gk), allocatable :: vtmp(:)
      real(wk), allocatable :: ktmp(:)
      integer(gk) :: child, p

      if (hsize == size(hkey)) then
        allocate (ktmp(2*hsize), vtmp(2*hsize))
        ktmp(1:hsize) = hkey
        vtmp(1:hsize) = hvert
        call move_alloc(ktmp, hkey)
        call move_alloc(vtmp, hvert)
      end if
      hsize = hsize + 1
      child = hsize
      do while (child > 1)
        p = child/2
        if (hkey(p) <= key) exit
        hkey(child) = hkey(p)
        hvert(child) = hvert(p)
        child = p
      end do
      hkey(child) = key
      hvert(child) = vert
    end subroutine heap_push

    subroutine heap_pop(key, vert)
      real(wk), intent(out) :: key
      integer(gk), intent(out) :: vert
      real(wk) :: lastk
      integer(gk) :: lastv, p, child

      key = hkey(1)
      vert = hvert(1)
      lastk = hkey(hsize)
      lastv = hvert(hsize)
      hsize = hsize - 1
      p = 1
      do
        child = 2*p
        if (child > hsize) exit
        if (child < hsize) then
          if (hkey(child + 1) < hkey(child)) child = child + 1
        end if
        if (hkey(child) >= lastk) exit
        hkey(p) = hkey(child)
        hvert(p) = hvert(child)
        p = child
      end do
      if (hsize > 0) then
        hkey(p) = lastk
        hvert(p) = lastv
      end if
    end subroutine heap_pop

  end subroutine dijkstra

  !> Kahn's topological sort of a directed graph. On success ok = .true. and
  !> order(1:n) is a topological order; ok = .false. means the graph has a
  !> cycle (order then holds the partial order of acyclic vertices).
  subroutine topological_sort(g, order, ok, norder)
    type(graph_t), intent(in) :: g
    integer(gk), allocatable, intent(out) :: order(:)
    logical, intent(out) :: ok
    integer(gk), intent(out), optional :: norder
    integer(gk), allocatable :: indeg(:)
    integer(gk) :: head, tail, u, v, i

    call g%require_built()
    if (.not. g%directed) error stop "graph_algorithms: topological_sort requires a directed graph"
    allocate (order(g%n), indeg(g%n))
    indeg = 0
    do i = 1, g%xadj(g%n + 1) - 1
      indeg(g%adjncy(i)) = indeg(g%adjncy(i)) + 1
    end do
    tail = 0
    do u = 1, g%n
      if (indeg(u) == 0) then
        tail = tail + 1
        order(tail) = u
      end if
    end do
    head = 1
    do while (head <= tail)
      u = order(head)
      head = head + 1
      do i = g%xadj(u), g%xadj(u + 1) - 1
        v = g%adjncy(i)
        indeg(v) = indeg(v) - 1
        if (indeg(v) == 0) then
          tail = tail + 1
          order(tail) = v
        end if
      end do
    end do
    ok = (tail == g%n)
    if (present(norder)) norder = tail
  end subroutine topological_sort

  !> Label connected components of an undirected graph. comp(v) is the
  !> component id in 1..ncomp.
  subroutine connected_components(g, comp, ncomp)
    type(graph_t), intent(in) :: g
    integer(gk), allocatable, intent(out) :: comp(:)
    integer(gk), intent(out) :: ncomp
    integer(gk), allocatable :: queue(:)
    integer(gk) :: head, tail, s, u, v, i

    call g%require_built()
    if (g%directed) error stop "graph_algorithms: connected_components requires an undirected graph"
    allocate (comp(g%n), queue(g%n))
    comp = 0
    ncomp = 0
    do s = 1, g%n
      if (comp(s) /= 0) cycle
      ncomp = ncomp + 1
      comp(s) = ncomp
      queue(1) = s
      head = 1
      tail = 1
      do while (head <= tail)
        u = queue(head)
        head = head + 1
        do i = g%xadj(u), g%xadj(u + 1) - 1
          v = g%adjncy(i)
          if (comp(v) == 0) then
            comp(v) = ncomp
            tail = tail + 1
            queue(tail) = v
          end if
        end do
      end do
    end do
  end subroutine connected_components

  !> Kruskal minimum spanning forest of an undirected weighted graph.
  !> edge_index(1:nmst) are indices into the graph's original edge list
  !> (g%esrc/g%edst/g%ewgt); total is the summed weight. For a connected
  !> graph nmst = n - 1 (a spanning tree), otherwise one tree per component.
  subroutine kruskal_mst(g, edge_index, nmst, total)
    type(graph_t), intent(in) :: g
    integer(gk), allocatable, intent(out) :: edge_index(:)
    integer(gk), intent(out) :: nmst
    real(wk), intent(out) :: total
    integer(gk), allocatable :: idx(:), ufparent(:)
    integer(gk) :: i, e, ru, rv

    if (g%directed) error stop "graph_algorithms: kruskal_mst requires an undirected graph"
    if (.not. g%weighted) error stop "graph_algorithms: kruskal_mst requires a weighted graph"
    allocate (edge_index(max(g%n - 1, 1_gk)), idx(g%nedges), ufparent(g%n))
    do i = 1, g%nedges
      idx(i) = i
    end do
    call heapsort_by_weight(idx)
    do i = 1, g%n
      ufparent(i) = i
    end do
    nmst = 0
    total = 0.0_wk
    do i = 1, g%nedges
      e = idx(i)
      ru = uf_find(g%esrc(e))
      rv = uf_find(g%edst(e))
      if (ru /= rv) then
        ufparent(ru) = rv
        nmst = nmst + 1
        edge_index(nmst) = e
        total = total + g%ewgt(e)
        if (nmst == g%n - 1) exit
      end if
    end do

  contains

    integer(gk) function uf_find(x) result(root)
      integer(gk), intent(in) :: x
      root = x
      do while (ufparent(root) /= root)
        ufparent(root) = ufparent(ufparent(root))  ! path halving
        root = ufparent(root)
      end do
    end function uf_find

    !> In-place heapsort of idx so that g%ewgt(idx(:)) is non-decreasing.
    subroutine heapsort_by_weight(a)
      integer(gk), intent(inout) :: a(:)
      integer(gk) :: nn, k, tmp

      nn = size(a, kind=gk)
      do k = nn/2, 1, -1
        call sift_down(a, k, nn)
      end do
      do k = nn, 2, -1
        tmp = a(1)
        a(1) = a(k)
        a(k) = tmp
        call sift_down(a, 1_gk, k - 1)
      end do
    end subroutine heapsort_by_weight

    subroutine sift_down(a, start, last)
      integer(gk), intent(inout) :: a(:)
      integer(gk), intent(in) :: start, last
      integer(gk) :: root, child, tmp

      root = start
      do
        child = 2*root
        if (child > last) exit
        if (child < last) then
          if (g%ewgt(a(child + 1)) > g%ewgt(a(child))) child = child + 1
        end if
        if (g%ewgt(a(child)) <= g%ewgt(a(root))) exit
        tmp = a(root)
        a(root) = a(child)
        a(child) = tmp
        root = child
      end do
    end subroutine sift_down

  end subroutine kruskal_mst

  !> Reconstruct the path ending at target from a parent array produced by
  !> bfs or dijkstra. path(1:npath) runs source -> target; npath = 0 when
  !> target has no recorded predecessor chain (unreachable, non-source).
  subroutine build_path(parent, source, target, path, npath)
    integer(gk), intent(in) :: parent(:)
    integer(gk), intent(in) :: source, target
    integer(gk), allocatable, intent(out) :: path(:)
    integer(gk), intent(out) :: npath
    integer(gk) :: v, k

    ! Count path length by walking predecessors.
    npath = 0
    if (target /= source .and. parent(target) == 0) then
      allocate (path(0))
      return
    end if
    v = target
    npath = 1
    do while (v /= source)
      v = parent(v)
      npath = npath + 1
    end do
    allocate (path(npath))
    v = target
    do k = npath, 1, -1
      path(k) = v
      if (k > 1) v = parent(v)
    end do
  end subroutine build_path

end module graph_algorithms
