#include <glog/logging.h>

#include "lib/header.h"
#include "lib/matrix.h"
#include "lib/random.h"
#include "lib/timer.h"

constexpr u32 kDim = 200;

struct Point {
    Point() {}

    Point(u32 dim) : a_(dim) {}

    u32 dimension() const { return a_.size(); }

    Vector getVector() const {
        auto a = a_;
        a.emplace_back(b_);
        return a;
    }

    Vector a_;
    double b_;
};

ostream& operator<<(ostream& o, const Point& p) {
    o << "(" << p.b_ << ", " << p.a_ << ")";
    return o;
}

Vector genRandomPoint(u32 dim) {
    Vector result(dim);
    for (auto& x : result) {
        x = rand1<double>();
    }
    return result;
}

Point getRandomPoint(const Vector& plane) {
    Point p(plane.size());
    p.b_ = rand1<double>();
    for (u32 i = 0; i < plane.size(); ++i) {
        p.a_[i] = rand1<double>();
        p.b_ += p.a_[i] * plane[i];
    }
    return p;
}

int main() {
    auto b = genRandomPoint(kDim);

    Matrix xtx(kDim);
    Vector xty(kDim);

    {
        Timer tGen("Generate");
        constexpr u32 kPoints = 4000 * kDim;
        for (u32 iPoint = 0; iPoint < kPoints; ++iPoint) {
            auto p = getRandomPoint(b);
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
        Matrix inv = xtx.invert();
        tSolve.finish();
        Vector bPrime = inv * xty;

        LOG(INFO) << OUT(b) << OUT(length(b));
        LOG(INFO) << OUT(bPrime) << OUT(length(bPrime));
        auto err = b - bPrime;
        LOG(INFO) << OUT(err) << OUT(length(err));
    }

    {
        Timer tSolve("Cholesky");
        Matrix chol = xtx.cholesky();
        tSolve.finish();
        Matrix xtx2 = chol*chol.transpose();
        LOG(INFO) << OUT(xtx.norm2()) << OUT(xtx2.norm2()) << OUT((xtx - xtx2).norm2());
    }

    return 0;
}
