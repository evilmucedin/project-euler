#include "lib/header.h"

struct Point {
    char x_;
    char y_;

    bool operator<(const Point& p) const {
        if (x_ != p.x_) {
            return x_ < p.x_;
        }
        return y_ < p.y_;
    }

    bool operator==(const Point& p) const {
        return (x_ == p.x_) && (y_ == p.y_);
    }

    bool operator!=(const Point& p) const {
        return !(*this == p);
    }
};

using Points = vector<Point>;

struct Position {
    Point ant_;
    vector<Point> seeds_{5};

    bool isFinal() const {
      for (const auto &s : seeds_) {
        if (s.y_ != 4) {
          return false;
        }
      }
      return true;
    }

    bool operator<(const Position& p) const {
        if (ant_ != p.ant_) {
            return ant_ < p.ant_;
        }
        return seeds_ < p.seeds_;
    }
};

using Positions = map<Position, double>;

int main() {
    Positions positions;

    {
      Position p0;
      p0.ant_.x_ = 2;
      p0.ant_.y_ = 2;
      for (int i = 0; i < 5; ++i) {
        p0.seeds_[i].x_ = i;
        p0.seeds_[i].y_ = 0;
      }
      positions[p0] = 1.0;
    }

    double result = 0.0;

    for (int step = 1; step < 1000; ++step) {
    }

    cout << OUT(result);

    return 0;
}
