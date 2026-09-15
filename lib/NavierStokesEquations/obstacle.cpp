#include "lib/NavierStokesEquations/obstacle.h"

#include <cassert>

namespace navierStokes {
namespace {

size_t maskIndex(const StaggeredGrid& grid, int i, int j) {
    return static_cast<size_t>(i) * static_cast<size_t>(grid.ny) + static_cast<size_t>(j);
}

}  // namespace

std::vector<char> solidNone(const StaggeredGrid& grid) {
    return std::vector<char>(static_cast<size_t>(grid.nx) * static_cast<size_t>(grid.ny), 0);
}

std::vector<char> solidDisk(const StaggeredGrid& grid, double centerX, double centerY, double radius) {
    std::vector<char> solid = solidNone(grid);
    const double radiusSquared = radius * radius;
    for (int i = 0; i < grid.nx; ++i) {
        for (int j = 0; j < grid.ny; ++j) {
            const double dx = grid.xP(i) - centerX;
            const double dy = grid.yP(j) - centerY;
            if (dx * dx + dy * dy <= radiusSquared) {
                solid[maskIndex(grid, i, j)] = 1;
            }
        }
    }
    return solid;
}

std::vector<char> solidRectangle(const StaggeredGrid& grid, double minX, double minY, double maxX, double maxY) {
    std::vector<char> solid = solidNone(grid);
    for (int i = 0; i < grid.nx; ++i) {
        for (int j = 0; j < grid.ny; ++j) {
            const double x = grid.xP(i);
            const double y = grid.yP(j);
            if (x >= minX && x <= maxX && y >= minY && y <= maxY) {
                solid[maskIndex(grid, i, j)] = 1;
            }
        }
    }
    return solid;
}

void solidUnion(std::vector<char>* target, const std::vector<char>& other) {
    assert(target != nullptr);
    assert(target->size() == other.size());
    for (size_t k = 0; k < other.size(); ++k) {
        if (other[k] != 0) {
            (*target)[k] = 1;
        }
    }
}

int solidCellCount(const std::vector<char>& solid) {
    int count = 0;
    for (char cell : solid) {
        if (cell != 0) {
            ++count;
        }
    }
    return count;
}

Field2D solidAsField(const StaggeredGrid& grid, const std::vector<char>& solid) {
    Field2D result(grid.nx, grid.ny, 0);
    if (solid.empty()) {
        return result;
    }
    assert(solid.size() == static_cast<size_t>(grid.nx) * static_cast<size_t>(grid.ny));
    for (int i = 0; i < grid.nx; ++i) {
        for (int j = 0; j < grid.ny; ++j) {
            result(i, j) = solid[maskIndex(grid, i, j)] != 0 ? 1.0 : 0.0;
        }
    }
    return result;
}

}  // namespace navierStokes
