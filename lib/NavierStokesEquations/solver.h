#pragma once

#include <functional>
#include <vector>

#include "lib/NavierStokesEquations/boundary.h"
#include "lib/NavierStokesEquations/field.h"
#include "lib/NavierStokesEquations/grid.h"
#include "lib/NavierStokesEquations/poisson.h"

namespace navierStokes {

enum class AdvectionScheme {
    // Second-order central differences in conservative (flux) form. The right
    // default: it is the accurate one, and it is what the convergence study in
    // examples/taylorGreen.cpp measures.
    Central,
    // First-order upwind. Only first-order accurate and noticeably diffusive,
    // but it stays well behaved at cell Reynolds numbers where the central
    // scheme produces oscillations.
    Upwind,
};

struct SolverOptions {
    // Kinematic viscosity nu. The Reynolds number of a flow with velocity
    // scale U and length scale L is U * L / nu.
    double viscosity = 1e-2;
    // Density, used only to convert the pressure the solver works with (which
    // is p / rho) into physical units.
    double density = 1.0;
    // Constant body force per unit mass, e.g. gravity, or the mean pressure
    // gradient that drives a periodic channel.
    double forceX = 0.0;
    double forceY = 0.0;
    AdvectionScheme advection = AdvectionScheme::Central;
    PoissonOptions poisson;
    // Fraction of the stability limit that maxStableTimeStep returns.
    double cflSafety = 0.5;
};

struct StepReport {
    double dt = 0.0;
    double time = 0.0;
    double maxVelocity = 0.0;
    // Largest |div u| over the fluid cells after the projection. The
    // divergence and gradient operators are exact transposes, so this is
    // exactly dt times the residual the pressure solve left behind rather than
    // an independent error: tightening PoissonOptions::tolerance tightens it,
    // and at tolerance 1e-12 it sits near machine epsilon.
    double maxDivergence = 0.0;
    PoissonReport poisson;
};

// Incompressible Navier-Stokes in two dimensions,
//
//   du/dt + (u . grad) u = -grad p + nu laplacian(u) + f,   div u = 0,
//
// discretised on a staggered grid and marched in time with Chorin's
// fractional-step (projection) method:
//
//   1. advance momentum explicitly, ignoring the pressure, to get u*;
//   2. solve laplacian(p) = div(u*) / dt;
//   3. correct, u = u* - dt grad p, which is discretely divergence free.
//
// The scheme is second-order accurate in space and first-order in time.
class NavierStokesSolver {
   public:
    NavierStokesSolver(const StaggeredGrid& grid, const BoundaryConditions& bc, const SolverOptions& options);

    // Marks obstacle cells; `solid` is indexed as i * ny + j and must have
    // nx * ny entries, or be empty to clear. Faces of solid cells are held at
    // zero, which imposes no-slip on the obstacle to first order: the surface
    // is approximated by the staircase of cell faces rather than resolved.
    //
    // Solid cells may not touch an inflow or outflow boundary, since the
    // boundary condition would then prescribe a velocity inside the obstacle.
    void setSolid(std::vector<char> solid);
    bool solidCell(int i, int j) const;
    bool hasSolid() const { return !solid_.empty(); }

    // Samples the given functions of (x, y) at the face positions of each
    // velocity component. The result is not automatically made divergence
    // free; call projectVelocity unless the field is known to be.
    void setVelocity(const std::function<double(double, double)>& uFunc,
                     const std::function<double(double, double)>& vFunc);
    void setUniformVelocity(double uValue, double vValue);

    // Makes the current velocity discretely divergence free with a single
    // projection. Use after setVelocity or after writing into u() / v().
    void projectVelocity();

    // Largest time step that keeps the explicit advection and diffusion stable,
    // scaled by options.cflSafety. Returns infinity for a fluid at rest with no
    // viscosity and no body force, so callers should cap it: stepAdaptive does.
    double maxStableTimeStep() const;

    StepReport step(double dt);
    StepReport stepAdaptive(double dtMax);

    double time() const { return time_; }
    const StaggeredGrid& grid() const { return grid_; }
    const BoundaryConditions& boundaryConditions() const { return bc_; }
    const SolverOptions& options() const { return options_; }

    // Velocity components on their faces and pressure at the cell centres.
    // The non-const accessors are for setting up or perturbing a flow; follow
    // any write with projectVelocity.
    const Field2D& u() const { return u_; }
    const Field2D& v() const { return v_; }
    const Field2D& p() const { return p_; }
    Field2D& u() { return u_; }
    Field2D& v() { return v_; }
    Field2D& p() { return p_; }

    double maxDivergence() const;
    double maxVelocity() const;
    double kineticEnergy() const;

    // Velocity interpolated to a cell centre, which is where most diagnostics
    // and every plot want it.
    double uAtCenter(int i, int j) const;
    double vAtCenter(int i, int j) const;

   private:
    void applyBoundaries();
    // Refreshes the ghost cells while leaving every boundary face as it is;
    // see OutflowUpdate for why the difference matters after a projection.
    void refreshGhosts();
    void applySolidFaces();
    void computeMomentum(double dt);
    void computeRhs(double dt);
    void project(double dt);

    // A velocity face is solid when either cell sharing it is solid.
    bool solidU(int i, int j) const;
    bool solidV(int i, int j) const;

    double advectU(int i, int j) const;
    double advectV(int i, int j) const;
    double laplacianU(int i, int j) const;
    double laplacianV(int i, int j) const;

    StaggeredGrid grid_;
    BoundaryConditions bc_;
    SolverOptions options_;
    PressurePoisson poisson_;

    Field2D u_;
    Field2D v_;
    Field2D p_;
    Field2D uStar_;
    Field2D vStar_;
    Field2D rhs_;
    std::vector<char> solid_;
    double time_ = 0.0;
};

}  // namespace navierStokes
