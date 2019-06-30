#define ARMA_USE_BLAS
#define ARMA_USE_CXX11

#include "lib/header.h"

#include "armadillo/armadillo"
#include "glog/logging.h"

#include "lib/benchmark.h"
#include "lib/random.h"
#include "lib/timer.h"

struct SparseMatrixNaive {
    struct Element {
        size_t index_;
        double value_;
    };
    using Row = std::vector<Element>;
    using Matrix = std::vector<Row>;
    Matrix m;
};

SparseMatrixNaive gen(size_t n, double fill) {
    SparseMatrixNaive result;
    result.m.resize(n);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            if (rand01<double>() < fill) {
                result.m[i].emplace_back(SparseMatrixNaive::Element{j, rand01<double>()});
            }
        }
        result.m[i].shrink_to_fit();
    }
    return result;
}

DoubleVector genV(size_t n) {
    DoubleVector result(n);
    for (auto& x : result) {
        x = rand01<double>();
    }
    return result;
}

DoubleVector mulNaive(const SparseMatrixNaive& m, const DoubleVector& v) {
    DoubleVector result(v.size());
    for (size_t i = 0; i < m.m.size(); ++i) {
        double e = 0.0;
        for (const auto& i : m.m[i]) {
            e += v[i.index_] * i.value_;
        }
        result[i] = e;
    }
    return result;
}

arma::sp_mat naiveToArma(const SparseMatrixNaive& m) {
    auto n = m.m.size();
    arma::sp_mat result(n, n);
    for (size_t i = 0; i < n; ++i) {
        for (const auto& e : m.m[i]) {
            result(i, e.index_) = e.value_;
        }
    }
    return result;
}

arma::vec doubleVectorToArma(const DoubleVector& v) {
    arma::vec result(v.size());
    for (size_t i = 0; i < v.size(); ++i) {
        result[i] = v[i];
    }
    return result;
}

int main() {
    static constexpr size_t N = 5000;
    auto sm = gen(N, 0.03);
    auto v = genV(N);

    LOG(INFO) << sum(mulNaive(sm, v));
    benchmark("Naive", [&]() { mulNaive(sm, v); });

    auto armasm = naiveToArma(sm);
    auto armav = doubleVectorToArma(v);

    LOG(INFO) << arma::sum(armasm * armav);
    benchmark("Arma", [&]() { auto res = armasm * armav; });

    return 0;
}
