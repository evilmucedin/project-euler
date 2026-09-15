// Plane Poiseuille flow: the startup of a pressure-driven channel.
//
// A channel that is periodic along x and bounded by no-slip walls at y = 0 and
// y = h, driven by a constant force G per unit mass, has the steady solution
//
//   u(y) = G y (h - y) / (2 nu),
//
// a parabola with peak G h^2 / (8 nu) and mean two thirds of that. Starting
// from rest, the flow approaches it on the viscous time scale h^2 / nu. Both
// the transient and the final profile are printed below, and the error against
// the exact parabola is measured on a sequence of grids: it should fall by a
// factor of four with each refinement.
//
//   buck2 run //lib/NavierStokesEquations/examples:channelFlow
//   ninja lib/NavierStokesEquations/examples/channelFlow && \
//       ./build-ninja/bin/lib/NavierStokesEquations/examples/channelFlow
//
// Writes channelProfile.csv; plot it with examples/plot.py channel.

#include <cmath>
#include <cstdio>
#include <vector>

#include "lib/NavierStokesEquations/navierStokes.h"

using namespace navierStokes;

namespace {

const double kHeight = 1.0;
const double kViscosity = 0.1;
const double kForce = 0.1;

double exactProfile(double y) { return kForce / (2.0 * kViscosity) * y * (kHeight - y); }

struct Result {
    double maxError = 0.0;
    double maxVelocity = 0.0;
    double flowRate = 0.0;
    double maxDivergence = 0.0;
    std::vector<double> positions;
    std::vector<double> profile;
};

Result run(int ny, double endTime, bool verbose) {
    // Only a few cells along x: the flow is one dimensional, and a periodic
    // channel has nothing to resolve in that direction.
    const StaggeredGrid grid(4, ny, 1.0, kHeight);
    SolverOptions options;
    options.viscosity = kViscosity;
    options.forceX = kForce;
    options.poisson.tolerance = 1e-12;

    NavierStokesSolver solver(grid, BoundaryConditions::periodicChannel(), options);

    if (verbose) {
        std::printf("%8s %10s %12s %12s\n", "time", "peak u", "exact peak", "maxDiv");
    }
    double nextReport = 0.0;
    while (solver.time() < endTime) {
        const StepReport report = solver.stepAdaptive(endTime - solver.time());
        if (verbose && report.time >= nextReport) {
            // The transient of the exact solution is a sum over the modes of
            // the channel; only its peak is printed, next to the steady value.
            std::printf("%8.2f %10.6f %12.6f %12.3e\n", report.time, report.maxVelocity,
                        exactProfile(0.5 * kHeight), report.maxDivergence);
            std::fflush(stdout);
            nextReport += 0.1 * endTime;
        }
    }

    Result result;
    result.maxDivergence = solver.maxDivergence();
    const int column = grid.nx / 2;
    for (int j = 0; j < ny; ++j) {
        const double y = grid.yU(j);
        const double computed = solver.u()(column, j);
        result.positions.push_back(y);
        result.profile.push_back(computed);
        result.maxError = std::max(result.maxError, std::fabs(computed - exactProfile(y)));
        result.maxVelocity = std::max(result.maxVelocity, computed);
        result.flowRate += computed * grid.dy;
    }
    return result;
}

}  // namespace

int main() {
    const double endTime = 25.0;  // several viscous times, h^2 / nu = 10
    std::printf("Plane Poiseuille flow, nu = %g, force = %g, h = %g\n", kViscosity, kForce, kHeight);
    std::printf("exact peak velocity %g, exact flow rate %g\n\n", kForce * kHeight * kHeight / (8.0 * kViscosity),
                kForce * kHeight * kHeight * kHeight / (12.0 * kViscosity));

    const Result reference = run(32, endTime, true);

    std::printf("\ngrid refinement at t = %g\n", endTime);
    std::printf("%5s %12s %8s %12s %12s %12s\n", "ny", "maxError", "order", "peak u", "flow rate", "maxDiv");
    std::vector<double> sizeColumn;
    std::vector<double> errorColumn;
    double previousError = 0.0;
    for (int ny : {8, 16, 32, 64}) {
        const Result result = run(ny, endTime, false);
        const double order = previousError > 0.0 ? std::log2(previousError / result.maxError) : 0.0;
        std::printf("%5d %12.3e %8s %12.6f %12.6f %12.3e\n", ny, result.maxError,
                    previousError > 0.0 ? std::to_string(order).substr(0, 5).c_str() : "-", result.maxVelocity,
                    result.flowRate, result.maxDivergence);
        std::fflush(stdout);
        sizeColumn.push_back(ny);
        errorColumn.push_back(result.maxError);
        previousError = result.maxError;
    }

    std::vector<double> exact;
    for (double y : reference.positions) {
        exact.push_back(exactProfile(y));
    }
    if (!writeColumnsCsv("channelProfile.csv", {"y", "computed", "exact", "ny", "maxError"},
                         {reference.positions, reference.profile, exact, sizeColumn, errorColumn})) {
        std::fprintf(stderr, "could not write channelProfile.csv\n");
        return 1;
    }
    std::printf("\nwrote channelProfile.csv\n");
    return 0;
}
