// Lid-driven cavity, the standard benchmark for an incompressible solver.
//
// A square box of unit side is filled with fluid at rest; the top wall then
// slides at unit speed and drags a primary vortex into the interior, with
// weaker counter-rotating vortices in the bottom corners. Ghia, Ghia & Shin
// (J. Comput. Phys. 48, 1982) tabulated the velocity along the two centrelines
// for several Reynolds numbers, and those numbers are reproduced below.
//
//   buck2 run //lib/NavierStokesEquations/examples:lidDrivenCavity
//   ninja lib/NavierStokesEquations/examples/lidDrivenCavity && \
//       ./build-ninja/bin/lib/NavierStokesEquations/examples/lidDrivenCavity
//
// Writes cavityCentrelines.csv, cavityVelocity.csv, cavityVorticity.csv and
// cavityVorticity.ppm; plot them with examples/plot.py cavity.

#include <cmath>
#include <cstdio>
#include <vector>

#include "lib/NavierStokesEquations/navierStokes.h"

using namespace navierStokes;

namespace {

const int kCells = 64;
const double kLidVelocity = 1.0;
const double kReynolds = 100.0;
const double kEndTime = 30.0;  // long enough for the flow to reach steady state

// Ghia, Ghia & Shin (1982), table I: u along the vertical centreline at
// Re = 100. Used only for the printed comparison.
struct Reference {
    double y;
    double u;
};

const Reference kGhiaRe100[] = {
    {0.0000, 0.00000},  {0.0547, -0.03717}, {0.0625, -0.04192}, {0.0703, -0.04775}, {0.1016, -0.06434},
    {0.1719, -0.10150}, {0.2813, -0.15662}, {0.4531, -0.21090}, {0.5000, -0.20581}, {0.6172, -0.13641},
    {0.7344, 0.00332},  {0.8516, 0.23151},  {0.9531, 0.68717},  {0.9609, 0.73722},  {0.9688, 0.78871},
    {0.9766, 0.84123},  {1.0000, 1.00000},
};

// Linear interpolation of the computed profile, so that it can be compared at
// the reference stations rather than at the grid points.
double interpolate(const std::vector<double>& positions, const std::vector<double>& values, double at) {
    if (at <= positions.front()) {
        return values.front();
    }
    if (at >= positions.back()) {
        return values.back();
    }
    for (size_t k = 1; k < positions.size(); ++k) {
        if (positions[k] >= at) {
            const double weight = (at - positions[k - 1]) / (positions[k] - positions[k - 1]);
            return values[k - 1] + weight * (values[k] - values[k - 1]);
        }
    }
    return values.back();
}

}  // namespace

int main() {
    const StaggeredGrid grid(kCells, kCells, 1.0, 1.0);
    SolverOptions options;
    options.viscosity = kLidVelocity * grid.lengthX / kReynolds;
    options.poisson.tolerance = 1e-10;
    options.poisson.maxIterations = 2000;

    NavierStokesSolver solver(grid, BoundaryConditions::cavity(kLidVelocity), options);

    std::printf("Lid-driven cavity, %d x %d, Re = %g, nu = %g\n", kCells, kCells, kReynolds, options.viscosity);
    std::printf("%8s %8s %10s %12s %10s %6s\n", "step", "time", "energy", "maxDiv", "maxVel", "cgIt");

    int step = 0;
    while (solver.time() < kEndTime) {
        const StepReport report = solver.stepAdaptive(kEndTime - solver.time());
        ++step;
        if (step % 1000 == 0 || solver.time() >= kEndTime) {
            std::printf("%8d %8.3f %10.6f %12.3e %10.6f %6d\n", step, report.time, solver.kineticEnergy(),
                        report.maxDivergence, report.maxVelocity, report.poisson.iterations);
            std::fflush(stdout);
        }
    }

    // The centrelines: u against y through the middle of the box, and v
    // against x. The interior values are read at the cell centres, which is
    // where the interpolated velocity lives; the two walls are added at their
    // prescribed values so that the profile spans the whole box, as the
    // reference table does.
    std::vector<double> yPositions = {0.0};
    std::vector<double> uProfile = {0.0};
    std::vector<double> xPositions = {0.0};
    std::vector<double> vProfile = {0.0};
    for (int j = 0; j < grid.ny; ++j) {
        yPositions.push_back(grid.yP(j));
        uProfile.push_back(solver.uAtCenter(grid.nx / 2, j));
    }
    yPositions.push_back(grid.lengthY);
    uProfile.push_back(kLidVelocity);
    for (int i = 0; i < grid.nx; ++i) {
        xPositions.push_back(grid.xP(i));
        vProfile.push_back(solver.vAtCenter(i, grid.ny / 2));
    }
    xPositions.push_back(grid.lengthX);
    vProfile.push_back(0.0);

    std::printf("\nu along the vertical centreline against Ghia et al. (1982), Re = 100\n");
    std::printf("%8s %12s %12s %12s\n", "y", "computed", "reference", "difference");
    double worst = 0.0;
    for (const Reference& reference : kGhiaRe100) {
        const double computed = interpolate(yPositions, uProfile, reference.y);
        worst = std::max(worst, std::fabs(computed - reference.u));
        std::printf("%8.4f %12.5f %12.5f %12.5f\n", reference.y, computed, reference.u, computed - reference.u);
    }
    std::printf("largest difference: %.5f\n", worst);

    double minimumU = 0.0;
    double minimumY = 0.0;
    for (size_t k = 0; k < uProfile.size(); ++k) {
        if (uProfile[k] < minimumU) {
            minimumU = uProfile[k];
            minimumY = yPositions[k];
        }
    }
    std::printf("\nminimum of u on the centreline: %.5f at y = %.4f (reference -0.21090 at y = 0.4531)\n", minimumU,
                minimumY);
    std::printf("steady state reached: final kinetic energy %.8f\n", solver.kineticEnergy());

    const Field2D vorticity = vorticityField(grid, solver.u(), solver.v());
    const bool written = writeColumnsCsv("cavityCentrelines.csv", {"y", "uCenterline", "x", "vCenterline"},
                                         {yPositions, uProfile, xPositions, vProfile}) &&
                         writeVelocityCsv("cavityVelocity.csv", grid, solver.u(), solver.v()) &&
                         writeFieldCsv("cavityVorticity.csv", vorticity) &&
                         writePpm("cavityVorticity.ppm", vorticity, -5.0, 5.0);
    if (!written) {
        std::fprintf(stderr, "could not write the output files\n");
        return 1;
    }
    std::printf("wrote cavityCentrelines.csv, cavityVelocity.csv, cavityVorticity.csv, cavityVorticity.ppm\n");
    return 0;
}
