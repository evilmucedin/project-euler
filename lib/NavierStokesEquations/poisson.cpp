#include "lib/NavierStokesEquations/poisson.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <utility>

namespace navierStokes {
namespace {

// Recomputing a fresh residual costs a full sweep, so the SOR loop only checks
// convergence every few iterations.
const int kSorResidualCheckInterval = 4;

}  // namespace

PressurePoisson::PressurePoisson(const StaggeredGrid& grid, const BoundaryConditions& bc)
    : grid_(grid),
      bc_(bc),
      periodicX_(bc.periodicX()),
      periodicY_(bc.periodicY()),
      weightX_(1.0 / (grid.dx * grid.dx)),
      weightY_(1.0 / (grid.dy * grid.dy)),
      fluidCells_(grid.nx * grid.ny) {}

void PressurePoisson::setSolid(std::vector<char> solid) {
    const size_t expected = static_cast<size_t>(grid_.nx) * static_cast<size_t>(grid_.ny);
    assert(solid.empty() || solid.size() == expected);
    solid_ = std::move(solid);
    fluidCells_ = 0;
    for (int i = 0; i < grid_.nx; ++i) {
        for (int j = 0; j < grid_.ny; ++j) {
            if (isFluid(i, j)) {
                ++fluidCells_;
            }
        }
    }
}

int PressurePoisson::wrapX(int i) const {
    if (!periodicX_) {
        return i;
    }
    return (i % grid_.nx + grid_.nx) % grid_.nx;
}

int PressurePoisson::wrapY(int j) const {
    if (!periodicY_) {
        return j;
    }
    return (j % grid_.ny + grid_.ny) % grid_.ny;
}

bool PressurePoisson::isFluid(int i, int j) const {
    if (periodicX_) {
        i = wrapX(i);
    } else if (i < 0 || i >= grid_.nx) {
        return false;
    }
    if (periodicY_) {
        j = wrapY(j);
    } else if (j < 0 || j >= grid_.ny) {
        return false;
    }
    if (solid_.empty()) {
        return true;
    }
    return solid_[static_cast<size_t>(i) * static_cast<size_t>(grid_.ny) + static_cast<size_t>(j)] == 0;
}

double PressurePoisson::neighborSum(const Field2D& p, int i, int j, double* diagonal) const {
    const int neighborI[4] = {i - 1, i + 1, i, i};
    const int neighborJ[4] = {j, j, j - 1, j + 1};
    const double weight[4] = {weightX_, weightX_, weightY_, weightY_};

    double sum = 0.0;
    double diag = 0.0;
    for (int k = 0; k < 4; ++k) {
        if (!isFluid(neighborI[k], neighborJ[k])) {
            continue;  // wall or solid: the flux term drops out (Neumann)
        }
        sum += weight[k] * p(wrapX(neighborI[k]), wrapY(neighborJ[k]));
        diag += weight[k];
    }
    *diagonal = diag;
    return sum;
}

void PressurePoisson::applyLaplacian(const Field2D& p, Field2D& out) const {
    for (int i = 0; i < grid_.nx; ++i) {
        for (int j = 0; j < grid_.ny; ++j) {
            if (!isFluid(i, j)) {
                out(i, j) = 0.0;
                continue;
            }
            double diag = 0.0;
            const double sum = neighborSum(p, i, j, &diag);
            out(i, j) = sum - diag * p(i, j);
        }
    }
}

double PressurePoisson::residualMaxAbs(const Field2D& p, const Field2D& rhs) const {
    double result = 0.0;
    for (int i = 0; i < grid_.nx; ++i) {
        for (int j = 0; j < grid_.ny; ++j) {
            if (!isFluid(i, j)) {
                continue;
            }
            double diag = 0.0;
            const double sum = neighborSum(p, i, j, &diag);
            result = std::max(result, std::fabs(sum - diag * p(i, j) - rhs(i, j)));
        }
    }
    return result;
}

double PressurePoisson::fluidMean(const Field2D& p) const {
    if (fluidCells_ == 0) {
        return 0.0;
    }
    // Neumaier compensated summation: the mean is subtracted from the pressure
    // every step, and an inaccurate mean would let a slow drift accumulate.
    double total = 0.0;
    double compensation = 0.0;
    for (int i = 0; i < grid_.nx; ++i) {
        for (int j = 0; j < grid_.ny; ++j) {
            if (!isFluid(i, j)) {
                continue;
            }
            const double value = p(i, j);
            const double t = total + value;
            if (std::fabs(total) >= std::fabs(value)) {
                compensation += (total - t) + value;
            } else {
                compensation += (value - t) + total;
            }
            total = t;
        }
    }
    return (total + compensation) / fluidCells_;
}

void PressurePoisson::subtractFluidMean(Field2D& p) const {
    const double mean = fluidMean(p);
    for (int i = 0; i < grid_.nx; ++i) {
        for (int j = 0; j < grid_.ny; ++j) {
            p(i, j) = isFluid(i, j) ? p(i, j) - mean : 0.0;
        }
    }
}

double PressurePoisson::dotFluid(const Field2D& a, const Field2D& b) const {
    double total = 0.0;
    for (int i = 0; i < grid_.nx; ++i) {
        for (int j = 0; j < grid_.ny; ++j) {
            if (isFluid(i, j)) {
                total += a(i, j) * b(i, j);
            }
        }
    }
    return total;
}

double PressurePoisson::maxAbsFluid(const Field2D& a) const {
    double result = 0.0;
    for (int i = 0; i < grid_.nx; ++i) {
        for (int j = 0; j < grid_.ny; ++j) {
            if (isFluid(i, j)) {
                result = std::max(result, std::fabs(a(i, j)));
            }
        }
    }
    return result;
}

void PressurePoisson::ensureScratch() {
    if (b_.nx() == grid_.nx && b_.ny() == grid_.ny) {
        return;
    }
    b_ = Field2D(grid_.nx, grid_.ny, 0);
    r_ = Field2D(grid_.nx, grid_.ny, 0);
    d_ = Field2D(grid_.nx, grid_.ny, 0);
    q_ = Field2D(grid_.nx, grid_.ny, 0);
}

PoissonReport PressurePoisson::solve(Field2D& p, const Field2D& rhs, const PoissonOptions& options) {
    assert(p.nx() == grid_.nx && p.ny() == grid_.ny);
    assert(rhs.nx() == grid_.nx && rhs.ny() == grid_.ny);
    ensureScratch();

    // Project the mean out of the right-hand side: with Neumann conditions on
    // every side the operator annihilates constants, so a right-hand side with
    // a non-zero mean has no solution at all and the iteration would chase a
    // component it cannot represent.
    for (int i = 0; i < grid_.nx; ++i) {
        for (int j = 0; j < grid_.ny; ++j) {
            b_(i, j) = isFluid(i, j) ? rhs(i, j) : 0.0;
        }
    }
    const double rhsMean = fluidMean(b_);
    for (int i = 0; i < grid_.nx; ++i) {
        for (int j = 0; j < grid_.ny; ++j) {
            if (isFluid(i, j)) {
                b_(i, j) -= rhsMean;
            }
        }
    }

    switch (options.method) {
        case PoissonMethod::Sor:
            return solveSor(p, options);
        case PoissonMethod::ConjugateGradient:
            break;
    }
    return solveConjugateGradient(p, options);
}

PoissonReport PressurePoisson::solveSor(Field2D& p, const PoissonOptions& options) {
    PoissonReport report;
    const double scale = std::max(maxAbsFluid(b_), 1.0);
    const double target = options.tolerance * scale;

    for (int iteration = 1; iteration <= options.maxIterations; ++iteration) {
        for (int i = 0; i < grid_.nx; ++i) {
            for (int j = 0; j < grid_.ny; ++j) {
                if (!isFluid(i, j)) {
                    p(i, j) = 0.0;
                    continue;
                }
                double diag = 0.0;
                const double sum = neighborSum(p, i, j, &diag);
                if (diag == 0.0) {
                    p(i, j) = 0.0;  // a fluid cell with no fluid neighbour
                    continue;
                }
                const double relaxed = (sum - b_(i, j)) / diag;
                p(i, j) += options.relaxation * (relaxed - p(i, j));
            }
        }
        report.iterations = iteration;
        if (iteration % kSorResidualCheckInterval == 0 || iteration == options.maxIterations) {
            report.residual = residualMaxAbs(p, b_);
            if (report.residual <= target) {
                report.converged = true;
                break;
            }
        }
    }
    if (!report.converged) {
        report.residual = residualMaxAbs(p, b_);
        report.converged = report.residual <= target;
    }
    subtractFluidMean(p);
    return report;
}

PoissonReport PressurePoisson::solveConjugateGradient(Field2D& p, const PoissonOptions& options) {
    PoissonReport report;
    const double scale = std::max(maxAbsFluid(b_), 1.0);
    const double target = options.tolerance * scale;

    // laplacian(p) = b is solved as A p = -b with A = -laplacian, which is
    // symmetric positive semi-definite; the remaining null space is the
    // constants, which the zero-mean projections below keep out of the
    // iteration. The initial residual is therefore -b - A p = -b + laplacian(p).
    subtractFluidMean(p);
    applyLaplacian(p, q_);
    for (int i = 0; i < grid_.nx; ++i) {
        for (int j = 0; j < grid_.ny; ++j) {
            r_(i, j) = isFluid(i, j) ? q_(i, j) - b_(i, j) : 0.0;
            d_(i, j) = r_(i, j);
        }
    }

    double rr = dotFluid(r_, r_);
    report.residual = maxAbsFluid(r_);
    if (report.residual <= target) {
        report.converged = true;
        return report;
    }

    for (int iteration = 1; iteration <= options.maxIterations; ++iteration) {
        applyLaplacian(d_, q_);
        double dq = 0.0;
        for (int i = 0; i < grid_.nx; ++i) {
            for (int j = 0; j < grid_.ny; ++j) {
                if (!isFluid(i, j)) {
                    q_(i, j) = 0.0;
                    continue;
                }
                q_(i, j) = -q_(i, j);  // A = -laplacian
                dq += d_(i, j) * q_(i, j);
            }
        }
        report.iterations = iteration;
        if (dq <= 0.0) {
            // d has run into the null space of A; nothing left to reduce.
            break;
        }
        const double alpha = rr / dq;
        for (int i = 0; i < grid_.nx; ++i) {
            for (int j = 0; j < grid_.ny; ++j) {
                if (!isFluid(i, j)) {
                    continue;
                }
                p(i, j) += alpha * d_(i, j);
                r_(i, j) -= alpha * q_(i, j);
            }
        }
        // In exact arithmetic the residual already has zero mean; subtracting it
        // only removes round-off drift along the null direction.
        subtractFluidMean(r_);

        report.residual = maxAbsFluid(r_);
        if (report.residual <= target) {
            report.converged = true;
            break;
        }
        const double rrNext = dotFluid(r_, r_);
        const double beta = rrNext / rr;
        rr = rrNext;
        for (int i = 0; i < grid_.nx; ++i) {
            for (int j = 0; j < grid_.ny; ++j) {
                if (isFluid(i, j)) {
                    d_(i, j) = r_(i, j) + beta * d_(i, j);
                }
            }
        }
    }

    subtractFluidMean(p);
    return report;
}

}  // namespace navierStokes
