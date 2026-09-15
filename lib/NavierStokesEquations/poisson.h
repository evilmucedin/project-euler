#pragma once

#include <vector>

#include "lib/NavierStokesEquations/boundary.h"
#include "lib/NavierStokesEquations/field.h"
#include "lib/NavierStokesEquations/grid.h"

namespace navierStokes {

enum class PoissonMethod {
    // Unpreconditioned conjugate gradients. Needs no tuning and is the default.
    ConjugateGradient,
    // Successive over-relaxation, kept as a reference implementation and
    // because it is competitive when a very loose tolerance is enough.
    Sor,
};

struct PoissonOptions {
    PoissonMethod method = PoissonMethod::ConjugateGradient;
    int maxIterations = 500;
    // Convergence is reached when max |residual| <= tolerance * scale, where
    // scale is max(max |rhs|, 1). The floor of 1 makes the test absolute rather
    // than relative once the right-hand side becomes small, which it does as a
    // flow approaches a steady state.
    double tolerance = 1e-8;
    // Over-relaxation factor of the SOR sweep, in (0, 2). Ignored by CG.
    double relaxation = 1.8;
};

struct PoissonReport {
    int iterations = 0;
    double residual = 0.0;
    bool converged = false;
};

// Solves the pressure Poisson equation laplacian(p) = rhs on the cell centres
// of a staggered grid.
//
// Every side of the domain carries a homogeneous Neumann condition: the normal
// velocity on a boundary face is prescribed by the velocity boundary
// conditions, so the projection must not change it, and "no pressure-driven
// correction of this face" is exactly dp/dn = 0. Solid obstacle cells are
// Neumann boundaries for the same reason. Periodic sides wrap instead.
//
// A pure Neumann problem determines p only up to an additive constant, and is
// solvable only for a right-hand side of zero mean. Both solvers therefore
// project the mean out of the right-hand side and return a zero-mean solution.
// Only the gradient of p is used downstream, so the lost constant carries no
// information.
class PressurePoisson {
   public:
    PressurePoisson(const StaggeredGrid& grid, const BoundaryConditions& bc);

    // Cells with solid[i * ny + j] != 0 are excluded from the solve. Pass an
    // empty vector to clear the mask.
    void setSolid(std::vector<char> solid);
    bool isFluid(int i, int j) const;
    int fluidCellCount() const { return fluidCells_; }

    // Solves in place, using the incoming p as the initial guess. Warm-starting
    // from the previous step's pressure is what keeps the per-step iteration
    // count low in a time-marching simulation.
    PoissonReport solve(Field2D& p, const Field2D& rhs, const PoissonOptions& options);

    // laplacian(p) on the fluid cells, zero elsewhere.
    void applyLaplacian(const Field2D& p, Field2D& out) const;
    double residualMaxAbs(const Field2D& p, const Field2D& rhs) const;
    void subtractFluidMean(Field2D& p) const;
    double fluidMean(const Field2D& p) const;

   private:
    PoissonReport solveSor(Field2D& p, const PoissonOptions& options);
    PoissonReport solveConjugateGradient(Field2D& p, const PoissonOptions& options);

    // Sum of w * p over the fluid neighbours of (i, j), with w = 1/dx^2 or
    // 1/dy^2; *diagonal receives the sum of the weights that contributed.
    // Neighbours that are walls or solid drop out, which is the Neumann
    // condition, and the resulting operator stays symmetric.
    double neighborSum(const Field2D& p, int i, int j, double* diagonal) const;

    double dotFluid(const Field2D& a, const Field2D& b) const;
    double maxAbsFluid(const Field2D& a) const;
    int wrapX(int i) const;
    int wrapY(int j) const;
    void ensureScratch();

    StaggeredGrid grid_;
    BoundaryConditions bc_;
    std::vector<char> solid_;
    bool periodicX_;
    bool periodicY_;
    double weightX_;
    double weightY_;
    int fluidCells_ = 0;

    // Scratch kept across calls so that marching a simulation in time does not
    // allocate every step. b_ holds the mean-projected right-hand side.
    Field2D b_;
    Field2D r_;
    Field2D d_;
    Field2D q_;
};

}  // namespace navierStokes
