#include "lib/matrix.h"

ostream& operator<<(ostream& s, const Matrix& m) {
    for (u32 i = 0; i < m.dim_; ++i) {
        for (u32 j = 0; j < m.dim_; ++j) {
            s << m.data_[i][j] << " ";
        }
        s << endl;
    }
    s << endl;
    return s;
}

Matrix Matrix::invert() const {
    auto result = Matrix::one(dim_);

    constexpr double kEps = 1e-9;

    Matrix copy(*this);

    auto mulLine = [&](u32 line, double d) {
        for (u32 j = 0; j < dim_; ++j) {
            copy.data_[line][j] *= d;
        }
        for (u32 j = 0; j < dim_; ++j) {
            result.data_[line][j] *= d;
        }
    };

    auto addLine = [&](u32 target, u32 source, double c) {
        for (u32 j = 0; j < dim_; ++j) {
            copy.data_[target][j] += c * copy.data_[source][j];
        }
        for (u32 j = 0; j < dim_; ++j) {
            result.data_[target][j] += c * result.data_[source][j];
        }
    };

    auto swapLine = [&](u32 target, u32 source) {
        for (u32 j = 0; j < dim_; ++j) {
            std::swap(copy.data_[target][j], copy.data_[source][j]);
        }
        for (u32 j = 0; j < dim_; ++j) {
            std::swap(result.data_[target][j], result.data_[source][j]);
        }
    };

    for (u32 i = 0; i < dim_; ++i) {
        double mx = fabs(copy.data_[i][i]);
        u32 mxIndex = i;
        for (u32 j = i + 1; j < dim_; ++j) {
            double mji = fabs(copy.data_[j][i]);
            if (mji > mx) {
                mx = mji;
                mxIndex = j;
            }
        }
        swapLine(i, mxIndex);
        double d = copy.data_[i][i];
        if (fabs(d) > kEps) {
            mulLine(i, 1.0 / d);
        }
        for (u32 j = i + 1; j < dim_; ++j) {
            addLine(j, i, -copy.data_[j][i]);
        }
    }

    for (i32 i = dim_ - 1; i >= 0; --i) {
        for (u32 j = i + 1; j < dim_; ++j) {
            addLine(i, j, -copy.data_[i][j]);
        }
    }

    // cout << OUTLN(copy);
    // cout << OUTLN(result);
    // LOG(INFO) << (*this)*result;

    return result;
}

Matrix Matrix::cholesky() const {
    Matrix result(dim_);
    Matrix copy(*this);

    for (size_t i = 0; i < dim_; ++i) {
        double aSqRoot = sqrt(copy.data_[i][i]);
        result.data_[i][i] = aSqRoot;
        double aSqRootInv = 1.0 / aSqRoot;
        for (size_t j = i + 1; j < dim_; ++j) {
            result.data_[j][i] = copy.data_[j][i] * aSqRootInv;
        }
        double aInv = 1.0 / copy.data_[i][i];
        for (size_t j = i + 1; j < dim_; ++j) {
            for (size_t k = i + 1; k < dim_; ++k) {
                copy.data_[i][k] -= copy.data_[j][i] * copy.data_[k][i] * aInv;
            }
        }
    }

    return result;
}

double Matrix::norm2() const {
    double sum = 0.0;
    for (size_t i = 0; i < dim_; ++i) {
        for (size_t j = 0; j < dim_; ++j) {
            sum += sqr(data_[i][j]);
        }
    }
    return sqrt(sum);
}

Matrix Matrix::transpose() const {
    Matrix result(dim_);
    for (u32 i = 0; i < dim_; ++i) {
        for (u32 j = 0; j < dim_; ++j) {
            result.data_[j][i] = data_[i][j];
        }
    }
    return result;
}

Matrix Matrix::operator*(const Matrix& m) const {
    Matrix result(m.dim_);
    for (u32 i = 0; i < m.dim_; ++i) {
        for (u32 j = 0; j < m.dim_; ++j) {
            for (u32 k = 0; k < m.dim_; ++k) {
                result.data_[i][k] += data_[i][j] * m.data_[j][k];
            }
        }
    }
    return result;
}

Matrix Matrix::operator-(const Matrix& m) const {
    Matrix result(m.dim_);
    for (u32 i = 0; i < m.dim_; ++i) {
        for (u32 j = 0; j < m.dim_; ++j) {
            result.data_[i][j] = data_[i][j] - m.data_[i][j];
        }
    }
    return result;
}

Vector Matrix::operator*(const Vector& m) const {
    Vector result(dim_);
    for (u32 i = 0; i < dim_; ++i) {
        for (u32 j = 0; j < dim_; ++j) {
            result[j] += data_[i][j] * m[i];
        }
    }
    return result;
}

Matrix& Matrix::operator/=(double x) {
    for (u32 i = 0; i < dim_; ++i) {
        for (u32 j = 0; j < dim_; ++j) {
            data_[i][j] /= x;
        }
    }
    return *this;
}

Matrix Matrix::one(u32 dim) {
    Matrix m(dim);
    for (u32 i = 0; i < dim; ++i) {
        m.data_[i][i] = 1.0;
    }
    return m;
}

VectorPoint::VectorPoint() {}

VectorPoint::VectorPoint(u32 dim) : a_(dim) {}

u32 VectorPoint::dimension() const { return a_.size(); }

DoubleVector VectorPoint::asVector() const {
    auto a = a_;
    a.emplace_back(b_);
    return a;
}

ostream& operator<<(ostream& o, const VectorPoint& p) {
    o << "(" << p.b_ << ", " << p.a_ << ")";
    return o;
}

DoubleVector linearRegression(const VectorPoints& points) {
    assert(!points.empty());

    Matrix xtx(points[0].dimension());
    Vector xty(points[0].dimension());

    for (u32 iVectorPoint = 0; iVectorPoint < points.size(); ++iVectorPoint) {
        const auto& p = points[iVectorPoint];
        assert(p.dimension() == xty.size());
        for (size_t i = 0; i < p.dimension(); ++i) {
            for (size_t j = 0; j < p.dimension(); ++j) {
                xtx.data_[i][j] += p.a_[i] * p.a_[j];
            }
        }
        for (size_t i = 0; i < p.dimension(); ++i) {
            xty[i] += p.a_[i] * p.b_;
        }
    }
    xtx /= points.size();
    for (auto& x : xty) {
        x /= points.size();
    }

    return xtx.invert() * xty;
}
