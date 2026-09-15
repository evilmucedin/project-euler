#include "lib/NavierStokesEquations/boundary.h"

#include <cassert>

namespace navierStokes {
namespace {

bool isPeriodic(const Wall& wall) { return wall.kind == BoundaryKind::Periodic; }

// Value of the velocity component normal to a boundary, which lives exactly on
// the boundary face and is therefore prescribed rather than computed.
double normalFaceValue(const Wall& wall, double interiorNeighbor, double current, OutflowUpdate outflow) {
    switch (wall.kind) {
        case BoundaryKind::Inflow:
            return wall.normalVelocity;
        case BoundaryKind::Outflow:
            if (outflow == OutflowUpdate::Keep) {
                return current;
            }
            return interiorNeighbor;  // zero gradient, rescaled by correctOutflow below
        case BoundaryKind::NoSlip:
        case BoundaryKind::FreeSlip:
        case BoundaryKind::Periodic:
            break;
    }
    return 0.0;
}

// Ghost value of the velocity component tangential to a boundary. That
// component has no sample on the boundary itself: the boundary face lies
// halfway between the last interior sample and the ghost sample, so a ghost of
// 2 * wall - interior puts exactly `wall` on the boundary.
double tangentialGhostValue(const Wall& wall, double interior) {
    switch (wall.kind) {
        case BoundaryKind::FreeSlip:
        case BoundaryKind::Outflow:
            return interior;  // zero shear / zero gradient
        case BoundaryKind::NoSlip:
        case BoundaryKind::Inflow:
        case BoundaryKind::Periodic:
            break;
    }
    return 2.0 * wall.wallVelocity - interior;
}

// Zero-gradient outflow does not conserve mass on its own, and the pressure
// Poisson problem of the projection step is solvable only when the net flux
// through the boundary vanishes. Spread the imbalance uniformly over the
// outflow faces, in the outward-normal sense.
void correctOutflow(const StaggeredGrid& grid, const BoundaryConditions& bc, Field2D& u, Field2D& v) {
    const double spanX = grid.nx * grid.dx;
    const double spanY = grid.ny * grid.dy;
    double outflowArea = 0.0;
    if (bc.left.kind == BoundaryKind::Outflow) {
        outflowArea += spanY;
    }
    if (bc.right.kind == BoundaryKind::Outflow) {
        outflowArea += spanY;
    }
    if (bc.bottom.kind == BoundaryKind::Outflow) {
        outflowArea += spanX;
    }
    if (bc.top.kind == BoundaryKind::Outflow) {
        outflowArea += spanX;
    }
    if (outflowArea == 0.0) {
        return;
    }

    const double correction = netBoundaryFlux(grid, u, v) / outflowArea;
    if (bc.left.kind == BoundaryKind::Outflow) {
        for (int j = -1; j <= grid.ny; ++j) {
            u(0, j) += correction;
        }
    }
    if (bc.right.kind == BoundaryKind::Outflow) {
        for (int j = -1; j <= grid.ny; ++j) {
            u(grid.nx, j) -= correction;
        }
    }
    if (bc.bottom.kind == BoundaryKind::Outflow) {
        for (int i = -1; i <= grid.nx; ++i) {
            v(i, 0) += correction;
        }
    }
    if (bc.top.kind == BoundaryKind::Outflow) {
        for (int i = -1; i <= grid.nx; ++i) {
            v(i, grid.ny) -= correction;
        }
    }
}

}  // namespace

bool BoundaryConditions::periodicX() const {
    const bool lo = isPeriodic(left);
    const bool hi = isPeriodic(right);
    assert(lo == hi && "both sides of the x axis must agree on periodicity");
    return lo && hi;
}

bool BoundaryConditions::periodicY() const {
    const bool lo = isPeriodic(bottom);
    const bool hi = isPeriodic(top);
    assert(lo == hi && "both sides of the y axis must agree on periodicity");
    return lo && hi;
}

BoundaryConditions BoundaryConditions::cavity(double lidVelocity) {
    BoundaryConditions bc;
    bc.top.wallVelocity = lidVelocity;
    return bc;
}

BoundaryConditions BoundaryConditions::doublyPeriodic() {
    BoundaryConditions bc;
    bc.left.kind = BoundaryKind::Periodic;
    bc.right.kind = BoundaryKind::Periodic;
    bc.bottom.kind = BoundaryKind::Periodic;
    bc.top.kind = BoundaryKind::Periodic;
    return bc;
}

BoundaryConditions BoundaryConditions::periodicChannel() {
    BoundaryConditions bc;
    bc.left.kind = BoundaryKind::Periodic;
    bc.right.kind = BoundaryKind::Periodic;
    return bc;
}

BoundaryConditions BoundaryConditions::inflowOutflow(double inflowVelocity) {
    BoundaryConditions bc;
    bc.left.kind = BoundaryKind::Inflow;
    bc.left.normalVelocity = inflowVelocity;
    bc.right.kind = BoundaryKind::Outflow;
    return bc;
}

BoundaryConditions BoundaryConditions::freeStream(double inflowVelocity) {
    BoundaryConditions bc = inflowOutflow(inflowVelocity);
    bc.bottom.kind = BoundaryKind::FreeSlip;
    bc.top.kind = BoundaryKind::FreeSlip;
    return bc;
}

double netBoundaryFlux(const StaggeredGrid& grid, const Field2D& u, const Field2D& v) {
    double flux = 0.0;
    for (int j = 0; j < grid.ny; ++j) {
        flux += (u(grid.nx, j) - u(0, j)) * grid.dy;
    }
    for (int i = 0; i < grid.nx; ++i) {
        flux += (v(i, grid.ny) - v(i, 0)) * grid.dx;
    }
    return flux;
}

void applyVelocityBoundaries(const StaggeredGrid& grid, const BoundaryConditions& bc, Field2D& u, Field2D& v,
                             OutflowUpdate outflow) {
    const int nx = grid.nx;
    const int ny = grid.ny;
    assert(u.nx() == nx + 1 && u.ny() == ny && u.halo() >= 1);
    assert(v.nx() == nx && v.ny() == ny + 1 && v.halo() >= 1);

    // The x pass runs first and covers every valid j including the ghost rows,
    // then the y pass covers every valid i including the ghost columns. In that
    // order the y pass overwrites the corner ghosts with values that already
    // account for the x boundaries, so all four corners end up consistent.
    const bool periodicX = bc.periodicX();
    const bool periodicY = bc.periodicY();

    for (int j = -1; j <= ny; ++j) {
        if (periodicX) {
            // u(0) and u(nx) are the same physical face; u(0) is the one the
            // momentum step updates, so u(nx) mirrors it.
            u(nx, j) = u(0, j);
            u(-1, j) = u(nx - 1, j);
            v(-1, j) = v(nx - 1, j);
            v(nx, j) = v(0, j);
        } else {
            u(0, j) = normalFaceValue(bc.left, u(1, j), u(0, j), outflow);
            u(nx, j) = normalFaceValue(bc.right, u(nx - 1, j), u(nx, j), outflow);
            v(-1, j) = tangentialGhostValue(bc.left, v(0, j));
            v(nx, j) = tangentialGhostValue(bc.right, v(nx - 1, j));
        }
    }

    for (int i = -1; i <= nx; ++i) {
        if (periodicY) {
            v(i, ny) = v(i, 0);
            v(i, -1) = v(i, ny - 1);
            u(i, -1) = u(i, ny - 1);
            u(i, ny) = u(i, 0);
        } else {
            v(i, 0) = normalFaceValue(bc.bottom, v(i, 1), v(i, 0), outflow);
            v(i, ny) = normalFaceValue(bc.top, v(i, ny - 1), v(i, ny), outflow);
            u(i, -1) = tangentialGhostValue(bc.bottom, u(i, 0));
            u(i, ny) = tangentialGhostValue(bc.top, u(i, ny - 1));
        }
    }

    if (outflow == OutflowUpdate::Extrapolate) {
        correctOutflow(grid, bc, u, v);
    }
}

}  // namespace navierStokes
