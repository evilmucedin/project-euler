#pragma once

#include "lib/NavierStokesEquations/field.h"
#include "lib/NavierStokesEquations/grid.h"

namespace navierStokes {

// Derived quantities on a staggered velocity field. Every function here reads
// one cell past the domain, so the ghost cells of u and v must be current;
// applyVelocityBoundaries and every solver step leave them that way.

// div u at the cell centres.
Field2D divergenceField(const StaggeredGrid& grid, const Field2D& u, const Field2D& v);

// Vorticity omega = dv/dx - du/dy at the cell corners, where the staggered
// layout places it naturally: both derivatives are plain differences of
// adjacent faces, with no averaging. Sized (nx + 1) x (ny + 1).
Field2D vorticityAtCorners(const StaggeredGrid& grid, const Field2D& u, const Field2D& v);

// The same vorticity averaged onto the cell centres, which is what plots and
// image output want. Sized nx x ny.
Field2D vorticityField(const StaggeredGrid& grid, const Field2D& u, const Field2D& v);

// Velocity interpolated to the cell centres. Either output may be null.
void velocityAtCenters(const StaggeredGrid& grid, const Field2D& u, const Field2D& v, Field2D* uCenter,
                       Field2D* vCenter);

// Velocity magnitude at the cell centres.
Field2D speedField(const StaggeredGrid& grid, const Field2D& u, const Field2D& v);

// Integral of |u|^2 / 2 over the domain.
double kineticEnergy(const StaggeredGrid& grid, const Field2D& u, const Field2D& v);

}  // namespace navierStokes
