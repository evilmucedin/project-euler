#include "lib/NavierStokesEquations/solver.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <utility>

namespace navierStokes {

NavierStokesSolver::NavierStokesSolver(const StaggeredGrid& grid, const BoundaryConditions& bc,
                                       const SolverOptions& options)
    : grid_(grid),
      bc_(bc),
      options_(options),
      poisson_(grid, bc),
      u_(grid.nx + 1, grid.ny, 1),
      v_(grid.nx, grid.ny + 1, 1),
      p_(grid.nx, grid.ny, 0),
      uStar_(grid.nx + 1, grid.ny, 1),
      vStar_(grid.nx, grid.ny + 1, 1),
      rhs_(grid.nx, grid.ny, 0) {
    assert(options.viscosity >= 0.0);
    assert(options.density > 0.0);
    applyBoundaries();
}

bool NavierStokesSolver::solidCell(int i, int j) const {
    if (solid_.empty()) {
        return false;
    }
    if (i < 0 || i >= grid_.nx || j < 0 || j >= grid_.ny) {
        return false;
    }
    return solid_[static_cast<size_t>(i) * static_cast<size_t>(grid_.ny) + static_cast<size_t>(j)] != 0;
}

void NavierStokesSolver::setSolid(std::vector<char> solid) {
    const size_t expected = static_cast<size_t>(grid_.nx) * static_cast<size_t>(grid_.ny);
    assert(solid.empty() || solid.size() == expected);
    solid_ = std::move(solid);

#ifndef NDEBUG
    // An inflow or outflow boundary prescribes a velocity on its faces, which
    // would contradict the no-slip faces of an obstacle sitting against it.
    const auto isOpen = [](const Wall& wall) {
        return wall.kind == BoundaryKind::Inflow || wall.kind == BoundaryKind::Outflow;
    };
    for (int j = 0; j < grid_.ny; ++j) {
        assert(!(isOpen(bc_.left) && solidCell(0, j)) && "solid cell on an inflow/outflow boundary");
        assert(!(isOpen(bc_.right) && solidCell(grid_.nx - 1, j)) && "solid cell on an inflow/outflow boundary");
    }
    for (int i = 0; i < grid_.nx; ++i) {
        assert(!(isOpen(bc_.bottom) && solidCell(i, 0)) && "solid cell on an inflow/outflow boundary");
        assert(!(isOpen(bc_.top) && solidCell(i, grid_.ny - 1)) && "solid cell on an inflow/outflow boundary");
    }
#endif

    poisson_.setSolid(solid_);
    applySolidFaces();
    applyBoundaries();
}

bool NavierStokesSolver::solidU(int i, int j) const {
    if (solid_.empty()) {
        return false;
    }
    // The face at i separates cell i - 1 from cell i; on a periodic axis the
    // faces at 0 and nx are the same face and wrap around.
    int left = i - 1;
    int right = i;
    if (i == 0) {
        left = bc_.periodicX() ? grid_.nx - 1 : -1;
    }
    if (i == grid_.nx) {
        right = bc_.periodicX() ? 0 : -1;
    }
    return solidCell(left, j) || solidCell(right, j);
}

bool NavierStokesSolver::solidV(int i, int j) const {
    if (solid_.empty()) {
        return false;
    }
    int below = j - 1;
    int above = j;
    if (j == 0) {
        below = bc_.periodicY() ? grid_.ny - 1 : -1;
    }
    if (j == grid_.ny) {
        above = bc_.periodicY() ? 0 : -1;
    }
    return solidCell(i, below) || solidCell(i, above);
}

void NavierStokesSolver::applySolidFaces() {
    if (solid_.empty()) {
        return;
    }
    for (int i = 0; i <= grid_.nx; ++i) {
        for (int j = 0; j < grid_.ny; ++j) {
            if (solidU(i, j)) {
                u_(i, j) = 0.0;
            }
        }
    }
    for (int i = 0; i < grid_.nx; ++i) {
        for (int j = 0; j <= grid_.ny; ++j) {
            if (solidV(i, j)) {
                v_(i, j) = 0.0;
            }
        }
    }
}

void NavierStokesSolver::applyBoundaries() { applyVelocityBoundaries(grid_, bc_, u_, v_); }

void NavierStokesSolver::refreshGhosts() {
    applyVelocityBoundaries(grid_, bc_, u_, v_, OutflowUpdate::Keep);
}

void NavierStokesSolver::setVelocity(const std::function<double(double, double)>& uFunc,
                                     const std::function<double(double, double)>& vFunc) {
    for (int i = 0; i <= grid_.nx; ++i) {
        for (int j = 0; j < grid_.ny; ++j) {
            u_(i, j) = uFunc(grid_.xU(i), grid_.yU(j));
        }
    }
    for (int i = 0; i < grid_.nx; ++i) {
        for (int j = 0; j <= grid_.ny; ++j) {
            v_(i, j) = vFunc(grid_.xV(i), grid_.yV(j));
        }
    }
    applySolidFaces();
    applyBoundaries();
}

void NavierStokesSolver::setUniformVelocity(double uValue, double vValue) {
    setVelocity([uValue](double, double) { return uValue; }, [vValue](double, double) { return vValue; });
}

void NavierStokesSolver::projectVelocity() {
    applyBoundaries();
    // Treat the current velocity as the intermediate field of a step of unit
    // length: one projection then removes its divergence.
    uStar_ = u_;
    vStar_ = v_;
    computeRhs(1.0);
    poisson_.solve(p_, rhs_, options_.poisson);
    project(1.0);
    refreshGhosts();
}

double NavierStokesSolver::laplacianU(int i, int j) const {
    const double d2x = (u_(i - 1, j) - 2.0 * u_(i, j) + u_(i + 1, j)) / (grid_.dx * grid_.dx);
    const double d2y = (u_(i, j - 1) - 2.0 * u_(i, j) + u_(i, j + 1)) / (grid_.dy * grid_.dy);
    return d2x + d2y;
}

double NavierStokesSolver::laplacianV(int i, int j) const {
    const double d2x = (v_(i - 1, j) - 2.0 * v_(i, j) + v_(i + 1, j)) / (grid_.dx * grid_.dx);
    const double d2y = (v_(i, j - 1) - 2.0 * v_(i, j) + v_(i, j + 1)) / (grid_.dy * grid_.dy);
    return d2x + d2y;
}

double NavierStokesSolver::advectU(int i, int j) const {
    const double dx = grid_.dx;
    const double dy = grid_.dy;
    if (options_.advection == AdvectionScheme::Upwind) {
        const double uHere = u_(i, j);
        const double vHere = 0.25 * (v_(i - 1, j) + v_(i, j) + v_(i - 1, j + 1) + v_(i, j + 1));
        const double dudx = uHere > 0.0 ? (u_(i, j) - u_(i - 1, j)) / dx : (u_(i + 1, j) - u_(i, j)) / dx;
        const double dudy = vHere > 0.0 ? (u_(i, j) - u_(i, j - 1)) / dy : (u_(i, j + 1) - u_(i, j)) / dy;
        return uHere * dudx + vHere * dudy;
    }
    // Conservative central form, d(u u)/dx + d(u v)/dy. The products are formed
    // where the two factors meet: u u at the cell centres either side of the
    // face, u v at the two corners above and below it.
    const double uEast = 0.5 * (u_(i, j) + u_(i + 1, j));
    const double uWest = 0.5 * (u_(i - 1, j) + u_(i, j));
    const double uNorth = 0.5 * (u_(i, j) + u_(i, j + 1));
    const double uSouth = 0.5 * (u_(i, j - 1) + u_(i, j));
    const double vNorth = 0.5 * (v_(i - 1, j + 1) + v_(i, j + 1));
    const double vSouth = 0.5 * (v_(i - 1, j) + v_(i, j));
    return (uEast * uEast - uWest * uWest) / dx + (uNorth * vNorth - uSouth * vSouth) / dy;
}

double NavierStokesSolver::advectV(int i, int j) const {
    const double dx = grid_.dx;
    const double dy = grid_.dy;
    if (options_.advection == AdvectionScheme::Upwind) {
        const double vHere = v_(i, j);
        const double uHere = 0.25 * (u_(i, j - 1) + u_(i + 1, j - 1) + u_(i, j) + u_(i + 1, j));
        const double dvdx = uHere > 0.0 ? (v_(i, j) - v_(i - 1, j)) / dx : (v_(i + 1, j) - v_(i, j)) / dx;
        const double dvdy = vHere > 0.0 ? (v_(i, j) - v_(i, j - 1)) / dy : (v_(i, j + 1) - v_(i, j)) / dy;
        return uHere * dvdx + vHere * dvdy;
    }
    const double vNorth = 0.5 * (v_(i, j) + v_(i, j + 1));
    const double vSouth = 0.5 * (v_(i, j - 1) + v_(i, j));
    const double vEast = 0.5 * (v_(i, j) + v_(i + 1, j));
    const double vWest = 0.5 * (v_(i - 1, j) + v_(i, j));
    const double uEast = 0.5 * (u_(i + 1, j - 1) + u_(i + 1, j));
    const double uWest = 0.5 * (u_(i, j - 1) + u_(i, j));
    return (uEast * vEast - uWest * vWest) / dx + (vNorth * vNorth - vSouth * vSouth) / dy;
}

void NavierStokesSolver::computeMomentum(double dt) {
    const int nx = grid_.nx;
    const int ny = grid_.ny;
    const double nu = options_.viscosity;

    // On a wall the face at i = 0 carries a prescribed normal velocity, so the
    // update starts at i = 1; on a periodic axis the face at i = 0 is interior
    // and the face at i = nx mirrors it.
    const bool periodicX = bc_.periodicX();
    const bool periodicY = bc_.periodicY();

    for (int i = periodicX ? 0 : 1; i < nx; ++i) {
        for (int j = 0; j < ny; ++j) {
            if (solidU(i, j)) {
                uStar_(i, j) = 0.0;
                continue;
            }
            uStar_(i, j) = u_(i, j) + dt * (-advectU(i, j) + nu * laplacianU(i, j) + options_.forceX);
        }
    }
    for (int j = 0; j < ny; ++j) {
        if (periodicX) {
            uStar_(nx, j) = uStar_(0, j);
        } else {
            // Prescribed faces pass through the momentum step untouched, which
            // is what makes div(u*) compatible with the Neumann pressure
            // problem that the projection solves.
            uStar_(0, j) = u_(0, j);
            uStar_(nx, j) = u_(nx, j);
        }
    }

    for (int i = 0; i < nx; ++i) {
        for (int j = periodicY ? 0 : 1; j < ny; ++j) {
            if (solidV(i, j)) {
                vStar_(i, j) = 0.0;
                continue;
            }
            vStar_(i, j) = v_(i, j) + dt * (-advectV(i, j) + nu * laplacianV(i, j) + options_.forceY);
        }
    }
    for (int i = 0; i < nx; ++i) {
        if (periodicY) {
            vStar_(i, ny) = vStar_(i, 0);
        } else {
            vStar_(i, 0) = v_(i, 0);
            vStar_(i, ny) = v_(i, ny);
        }
    }
}

void NavierStokesSolver::computeRhs(double dt) {
    for (int i = 0; i < grid_.nx; ++i) {
        for (int j = 0; j < grid_.ny; ++j) {
            if (solidCell(i, j)) {
                rhs_(i, j) = 0.0;
                continue;
            }
            const double divergence = (uStar_(i + 1, j) - uStar_(i, j)) / grid_.dx +
                                      (vStar_(i, j + 1) - vStar_(i, j)) / grid_.dy;
            rhs_(i, j) = divergence / dt;
        }
    }
}

void NavierStokesSolver::project(double dt) {
    const int nx = grid_.nx;
    const int ny = grid_.ny;
    const bool periodicX = bc_.periodicX();
    const bool periodicY = bc_.periodicY();

    for (int i = periodicX ? 0 : 1; i < nx; ++i) {
        for (int j = 0; j < ny; ++j) {
            if (solidU(i, j)) {
                u_(i, j) = 0.0;
                continue;
            }
            const int west = (i == 0) ? nx - 1 : i - 1;  // only reached when periodic
            u_(i, j) = uStar_(i, j) - dt * (p_(i, j) - p_(west, j)) / grid_.dx;
        }
    }
    for (int j = 0; j < ny; ++j) {
        if (periodicX) {
            u_(nx, j) = u_(0, j);
        } else {
            u_(0, j) = uStar_(0, j);
            u_(nx, j) = uStar_(nx, j);
        }
    }

    for (int i = 0; i < nx; ++i) {
        for (int j = periodicY ? 0 : 1; j < ny; ++j) {
            if (solidV(i, j)) {
                v_(i, j) = 0.0;
                continue;
            }
            const int south = (j == 0) ? ny - 1 : j - 1;
            v_(i, j) = vStar_(i, j) - dt * (p_(i, j) - p_(i, south)) / grid_.dy;
        }
    }
    for (int i = 0; i < nx; ++i) {
        if (periodicY) {
            v_(i, ny) = v_(i, 0);
        } else {
            v_(i, 0) = vStar_(i, 0);
            v_(i, ny) = vStar_(i, ny);
        }
    }
}

double NavierStokesSolver::maxStableTimeStep() const {
    double limit = std::numeric_limits<double>::infinity();
    const double uMax = u_.maxAbs();
    const double vMax = v_.maxAbs();
    if (uMax > 0.0) {
        limit = std::min(limit, grid_.dx / uMax);
    }
    if (vMax > 0.0) {
        limit = std::min(limit, grid_.dy / vMax);
    }
    if (options_.viscosity > 0.0) {
        // Stability limit of the explicit 5-point diffusion operator in 2D.
        const double inverseSquares = 1.0 / (grid_.dx * grid_.dx) + 1.0 / (grid_.dy * grid_.dy);
        limit = std::min(limit, 0.5 / (options_.viscosity * inverseSquares));
    }
    const double force = std::hypot(options_.forceX, options_.forceY);
    if (force > 0.0) {
        // Keep a body force from accelerating fluid across a cell in one step.
        limit = std::min(limit, std::sqrt(2.0 * grid_.minSpacing() / force));
    }
    return options_.cflSafety * limit;
}

StepReport NavierStokesSolver::step(double dt) {
    assert(dt > 0.0 && std::isfinite(dt));

    applyBoundaries();
    computeMomentum(dt);
    computeRhs(dt);

    StepReport report;
    report.poisson = poisson_.solve(p_, rhs_, options_.poisson);
    project(dt);

    // Only the ghost cells: the projection left every prescribed face alone,
    // and re-deriving a zero-gradient outflow face here would undo it.
    refreshGhosts();

    time_ += dt;
    report.dt = dt;
    report.time = time_;
    report.maxDivergence = maxDivergence();
    report.maxVelocity = maxVelocity();
    return report;
}

StepReport NavierStokesSolver::stepAdaptive(double dtMax) {
    assert(dtMax > 0.0 && std::isfinite(dtMax));
    return step(std::min(dtMax, maxStableTimeStep()));
}

double NavierStokesSolver::maxDivergence() const {
    double result = 0.0;
    for (int i = 0; i < grid_.nx; ++i) {
        for (int j = 0; j < grid_.ny; ++j) {
            if (solidCell(i, j)) {
                continue;
            }
            const double divergence =
                (u_(i + 1, j) - u_(i, j)) / grid_.dx + (v_(i, j + 1) - v_(i, j)) / grid_.dy;
            result = std::max(result, std::fabs(divergence));
        }
    }
    return result;
}

double NavierStokesSolver::maxVelocity() const {
    double result = 0.0;
    for (int i = 0; i < grid_.nx; ++i) {
        for (int j = 0; j < grid_.ny; ++j) {
            if (solidCell(i, j)) {
                continue;
            }
            result = std::max(result, std::hypot(uAtCenter(i, j), vAtCenter(i, j)));
        }
    }
    return result;
}

double NavierStokesSolver::kineticEnergy() const {
    double total = 0.0;
    for (int i = 0; i < grid_.nx; ++i) {
        for (int j = 0; j < grid_.ny; ++j) {
            if (solidCell(i, j)) {
                continue;
            }
            const double uc = uAtCenter(i, j);
            const double vc = vAtCenter(i, j);
            total += 0.5 * (uc * uc + vc * vc);
        }
    }
    return total * grid_.cellArea();
}

double NavierStokesSolver::uAtCenter(int i, int j) const { return 0.5 * (u_(i, j) + u_(i + 1, j)); }

double NavierStokesSolver::vAtCenter(int i, int j) const { return 0.5 * (v_(i, j) + v_(i, j + 1)); }

}  // namespace navierStokes
