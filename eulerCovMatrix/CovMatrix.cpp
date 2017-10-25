#include <glog/logging.h>

#include "lib/header.h"
#include "lib/random.h"

constexpr u32 kDim = 10;

struct Point {
  Point() {}

  Point(u32 dim) : a_(dim) {}

  u32 dimension() const { return a_.size(); }

  DoubleVector getVector() const {
    auto a = a_;
    a.emplace_back(b_);
    return a;
  }

  DoubleVector a_;
  double b_;
};

ostream& operator<<(ostream& o, const Point& p) {
  o << "(" << p.b_ << ", " << p.a_ << ")";
  return o;
}

Point genPoint(u32 dim) {
  Point result(dim);
  for (auto& x : result.a_) {
    x = rand1<double>();
  }
  result.b_ = rand1<double>();
  return result;
}

Point getRandomPoint(const Point& plane) {
  Point p(plane.dimension());
  p.b_ = plane.b_;
  for (u32 i = 0; i < plane.dimension(); ++i) {
    p.a_[i] = rand1<double>();
    p.b_ += p.a_[i] * plane.a_[i];
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

  u64 n_;
  double x_;
  double y_;
  double xy_;
};

int main() {
  auto plane = genPoint(kDim);
  auto vctPlane = plane.getVector();

  vector<vector<Accumulator>> accs(kDim + 1, vector<Accumulator>(kDim + 1));

  constexpr u32 kPoints = 1000;
  for (u32 iPoint = 0; iPoint < kPoints; ++iPoint) {
    auto p = getRandomPoint(plane).getVector();
    for (size_t i = 0; i < p.size(); ++i) {
        for (size_t j = 0; j < p.size(); ++j) {
            accs[i][j].add(p[i], p[j]);
        }
    }
  }

  for (size_t i = 0; i < accs.size(); ++i) {
    for (size_t j = 0; j < accs.size(); ++j) {
        LOG(INFO) << OUT(i) << OUT(j) << OUT(accs[i][j].covariance()) << OUT(vctPlane[i]);
    }
  }

  return 0;
}
