#pragma once

#include "lib/NavierStokesEquations/field.h"
#include "lib/NavierStokesEquations/grid.h"

namespace navierStokes {

enum class BoundaryKind {
    // Wall: zero normal velocity, tangential velocity equal to wallVelocity.
    NoSlip,
    // Wall: zero normal velocity, zero tangential shear.
    FreeSlip,
    // Wraps around to the opposite side. Both sides of an axis must agree.
    Periodic,
    // Prescribed normal velocity (see Wall::normalVelocity).
    Inflow,
    // Zero-gradient, rescaled so that the net flux through the boundary of the
    // domain vanishes.
    Outflow,
};

struct Wall {
    BoundaryKind kind = BoundaryKind::NoSlip;

    // Velocity of the wall along itself: u for the bottom/top walls, v for the
    // left/right walls. This is what drives a lid-driven cavity.
    double wallVelocity = 0.0;

    // Normal velocity imposed by Inflow, given as the velocity *component*
    // along the axis rather than as an inward speed: a left boundary with
    // normalVelocity = 1 blows fluid in, a right boundary with
    // normalVelocity = 1 sucks it out.
    double normalVelocity = 0.0;
};

struct BoundaryConditions {
    Wall left;
    Wall right;
    Wall bottom;
    Wall top;

    // True when the axis wraps around. Asserts that both sides agree, since a
    // half-periodic axis is not a meaningful configuration.
    bool periodicX() const;
    bool periodicY() const;

    // Lid-driven cavity: no-slip everywhere, top wall sliding at lidVelocity.
    static BoundaryConditions cavity(double lidVelocity);
    // Periodic in both directions, as used by the Taylor-Green vortex.
    static BoundaryConditions doublyPeriodic();
    // Periodic along x with no-slip walls along y: plane channel flow driven by
    // a body force.
    static BoundaryConditions periodicChannel();
    // Inflow on the left, outflow on the right, no-slip walls along y.
    static BoundaryConditions inflowOutflow(double inflowVelocity);
    // Inflow on the left, outflow on the right, free-slip walls along y, which
    // keeps a uniform stream uniform and is the usual choice for flow past an
    // obstacle in an otherwise unbounded fluid.
    static BoundaryConditions freeStream(double inflowVelocity);
};

// What applyVelocityBoundaries does with the faces that lie on an Outflow
// boundary. Every other boundary kind prescribes its faces from the boundary
// condition alone, so re-applying it changes nothing; an outflow face is the
// one that is derived from the interior, and it matters when that happens.
enum class OutflowUpdate {
    // Re-derive the outflow faces from the interior and rescale them so that
    // the net flux vanishes. This is the right choice before the momentum and
    // pressure stages of a step, which need a flux-balanced field.
    Extrapolate,
    // Leave the outflow faces exactly as they are and refresh only the ghost
    // cells. This is the right choice after the projection: the projection
    // deliberately leaves prescribed faces untouched, so re-deriving an
    // outflow face from the corrected interior would put the divergence that
    // the projection just removed straight back into the last row of cells.
    Keep,
};

// Fill the boundary faces and ghost cells of u and v so that every interior
// stencil can be evaluated without special-casing the domain edge.
//
// u must be sized (nx + 1) x ny and v must be sized nx x (ny + 1), both with a
// halo of at least one cell.
void applyVelocityBoundaries(const StaggeredGrid& grid, const BoundaryConditions& bc, Field2D& u, Field2D& v,
                             OutflowUpdate outflow = OutflowUpdate::Extrapolate);

// Net outward volume flux through the boundary of the domain. The pressure
// Poisson problem of the projection step uses homogeneous Neumann conditions on
// every side, so it is solvable only when this vanishes;
// applyVelocityBoundaries enforces that by rescaling the outflow faces.
double netBoundaryFlux(const StaggeredGrid& grid, const Field2D& u, const Field2D& v);

}  // namespace navierStokes
