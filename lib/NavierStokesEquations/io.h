#pragma once

#include <string>
#include <vector>

#include "lib/NavierStokesEquations/field.h"
#include "lib/NavierStokesEquations/grid.h"

namespace navierStokes {

// Output helpers with no dependency beyond the standard library, so that the
// examples can be plotted with examples/plot.py or viewed directly.
// Every writer returns false when the file cannot be opened for writing.

// One line per grid row j, ascending in y; one comma-separated column per i,
// ascending in x. numpy.loadtxt(path, delimiter=",") gives an array indexed
// [j][i], which matplotlib.imshow renders correctly with origin="lower".
bool writeFieldCsv(const std::string& path, const Field2D& field);

// Long form with coordinates: "x,y,u,v,speed", one line per cell, velocities
// interpolated to the cell centres. Suited to quiver and streamline plots.
bool writeVelocityCsv(const std::string& path, const StaggeredGrid& grid, const Field2D& u, const Field2D& v);

// Named columns of equal length, e.g. a centreline profile or a time series.
bool writeColumnsCsv(const std::string& path, const std::vector<std::string>& headers,
                     const std::vector<std::vector<double>>& columns);

// Binary PPM (P6) image of a field, using a blue-white-red diverging colour
// map over [low, high] with values outside the range clamped. Cells where
// `mask` is non-zero are painted grey; pass null for no mask. Row order is
// flipped so that +y points up in the image.
bool writePpm(const std::string& path, const Field2D& field, double low, double high,
              const Field2D* mask = nullptr);

}  // namespace navierStokes
