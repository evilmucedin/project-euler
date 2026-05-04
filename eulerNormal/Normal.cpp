#include <cassert>
#include <fstream>
#include <random>

#include "glog/logging.h"

#include "lib/header.h"
#include "lib/math.h"
#include "lib/random.h"
#include "lib/stat/stat.h"
#include "lib/timer.h"

template <typename T>
void saveSample(T& x, const string& filename) {
    Timer t(string("saveSample: ") + typeid(T).name());
    ofstream fOut(filename);
    constexpr size_t kN = 1000000;
    StatAccumulator sa;
    for (size_t i = 0; i < kN; ++i) {
        double value = x();
        fOut << value << endl;
        sa.add(value);
    }
    LOG(INFO) << filename << " Mean: " << sa.average() << " Var: " << sa.variance() << " Min: " << sa.min() << " Max: " << sa.max();
}

double normalNaive() {
    constexpr size_t kN = 100;
    double result = 0.0;
    for (size_t i = 0; i < kN; ++i) {
        result += rand01<double>();
    }
    result -= 0.5 * kN;
    result *= sqrt(12.0 / kN);
    return result;
}

double normalBoxMuller() {
    double u1 = rand01<double>();
    double u2 = rand01<double>();
    while (u1 < 1e-10) {
        u1 = rand01<double>();
    }
    return sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
}

double normalDensity(double x) { return exp(-sqr(x) / 2.0) / sqrt(2.0 * M_PI); }

constexpr double kBins = 1000.0;
constexpr double kInf = 500.0;

struct InvCDFNormal {
    DoubleVector x_;
    DoubleVector y_;

    InvCDFNormal() {
        x_.push_back(-kInf);
        double dx0 = kInf / kBins;
        for (double dx : vector<double>{dx0, dx0 / 10, dx0 / 100, dx0 / 1000}) {
            for (double x = -dx * kBins; x < dx * kBins; x += dx) {
                x_.emplace_back(x);
            }
        }
        x_.push_back(kInf);
        sort(x_.begin(), x_.end());
        x_.erase(unique(x_.begin(), x_.end()), x_.end());
        double sum = 0.0;
        y_.resize(x_.size());
        y_[0] = 0.0;
        for (size_t i = 1; i < x_.size(); ++i) {
            sum += (normalDensity(x_[i]) + normalDensity(x_[i - 1])) / 2.0 * (x_[i] - x_[i - 1]);
            y_[i] = sum;
        }
        for (size_t i = 1; i < x_.size();) {
            if (y_[i] < y_[i - 1] + 1e-6) {
                x_.erase(x_.begin() + i);
                y_.erase(y_.begin() + i);
            } else {
                ++i;
            }
        }
        LOG(INFO) << "CDF sum: " << sum << " Points: " << x_.size() << " Limit: " << normalDensity(kInf);
    }

    double operator()() const {
        double value = rand01<double>();
        while (value >= y_.back()) {
            value = rand01<double>();
        }
        auto it = lower_bound(y_.begin(), y_.end(), value);
        if (it + 1 == y_.end()) {
            --it;
        }
        auto index = it - y_.begin();
        return x_[index] + (value - y_[index]) / (y_[index + 1] - y_[index]) * (x_[index + 1] - x_[index]);
    }
};

double randMC() {
    double x;
    double y;
    do {
        y = rand01<double>();
        x = (rand01<double>() - 0.5) * 1000.0;
    } while (y > normalDensity(x));
    return x;
}

double normalStd() {
    static normal_distribution<double> distribution(0.0, 1.0);
    return distribution(getRandomNumberGenerator());
}

int main() {
    InvCDFNormal g;
    saveSample(g, "normalInvCDF.csv");
    saveSample(normalStd, "normalStd.csv");
    saveSample(normalNaive, "normalNaive.csv");
    saveSample(normalBoxMuller, "normalBoxMuller.csv");
    saveSample(randMC, "normalMC.csv");
    saveSample(rand01<double>, "uniform.csv");
    return 0;
}
