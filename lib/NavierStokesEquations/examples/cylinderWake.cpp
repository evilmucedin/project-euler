// Flow past a circular cylinder: the Karman vortex street.
//
// A uniform stream meets a cylinder of diameter D at Reynolds number
// U D / nu = 100. Above about Re = 47 the steady wake is unstable, and vortices
// shed alternately from the two sides at a frequency f whose dimensionless
// form, the Strouhal number St = f D / U, is close to 0.16 at this Reynolds
// number. The shedding frequency is recovered below from the transverse
// velocity recorded at a probe in the wake.
//
// The cylinder is resolved as a staircase of solid cells, so the drag is only
// crudely approximated; the shedding frequency is far less sensitive to that
// and comes out in the right range even on this coarse grid. The cylinder sits
// half a cell off the centreline, which is what breaks the symmetry and lets
// the instability grow without waiting for round-off to seed it.
//
//   buck2 run //lib/NavierStokesEquations/examples:cylinderWake
//   ninja lib/NavierStokesEquations/examples/cylinderWake && \
//       ./build-ninja/bin/lib/NavierStokesEquations/examples/cylinderWake
//
// Writes cylinderProbe.csv and a numbered sequence of cylinderWake####.ppm
// images of the vorticity; plot them with examples/plot.py cylinder.

#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

#include "lib/NavierStokesEquations/navierStokes.h"

using namespace navierStokes;

namespace {

const int kCellsX = 160;
const int kCellsY = 40;
const double kLengthX = 4.0;
const double kLengthY = 1.0;
const double kDiameter = 0.2;
const double kCenterX = 0.8;
const double kStream = 1.0;
const double kReynolds = 100.0;
const double kEndTime = 40.0;
const int kFrames = 40;

// Times at which the transverse velocity crosses zero from below, from which
// the shedding period follows directly.
std::vector<double> upwardCrossings(const std::vector<double>& times, const std::vector<double>& values) {
    std::vector<double> crossings;
    for (size_t k = 1; k < values.size(); ++k) {
        if (values[k - 1] < 0.0 && values[k] >= 0.0) {
            const double weight = -values[k - 1] / (values[k] - values[k - 1]);
            crossings.push_back(times[k - 1] + weight * (times[k] - times[k - 1]));
        }
    }
    return crossings;
}

std::string frameName(int index) {
    char buffer[64];
    std::snprintf(buffer, sizeof(buffer), "cylinderWake%04d.ppm", index);
    return buffer;
}

}  // namespace

int main() {
    const StaggeredGrid grid(kCellsX, kCellsY, kLengthX, kLengthY);
    SolverOptions options;
    options.viscosity = kStream * kDiameter / kReynolds;
    // The wake carries steep velocity gradients across a coarse grid, where the
    // central scheme would ring; upwinding trades some accuracy for a clean
    // solution at this resolution.
    options.advection = AdvectionScheme::Upwind;
    options.poisson.tolerance = 1e-9;
    options.poisson.maxIterations = 2000;

    NavierStokesSolver solver(grid, BoundaryConditions::freeStream(kStream), options);
    // Half a cell below the centreline, which seeds the instability.
    const double centerY = 0.5 * kLengthY - 0.5 * grid.dy;
    const std::vector<char> cylinder = solidDisk(grid, kCenterX, centerY, 0.5 * kDiameter);
    solver.setSolid(cylinder);
    solver.setUniformVelocity(kStream, 0.0);
    solver.projectVelocity();

    const Field2D mask = solidAsField(grid, cylinder);
    // The probe sits two diameters downstream of the cylinder, on its axis.
    int probeI = 0;
    int probeJ = 0;
    for (int i = 0; i < grid.nx; ++i) {
        if (grid.xP(i) <= kCenterX + 2.0 * kDiameter) {
            probeI = i;
        }
    }
    for (int j = 0; j < grid.ny; ++j) {
        if (grid.yP(j) <= centerY) {
            probeJ = j;
        }
    }

    std::printf("Flow past a cylinder, %d x %d, Re = %g, nu = %g, D = %g\n", kCellsX, kCellsY, kReynolds,
                options.viscosity, kDiameter);
    std::printf("%d solid cells, probe at x = %.3f, y = %.3f\n\n", solidCellCount(cylinder), grid.xP(probeI),
                grid.yP(probeJ));
    std::printf("%8s %8s %10s %10s %12s %6s\n", "step", "time", "probe v", "maxVel", "maxDiv", "cgIt");

    std::vector<double> probeTimes;
    std::vector<double> probeValues;
    int step = 0;
    int frame = 0;
    double nextFrameTime = 0.0;
    while (solver.time() < kEndTime) {
        const StepReport report = solver.stepAdaptive(kEndTime - solver.time());
        ++step;
        probeTimes.push_back(report.time);
        probeValues.push_back(solver.vAtCenter(probeI, probeJ));

        if (report.time >= nextFrameTime && frame < kFrames) {
            const Field2D vorticity = vorticityField(grid, solver.u(), solver.v());
            if (!writePpm(frameName(frame), vorticity, -20.0, 20.0, &mask)) {
                std::fprintf(stderr, "could not write %s\n", frameName(frame).c_str());
                return 1;
            }
            ++frame;
            nextFrameTime = frame * kEndTime / kFrames;
        }
        if (step % 200 == 0 || solver.time() >= kEndTime) {
            std::printf("%8d %8.3f %10.5f %10.5f %12.3e %6d\n", step, report.time, probeValues.back(),
                        report.maxVelocity, report.maxDivergence, report.poisson.iterations);
            std::fflush(stdout);
        }
    }

    // Measure the frequency over the second half of the run, by which time the
    // initial transient has been washed downstream.
    std::vector<double> lateTimes;
    std::vector<double> lateValues;
    for (size_t k = 0; k < probeTimes.size(); ++k) {
        if (probeTimes[k] >= 0.5 * kEndTime) {
            lateTimes.push_back(probeTimes[k]);
            lateValues.push_back(probeValues[k]);
        }
    }
    double amplitude = 0.0;
    for (double value : lateValues) {
        amplitude = std::max(amplitude, std::fabs(value));
    }

    const std::vector<double> crossings = upwardCrossings(lateTimes, lateValues);
    std::printf("\nprobe amplitude over the second half of the run: %.5f\n", amplitude);
    if (crossings.size() >= 2) {
        const double period = (crossings.back() - crossings.front()) / (crossings.size() - 1);
        const double strouhal = kDiameter / (period * kStream);
        std::printf("%zu shedding cycles, period %.4f, Strouhal number %.4f (reference about 0.16)\n",
                    crossings.size() - 1, period, strouhal);
    } else {
        std::printf("no shedding detected: the wake is still steady, try a longer kEndTime\n");
    }

    if (!writeColumnsCsv("cylinderProbe.csv", {"time", "v"}, {probeTimes, probeValues})) {
        std::fprintf(stderr, "could not write cylinderProbe.csv\n");
        return 1;
    }
    std::printf("wrote cylinderProbe.csv and %d frames cylinderWake0000.ppm ...\n", frame);
    return 0;
}
