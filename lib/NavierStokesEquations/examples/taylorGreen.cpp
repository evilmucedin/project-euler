// Order-of-accuracy study on the Taylor-Green vortex.
//
// On a doubly periodic domain of side 2 pi the vortex
//
//   u = -cos(x) sin(y) e^{-2 nu t},   v = sin(x) cos(y) e^{-2 nu t},
//   p = -(cos(2x) + cos(2y)) e^{-4 nu t} / 4
//
// is an exact solution: the nonlinear term is balanced by the pressure
// gradient and the flow simply decays. That makes it the standard test for the
// order of a Navier-Stokes discretisation, since the error against the exact
// solution can be measured directly.
//
// The time step is taken proportional to dx^2 so that the first-order splitting
// error stays below the second-order spatial error; the table below should then
// show the error falling by a factor of four each time the grid is refined.
//
//   buck2 run //lib/NavierStokesEquations/examples:taylorGreen
//   ninja lib/NavierStokesEquations/examples/taylorGreen && \
//       ./build-ninja/bin/lib/NavierStokesEquations/examples/taylorGreen

#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

#include "lib/NavierStokesEquations/navierStokes.h"

using namespace navierStokes;

namespace {

const double kViscosity = 0.05;
const double kEndTime = 0.5;

double exactU(double x, double y, double t) {
    return -std::cos(x) * std::sin(y) * std::exp(-2.0 * kViscosity * t);
}

double exactV(double x, double y, double t) {
    return std::sin(x) * std::cos(y) * std::exp(-2.0 * kViscosity * t);
}

struct Errors {
    double rootMeanSquare = 0.0;
    double maximum = 0.0;
};

// Error of both velocity components against the exact solution, measured on
// the faces where the solver stores them.
Errors measure(const NavierStokesSolver& solver) {
    const StaggeredGrid& grid = solver.grid();
    Errors errors;
    double sumSquares = 0.0;
    int count = 0;
    for (int i = 0; i <= grid.nx; ++i) {
        for (int j = 0; j < grid.ny; ++j) {
            const double error = solver.u()(i, j) - exactU(grid.xU(i), grid.yU(j), solver.time());
            sumSquares += error * error;
            errors.maximum = std::max(errors.maximum, std::fabs(error));
            ++count;
        }
    }
    for (int i = 0; i < grid.nx; ++i) {
        for (int j = 0; j <= grid.ny; ++j) {
            const double error = solver.v()(i, j) - exactV(grid.xV(i), grid.yV(j), solver.time());
            sumSquares += error * error;
            errors.maximum = std::max(errors.maximum, std::fabs(error));
            ++count;
        }
    }
    errors.rootMeanSquare = std::sqrt(sumSquares / count);
    return errors;
}

}  // namespace

int main() {
    const int sizes[] = {16, 32, 64, 128};

    std::printf("Taylor-Green vortex, nu = %g, t = %g, dt ~ dx^2\n\n", kViscosity, kEndTime);
    std::printf("%5s %10s %8s %12s %8s %10s %10s\n", "n", "rms", "order", "max", "order", "maxDiv", "energy");

    std::vector<double> gridColumn;
    std::vector<double> rmsColumn;
    std::vector<double> maxColumn;
    std::vector<double> orderColumn;

    double previousRms = 0.0;
    double previousMax = 0.0;
    for (int n : sizes) {
        const StaggeredGrid grid(n, n, 2.0 * M_PI, 2.0 * M_PI);
        SolverOptions options;
        options.viscosity = kViscosity;
        options.poisson.tolerance = 1e-12;
        options.poisson.maxIterations = 5000;

        NavierStokesSolver solver(grid, BoundaryConditions::doublyPeriodic(), options);
        solver.setVelocity([](double x, double y) { return exactU(x, y, 0.0); },
                           [](double x, double y) { return exactV(x, y, 0.0); });

        const double stepTarget = 0.05 * grid.dx * grid.dx;
        const int steps = static_cast<int>(std::ceil(kEndTime / stepTarget));
        const double dt = kEndTime / steps;
        double maxDivergence = 0.0;
        for (int step = 0; step < steps; ++step) {
            maxDivergence = std::max(maxDivergence, solver.step(dt).maxDivergence);
        }

        const Errors errors = measure(solver);
        const double rmsOrder = previousRms > 0.0 ? std::log2(previousRms / errors.rootMeanSquare) : 0.0;
        const double maxOrder = previousMax > 0.0 ? std::log2(previousMax / errors.maximum) : 0.0;
        std::printf("%5d %10.3e %8s %12.3e %8s %10.2e %10.6f\n", n, errors.rootMeanSquare,
                    previousRms > 0.0 ? std::to_string(rmsOrder).substr(0, 5).c_str() : "-", errors.maximum,
                    previousMax > 0.0 ? std::to_string(maxOrder).substr(0, 5).c_str() : "-", maxDivergence,
                    solver.kineticEnergy());
        std::fflush(stdout);

        gridColumn.push_back(n);
        rmsColumn.push_back(errors.rootMeanSquare);
        maxColumn.push_back(errors.maximum);
        orderColumn.push_back(rmsOrder);

        previousRms = errors.rootMeanSquare;
        previousMax = errors.maximum;
    }

    // The energy of the exact solution decays as e^{-4 nu t} from its initial
    // value of (2 pi)^2 / 4 * 2 * 1/2 = pi^2.
    const double exactEnergy = M_PI * M_PI * std::exp(-4.0 * kViscosity * kEndTime);
    std::printf("\nexact kinetic energy at t = %g: %.6f\n", kEndTime, exactEnergy);

    if (!writeColumnsCsv("taylorGreenConvergence.csv", {"n", "rms", "max", "order"},
                         {gridColumn, rmsColumn, maxColumn, orderColumn})) {
        std::fprintf(stderr, "could not write taylorGreenConvergence.csv\n");
        return 1;
    }
    std::printf("wrote taylorGreenConvergence.csv\n");
    return 0;
}
