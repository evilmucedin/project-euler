#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <vector>

namespace navierStokes {

// A 2D array of doubles with a ghost ("halo") border of `halo` cells on every
// side. Logical indices run over [0, nx) x [0, ny); ghost cells are addressed
// with indices in [-halo, 0) and [n, n + halo).
//
// The staggered-grid stencils in this library read one cell past the domain
// when they update a face next to the boundary, and boundary conditions are
// imposed by writing into that ghost layer. That keeps the interior update
// loops free of per-side special cases.
class Field2D {
   public:
    Field2D() = default;

    Field2D(int nx, int ny, int halo = 1)
        : nx_(nx),
          ny_(ny),
          halo_(halo),
          strideX_(ny + 2 * halo),
          data_(static_cast<size_t>(nx + 2 * halo) * static_cast<size_t>(ny + 2 * halo), 0.0) {
        assert(nx >= 0 && ny >= 0 && halo >= 0);
    }

    double& operator()(int i, int j) { return data_[index(i, j)]; }
    const double& operator()(int i, int j) const { return data_[index(i, j)]; }

    int nx() const { return nx_; }
    int ny() const { return ny_; }
    int halo() const { return halo_; }

    void fill(double value) { std::fill(data_.begin(), data_.end(), value); }
    void swap(Field2D& other);

    // Reductions over the logical cells only; ghost cells are ignored.
    double maxAbs() const;
    double sum() const;
    double mean() const;

   private:
    size_t index(int i, int j) const {
        assert(i >= -halo_ && i < nx_ + halo_);
        assert(j >= -halo_ && j < ny_ + halo_);
        return static_cast<size_t>(i + halo_) * static_cast<size_t>(strideX_) + static_cast<size_t>(j + halo_);
    }

    int nx_ = 0;
    int ny_ = 0;
    int halo_ = 0;
    int strideX_ = 0;
    std::vector<double> data_;
};

}  // namespace navierStokes
