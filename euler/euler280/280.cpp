#include "glog/logging.h"

#include "lib/header.h"

using DT = long double;

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
      IntVector counts(5);
      for (const auto &s : seeds_) {
        if (s.y_ != 4 || counts[s.x_]) {
          return false;
        }
        ++counts[s.x_];
      }
      return true;
    }

    bool operator<(const Position& p) const {
        if (ant_ != p.ant_) {
            return ant_ < p.ant_;
        }
        return seeds_ < p.seeds_;
    }

    vector<Position> next() const {
        static const vector<vector<int>> kDirs = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
        vector<Position> result;
        for (int i = 0; i < 4; ++i) {
            Position p = *this;
            Point& ant = p.ant_;
            ant.x_ += kDirs[i][0];
            if (ant.x_ < 0 || ant.x_ > 4) {
                continue;
            }
            ant.y_ += kDirs[i][1];
            if (ant.y_ < 0 || ant.y_ > 4) {
                continue;
            }
            vector<int> counts0(5);
            vector<int> counts4(5);
            for (auto& seed: p.seeds_) {
                if (seed == ant_) {
                    if (seed.y_ != 4 && seed.y_ != 0) {
                        seed = ant;
                    } else {
                        if (seed.y_ == 4) {
                            ++counts4[seed.x_];
                            if (counts4[seed.x_] > 1) {
                                seed = ant;
                            }
                        } else {
                            ++counts0[seed.x_];
                            if (counts0[seed.x_] == 1) {
                                seed = ant;
                            }
                        }
                    }
                }
            }
            sort(p.seeds_.begin(), p.seeds_.end());
            result.emplace_back(p);
        }
        return result;
    }
};

ostream& operator<<(ostream& s, const Point& p) {
    s << "(" << (int)p.x_ << ", " << (int)p.y_ << ")";
    return s;
}

ostream& operator<<(ostream& s, const Position& p) {
    s << "ant=" << p.ant_ << " {" << p.seeds_ << "}";
    return s;
}

int main() {
    Position p0;
    p0.ant_.x_ = 2;
    p0.ant_.y_ = 2;
    for (int i = 0; i < 5; ++i) {
        p0.seeds_[i].x_ = i;
        p0.seeds_[i].y_ = 0;
    }

    map<Position, int> pos2index;
    queue<Position> active;
    active.push(p0);
    while (!active.empty()) {
        Position p = active.front();
        active.pop();
        if (!pos2index.count(p)) {
            size_t index = pos2index.size();
            pos2index[p] = index;
            auto next = p.next();
            for (const auto& p: next) {
                active.push(p);
            }
        }
    }

    vector<IntVector> graph(pos2index.size());
    BoolVector isFinal(graph.size());
    int cFinal = 0;
    for (const auto& kv: pos2index) {
        if (kv.first.isFinal()) {
            isFinal[kv.second] = true;
            ++cFinal;
        }
        auto next = kv.first.next();
        for (const auto& n: next) {
            graph[kv.second].emplace_back(pos2index[n]);
        }
    }

    LOG(INFO) << "Graph is ready: " << cFinal << " " << pos2index.size();


    using Positions = vector<DT>;
    Positions positions(graph.size());
    positions[pos2index[p0]] = 1.0;
    DT result = 0.0;
    DT prob = 0.0;
    for (int step = 1; step < 10000; ++step) {
        Positions next(graph.size());

        for (int i = 0; i < graph.size(); ++i) {
            for (const auto& p: graph[i]) {
                next[p] += positions[i]/graph[i].size();
            }
        }

        DT sum = 0.0;
        for (int i = 0; i < graph.size(); ++i) {
            if (isFinal[i]) {
                result += step*next[i];
                prob += next[i];
                positions[i] = 0;
            } else {
                sum += next[i];
                positions[i] = next[i];
            }
        }

        LOG(INFO) << step << " " << OUT(positions.size()) << OUT(result) << OUT(prob);
        printf("%d\t%.6Lf\t%.6Lf\t%Lg\n", step, result, prob, sum);
    }

    cout << OUT(result) << OUT(prob);

    return 0;
}
