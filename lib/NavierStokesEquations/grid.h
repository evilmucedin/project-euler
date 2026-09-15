#pragma once

#include <algorithm>
#include <cassert>

namespace navierStokes {

// Geometry of a uniform staggered grid (Arakawa C / MAC layout) covering the
// rectangle [0, lengthX] x [0, lengthY] with nx by ny cells.
//
//   p(i, j)  at ((i + 1/2) dx, (j + 1/2) dy),  i in [0, nx),  j in [0, ny)
//   u(i, j)  at (i dx,        (j + 1/2) dy),   i in [0, nx],  j in [0, ny)
//   v(i, j)  at ((i + 1/2) dx, j dy),          i in [0, nx),  j in [0, ny]
//
// Velocity components live on the cell faces they cross, so the discrete
// divergence of a cell is a difference of the four faces that bound it, and
// the pressure gradient that corrects a face is a difference of the two cells
// that share it. Those two operators are exact transposes of each other, which
// is why the projection below drives the divergence to round-off rather than to
// the tolerance of the pressure solve.
struct StaggeredGrid {
    StaggeredGrid(int nxCells, int nyCells, double sizeX, double sizeY)
        : nx(nxCells), ny(nyCells), lengthX(sizeX), lengthY(sizeY), dx(sizeX / nxCells), dy(sizeY / nyCells) {
        assert(nxCells > 0 && nyCells > 0);
        assert(sizeX > 0.0 && sizeY > 0.0);
    }

    // Coordinates of the sample points of each field.
    double xU(int i) const { return i * dx; }
    double yU(int j) const { return (j + 0.5) * dy; }
    double xV(int i) const { return (i + 0.5) * dx; }
    double yV(int j) const { return j * dy; }
    double xP(int i) const { return (i + 0.5) * dx; }
    double yP(int j) const { return (j + 0.5) * dy; }

    double cellArea() const { return dx * dy; }
    double minSpacing() const { return std::min(dx, dy); }

    int nx;
    int ny;
    double lengthX;
    double lengthY;
    double dx;
    double dy;
};

}  // namespace navierStokes
