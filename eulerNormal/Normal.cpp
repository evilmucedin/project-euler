#include <fstream>

#include "glog/logging.h"

#include "lib/header.h"
#include "lib/math.h"

template<typename T>
void saveSample(T& x, const string& filename) {
    ofstream fOut(filename);
    static const size_t kN = 1000000;
    double sum = 0;
    double sum2 = 0;
    for (size_t i = 0; i < kN; ++i) {
        double value = x();
        fOut << value << endl;
        sum += value;
        sum2 += value*value;
    }
    LOG(INFO) << filename << " Mean: " << sum/kN << " Var: " << (sum2 - sum*sum/kN)/kN;
}

double rand01() {
    return static_cast<double>(rand())/RAND_MAX;
}

double normalNaive() {
    static const size_t kN = 100;
    double result = 0.0;
    for (size_t i = 0; i < kN; ++i) {
        result += rand01();
    }
    result -= 0.5*kN;
    result *= sqrt(12.0/kN);
    return result;
}

double normalBoxMuller() {
    double u1 = rand01();
    double u2 = rand01();
    while (u1 < 1e-10) {
        u1 = rand01();
    }
    return sqrt(-2.0*log(u1))*cos(2.0*M_PI*u2);
}

double normalDensity(double x) {
    return exp(-Sqr(x)/2.0)/sqrt(2.0*M_PI);
}

static constexpr size_t kBins = 1000;
static constexpr double kInf = 1e7;

struct InvCDGNormal {
    vector<double> x_;
    vector<double> y_;

    InvCDGNormal() {
        x_.push_back(-kInf);
        for (double dx : vector<double>{1.0, 0.1, 0.01, 0.001}) {
            for (double x = -1000.0*dx; x < 1000.0*dx; x += dx) {
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
            sum += normalDensity(x_[i]) * (x_[i] - x_[i - 1]);
            y_[i] = sum;
        }
        LOG(INFO) << "CDF sum: " << sum;
    }

    double operator()() const {
        double value = rand01();
        while (value >= y_.back()) {
            value = rand01();
        }
        auto it = lower_bound(y_.begin(), y_.end(), value);
        auto index = it - y_.begin();
        return x_[index] + (value - y_[index])/(y_[index + 1] - y_[index])*(x_[index + 1] - x_[index]);
    }
};

int main() {
    InvCDGNormal g;
    saveSample(g, "uniformInvCDF.csv");
    saveSample(rand01, "uniform.csv");
    saveSample(normalNaive, "normalNaive.csv");
    saveSample(normalBoxMuller, "normalBoxMuller.csv");
    return 0;
}
