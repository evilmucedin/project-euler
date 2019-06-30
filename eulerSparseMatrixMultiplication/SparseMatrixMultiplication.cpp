#include "lib/header.h"

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

int main() {
    static constexpr size_t N = 5000;
    auto sm = gen(N, 0.03);
    auto v = genV(N);

    benchmark("Naive", [&]() { mulNaive(sm, v); });

    return 0;
}
