!> graphs: a generic, efficient graph library in modern Fortran.
!>
!> Storage model
!>   Edges are appended to a dynamic edge buffer with add_edge (amortized O(1)),
!>   then build() converts the buffer into a compressed sparse row (CSR)
!>   adjacency structure in O(n + m) via counting sort. After build() the
!>   neighbors of vertex v are adjncy(xadj(v) : xadj(v+1)-1) and, for weighted
!>   graphs, the matching weights are adjwgt(...) over the same range.
!>
!> Conventions
!>   - Vertices are 1-based integers of kind gk.
!>   - Graphs may be directed or undirected (undirected edges are stored in
!>     both directions in the CSR structure, once in the edge buffer).
!>   - Weights are of kind wk; unweighted graphs skip weight storage entirely.
!>   - The original edge buffer is kept after build() so that edge-list
!>     algorithms (e.g. Kruskal MST) can run without duplicating edges.
module graphs
  use, intrinsic :: iso_fortran_env, only: int32, real64
  implicit none
  private

  !> Kind used for vertex ids and indices. Change to int64 for huge graphs.
  integer, parameter, public :: gk = int32
  !> Kind used for edge weights.
  integer, parameter, public :: wk = real64
  !> Sentinel for "unreachable" distances in weighted algorithms.
  real(wk), parameter, public :: GRAPH_INF = huge(1.0_wk)

  type, public :: graph_t
    integer(gk) :: n = 0            !< number of vertices
    integer(gk) :: nedges = 0       !< number of edges in the input buffer
    logical :: directed = .true.
    logical :: weighted = .false.
    logical :: built = .false.
    ! Input edge buffer (kept after build for edge-list algorithms).
    integer(gk), allocatable :: esrc(:)
    integer(gk), allocatable :: edst(:)
    real(wk),    allocatable :: ewgt(:)
    ! CSR adjacency, valid only after build().
    integer(gk), allocatable :: xadj(:)    !< size n+1, 1-based offsets
    integer(gk), allocatable :: adjncy(:)  !< neighbor lists
    real(wk),    allocatable :: adjwgt(:)  !< weights parallel to adjncy
  contains
    procedure :: init => graph_init
    procedure :: add_edge => graph_add_edge
    procedure :: build => graph_build
    procedure :: num_vertices => graph_num_vertices
    procedure :: num_edges => graph_num_edges
    procedure :: degree => graph_degree
    procedure :: require_built => graph_require_built
  end type graph_t

contains

  !> Initialize a graph with n vertices (more are added automatically if
  !> add_edge references a larger vertex id).
  subroutine graph_init(this, n, directed, weighted)
    class(graph_t), intent(out) :: this
    integer(gk), intent(in) :: n
    logical, intent(in), optional :: directed
    logical, intent(in), optional :: weighted

    if (n < 0) error stop "graphs: init: n must be non-negative"
    this%n = n
    if (present(directed)) this%directed = directed
    if (present(weighted)) this%weighted = weighted
    allocate (this%esrc(16), this%edst(16))
    if (this%weighted) allocate (this%ewgt(16))
  end subroutine graph_init

  !> Append an edge u -> v (or u -- v for undirected graphs).
  !> For weighted graphs the weight defaults to 1 when omitted.
  subroutine graph_add_edge(this, u, v, w)
    class(graph_t), intent(inout) :: this
    integer(gk), intent(in) :: u, v
    real(wk), intent(in), optional :: w

    if (this%built) error stop "graphs: add_edge called after build"
    if (u < 1 .or. v < 1) error stop "graphs: vertex ids must be >= 1"
    this%n = max(this%n, u, v)
    if (this%nedges == size(this%esrc)) call grow_edge_buffer(this)
    this%nedges = this%nedges + 1
    this%esrc(this%nedges) = u
    this%edst(this%nedges) = v
    if (this%weighted) then
      if (present(w)) then
        this%ewgt(this%nedges) = w
      else
        this%ewgt(this%nedges) = 1.0_wk
      end if
    end if
  end subroutine graph_add_edge

  !> Convert the edge buffer into CSR adjacency in O(n + m).
  subroutine graph_build(this)
    class(graph_t), intent(inout) :: this
    integer(gk), allocatable :: cursor(:)
    integer(gk) :: i, u, v, pos, mm

    if (this%built) return
    mm = this%nedges
    if (.not. this%directed) mm = 2*this%nedges
    allocate (this%xadj(this%n + 1))
    allocate (this%adjncy(max(mm, 1_gk)))
    if (this%weighted) allocate (this%adjwgt(max(mm, 1_gk)))

    allocate (cursor(max(this%n, 1_gk)))
    cursor = 0
    do i = 1, this%nedges
      cursor(this%esrc(i)) = cursor(this%esrc(i)) + 1
      if (.not. this%directed) cursor(this%edst(i)) = cursor(this%edst(i)) + 1
    end do
    this%xadj(1) = 1
    do i = 1, this%n
      this%xadj(i + 1) = this%xadj(i) + cursor(i)
    end do
    cursor(1:this%n) = this%xadj(1:this%n)
    do i = 1, this%nedges
      u = this%esrc(i)
      v = this%edst(i)
      pos = cursor(u)
      this%adjncy(pos) = v
      if (this%weighted) this%adjwgt(pos) = this%ewgt(i)
      cursor(u) = pos + 1
      if (.not. this%directed) then
        pos = cursor(v)
        this%adjncy(pos) = u
        if (this%weighted) this%adjwgt(pos) = this%ewgt(i)
        cursor(v) = pos + 1
      end if
    end do
    this%built = .true.
  end subroutine graph_build

  pure integer(gk) function graph_num_vertices(this) result(n)
    class(graph_t), intent(in) :: this
    n = this%n
  end function graph_num_vertices

  pure integer(gk) function graph_num_edges(this) result(m)
    class(graph_t), intent(in) :: this
    m = this%nedges
  end function graph_num_edges

  !> Out-degree of v (total degree for undirected graphs). Requires build().
  integer(gk) function graph_degree(this, v) result(d)
    class(graph_t), intent(in) :: this
    integer(gk), intent(in) :: v
    call this%require_built()
    d = this%xadj(v + 1) - this%xadj(v)
  end function graph_degree

  subroutine graph_require_built(this)
    class(graph_t), intent(in) :: this
    if (.not. this%built) error stop "graphs: call build() before querying adjacency"
  end subroutine graph_require_built

  subroutine grow_edge_buffer(this)
    class(graph_t), intent(inout) :: this
    integer(gk), allocatable :: itmp(:)
    real(wk), allocatable :: rtmp(:)
    integer(gk) :: cap

    cap = 2*size(this%esrc)
    allocate (itmp(cap))
    itmp(1:this%nedges) = this%esrc(1:this%nedges)
    call move_alloc(itmp, this%esrc)
    allocate (itmp(cap))
    itmp(1:this%nedges) = this%edst(1:this%nedges)
    call move_alloc(itmp, this%edst)
    if (this%weighted) then
      allocate (rtmp(cap))
      rtmp(1:this%nedges) = this%ewgt(1:this%nedges)
      call move_alloc(rtmp, this%ewgt)
    end if
  end subroutine grow_edge_buffer

end module graphs
