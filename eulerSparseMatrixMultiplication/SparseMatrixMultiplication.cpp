#define ARMA_USE_BLAS
#define ARMA_USE_CXX11

#include "lib/header.h"

#include "armadillo/armadillo"
#include "glog/logging.h"
#include "mkl.h"

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
        for (const auto& j : m.m[i]) {
            e += v[j.index_] * j.value_;
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

struct SparseMatrixBLAS {
    int m_;
    std::vector<double> a_;
    std::vector<int> ia_;
    std::vector<int> ja_;
};

SparseMatrixBLAS naiveToBLAS(const SparseMatrixNaive& m) {
    SparseMatrixBLAS result;
    auto n = m.m.size();
    result.m_ = n;
    for (size_t i = 0; i < n; ++i) {
        result.ia_.emplace_back(result.a_.size());
        for (const auto& index : m.m[i]) {
            result.a_.emplace_back(index.value_);
            result.ja_.emplace_back(index.index_);
        }
    }
    result.ia_.emplace_back(result.a_.size());
    return result;
}

DoubleVector mulBLAS(const SparseMatrixBLAS& m, const DoubleVector& v) {
    DoubleVector result(m.m_);
    mkl_cspblas_dcsrgemv("N", &m.m_, m.a_.data(), m.ia_.data(), m.ja_.data(), v.data(), result.data());
    return result;
}

int main() {
    static constexpr size_t N = 6000;
    auto sm = gen(N, 0.03);
    auto v = genV(N);

    LOG(INFO) << sum(mulNaive(sm, v));
    benchmark("Naive", [&]() { mulNaive(sm, v); });

    auto armasm = naiveToArma(sm);
    auto armav = doubleVectorToArma(v);

    LOG(INFO) << arma::sum(armasm * armav);
    benchmark("Arma", [&]() { auto res = armasm * armav; });

    auto armasm2 = arma::mat(armasm);
    LOG(INFO) << arma::sum(armasm2 * armav);
    benchmark("ArmaD", [&]() { auto res = armasm2 * armav; });

    auto blassm = naiveToBLAS(sm);
    LOG(INFO) << sum(mulBLAS(blassm, v));
    benchmark("MKL", [&]() { mulBLAS(blassm, v); });

    return 0;
}
