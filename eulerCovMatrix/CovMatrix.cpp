#include <glog/logging.h>

#include "lib/header.h"
#include "lib/random.h"

constexpr u32 kDim = 40;

using Vector = DoubleVector;

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

struct Accumulator {
  Accumulator() : n_(0), x_(0.0), y_(0.0), xy_(0.0) {}

  void add(double x, double y) {
    ++n_;
    x_ += x;
    y_ += y;
    xy_ += x*y;
  }

  double covariance() const { return xy_ / n_ - (x_ / n_) * (y_ / n_); }

  double xy() const { return xy_; }

  u64 n_;
  double x_;
  double y_;
  double xy_;
};

struct Matrix {
  Matrix(u32 dim) : dim_(dim), data_(dim_, Vector(dim_)) {}
  Matrix(const Matrix& m) : dim_(m.dim_), data_(m.data_) {}

  Matrix invert() const;
  Matrix transpose() const;
  Matrix operator*(const Matrix& m) const;
  Vector operator*(const Vector& m) const;

  u32 dim_;
  using Data = vector<Vector>;
  Data data_;
};

ostream& operator<<(ostream& s, const Matrix& m) {
    s << endl;
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
  Matrix result(dim_);
  for (u32 i = 0; i < dim_; ++i) {
    result.data_[i][i] = 1.0;
  }

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

  for (u32 i = 0; i < dim_; ++i) {
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

  // LOG(INFO) << copy;
  // LOG(INFO) << result;
  // LOG(INFO) << (*this)*result;

  return result;
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
                result.data_[i][k] += data_[i][j]*m.data_[j][k];
            }
        }
    }
    return result;
}

Vector Matrix::operator*(const Vector& m) const {
    Vector result(dim_);
    for (u32 i = 0; i < dim_; ++i) {
        for (u32 j = 0; j < dim_; ++j) {
            result[j] += data_[i][j]*m[i];
        }
    }
    return result;
}

int main() {
  auto b = genRandomPoint(kDim);

  vector<vector<Accumulator>> accs(kDim, vector<Accumulator>(kDim));
  Vector xty(kDim);

  constexpr u32 kPoints = 1000*kDim;
  for (u32 iPoint = 0; iPoint < kPoints; ++iPoint) {
    auto p = getRandomPoint(b);
    for (size_t i = 0; i < p.dimension(); ++i) {
        for (size_t j = 0; j < p.dimension(); ++j) {
            accs[i][j].add(p.a_[i], p.a_[j]);
        }
    }
    for (size_t i = 0; i < p.dimension(); ++i) {
        xty[i] += p.a_[i]*p.b_;
    }
  }

  Matrix xtx(kDim);

  for (size_t i = 0; i < accs.size(); ++i) {
    for (size_t j = 0; j < accs.size(); ++j) {
        xtx.data_[i][j] = accs[i][j].xy();
    }
  }

  Matrix inv = xtx.invert();
  Vector bPrime = inv*xty;

  LOG(INFO) << OUT(b) << OUT(length(b));
  LOG(INFO) << OUT(bPrime) << OUT(length(bPrime));
  auto err = b - bPrime;
  LOG(INFO) << OUT(err) << OUT(length(err));

  return 0;
}
