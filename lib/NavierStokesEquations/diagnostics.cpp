#include "lib/NavierStokesEquations/diagnostics.h"

#include <cmath>

namespace navierStokes {

Field2D divergenceField(const StaggeredGrid& grid, const Field2D& u, const Field2D& v) {
    Field2D result(grid.nx, grid.ny, 0);
    for (int i = 0; i < grid.nx; ++i) {
        for (int j = 0; j < grid.ny; ++j) {
            result(i, j) = (u(i + 1, j) - u(i, j)) / grid.dx + (v(i, j + 1) - v(i, j)) / grid.dy;
        }
    }
    return result;
}

Field2D vorticityAtCorners(const StaggeredGrid& grid, const Field2D& u, const Field2D& v) {
    Field2D result(grid.nx + 1, grid.ny + 1, 0);
    for (int i = 0; i <= grid.nx; ++i) {
        for (int j = 0; j <= grid.ny; ++j) {
            result(i, j) = (v(i, j) - v(i - 1, j)) / grid.dx - (u(i, j) - u(i, j - 1)) / grid.dy;
        }
    }
    return result;
}

Field2D vorticityField(const StaggeredGrid& grid, const Field2D& u, const Field2D& v) {
    const Field2D corners = vorticityAtCorners(grid, u, v);
    Field2D result(grid.nx, grid.ny, 0);
    for (int i = 0; i < grid.nx; ++i) {
        for (int j = 0; j < grid.ny; ++j) {
            result(i, j) = 0.25 * (corners(i, j) + corners(i + 1, j) + corners(i, j + 1) + corners(i + 1, j + 1));
        }
    }
    return result;
}

void velocityAtCenters(const StaggeredGrid& grid, const Field2D& u, const Field2D& v, Field2D* uCenter,
                       Field2D* vCenter) {
    if (uCenter != nullptr) {
        *uCenter = Field2D(grid.nx, grid.ny, 0);
    }
    if (vCenter != nullptr) {
        *vCenter = Field2D(grid.nx, grid.ny, 0);
    }
    for (int i = 0; i < grid.nx; ++i) {
        for (int j = 0; j < grid.ny; ++j) {
            if (uCenter != nullptr) {
                (*uCenter)(i, j) = 0.5 * (u(i, j) + u(i + 1, j));
            }
            if (vCenter != nullptr) {
                (*vCenter)(i, j) = 0.5 * (v(i, j) + v(i, j + 1));
            }
        }
    }
}

Field2D speedField(const StaggeredGrid& grid, const Field2D& u, const Field2D& v) {
    Field2D result(grid.nx, grid.ny, 0);
    for (int i = 0; i < grid.nx; ++i) {
        for (int j = 0; j < grid.ny; ++j) {
            const double uc = 0.5 * (u(i, j) + u(i + 1, j));
            const double vc = 0.5 * (v(i, j) + v(i, j + 1));
            result(i, j) = std::hypot(uc, vc);
        }
    }
    return result;
}

double kineticEnergy(const StaggeredGrid& grid, const Field2D& u, const Field2D& v) {
    double total = 0.0;
    for (int i = 0; i < grid.nx; ++i) {
        for (int j = 0; j < grid.ny; ++j) {
            const double uc = 0.5 * (u(i, j) + u(i + 1, j));
            const double vc = 0.5 * (v(i, j) + v(i, j + 1));
            total += 0.5 * (uc * uc + vc * vc);
        }
    }
    return total * grid.cellArea();
}

}  // namespace navierStokes
