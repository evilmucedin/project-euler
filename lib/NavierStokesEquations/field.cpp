#include "lib/NavierStokesEquations/field.h"

#include <cmath>
#include <utility>

namespace navierStokes {

void Field2D::swap(Field2D& other) {
    std::swap(nx_, other.nx_);
    std::swap(ny_, other.ny_);
    std::swap(halo_, other.halo_);
    std::swap(strideX_, other.strideX_);
    data_.swap(other.data_);
}

double Field2D::maxAbs() const {
    double result = 0.0;
    for (int i = 0; i < nx_; ++i) {
        for (int j = 0; j < ny_; ++j) {
            result = std::max(result, std::fabs((*this)(i, j)));
        }
    }
    return result;
}

double Field2D::sum() const {
    // Neumaier compensated summation: the pressure solver sums fields of
    // O(nx*ny) entries to subtract a mean, and the compensation keeps that
    // mean accurate enough that repeated projections do not drift.
    double total = 0.0;
    double compensation = 0.0;
    for (int i = 0; i < nx_; ++i) {
        for (int j = 0; j < ny_; ++j) {
            const double value = (*this)(i, j);
            const double t = total + value;
            if (std::fabs(total) >= std::fabs(value)) {
                compensation += (total - t) + value;
            } else {
                compensation += (value - t) + total;
            }
            total = t;
        }
    }
    return total + compensation;
}

double Field2D::mean() const {
    const size_t count = static_cast<size_t>(nx_) * static_cast<size_t>(ny_);
    return count == 0 ? 0.0 : sum() / static_cast<double>(count);
}

}  // namespace navierStokes
