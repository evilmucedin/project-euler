#include <glog/logging.h>

#include "lib/header.h"
#include "lib/matrix.h"
#include "lib/random.h"
#include "lib/timer.h"

constexpr u32 kDim = 200;

Vector genRandomVector(u32 dim) {
    Vector result(dim);
    for (auto& x : result) {
        x = rand1<double>();
    }
    return result;
}

VectorPoint getRandomVectorPoint(const Vector& plane) {
    VectorPoint p(plane.size());
    p.b_ = rand1<double>();
    for (u32 i = 0; i < plane.size(); ++i) {
        p.a_[i] = rand1<double>();
        p.b_ += p.a_[i] * plane[i];
    }
    return p;
}

int main() {
    auto b = genRandomVector(kDim);

    Matrix xtx(kDim);
    Vector xty(kDim);

    {
        Timer tGen("Generate");
        constexpr u32 kVectorPoints = 4000 * kDim;
        for (u32 iVectorPoint = 0; iVectorPoint < kVectorPoints; ++iVectorPoint) {
            auto p = getRandomVectorPoint(b);
            for (size_t i = 0; i < p.dimension(); ++i) {
                for (size_t j = 0; j < p.dimension(); ++j) {
                    xtx.data_[i][j] += p.a_[i] * p.a_[j];
                }
            }
            for (size_t i = 0; i < p.dimension(); ++i) {
                xty[i] += p.a_[i] * p.b_;
            }
        }
    }

    {
        Timer tSolve("Invert");
        auto inv = xtx.invert();
        tSolve.finish();
        Vector bPrime = inv * xty;

        LOG(INFO) << OUT(b) << OUT(length(b));
        LOG(INFO) << OUT(bPrime) << OUT(length(bPrime));
        auto err = b - bPrime;
        LOG(INFO) << OUT(err) << OUT(length(err));
    }

    {
        Timer tSolve("Cholesky");
        auto chol = xtx.cholesky();
        tSolve.finish();
        Matrix xtx2 = chol*chol.transpose();
        LOG(INFO) << OUT(xtx.norm2()) << OUT(xtx2.norm2()) << OUT((xtx - xtx2).norm2());
    }

    return 0;
}
