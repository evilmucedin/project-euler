#pragma once

#include <vector>

#include "lib/NavierStokesEquations/field.h"
#include "lib/NavierStokesEquations/grid.h"

namespace navierStokes {

// Builders for the obstacle masks that NavierStokesSolver::setSolid takes. A
// mask has nx * ny entries indexed as i * ny + j, non-zero meaning solid. A
// cell is marked when its centre lies inside the shape, so a shape is resolved
// as a staircase of whole cells.

std::vector<char> solidNone(const StaggeredGrid& grid);
std::vector<char> solidDisk(const StaggeredGrid& grid, double centerX, double centerY, double radius);
std::vector<char> solidRectangle(const StaggeredGrid& grid, double minX, double minY, double maxX, double maxY);

// Adds `other` into `target` in place.
void solidUnion(std::vector<char>* target, const std::vector<char>& other);

int solidCellCount(const std::vector<char>& solid);

// The mask as a field of 0 and 1, for writing alongside the flow fields so
// that a plot can mask the obstacle out.
Field2D solidAsField(const StaggeredGrid& grid, const std::vector<char>& solid);

}  // namespace navierStokes
