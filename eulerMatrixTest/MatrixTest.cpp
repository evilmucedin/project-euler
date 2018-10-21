#include "lib/header.h"
#include "lib/timer.h"
#include "lib/random.h"

#include "gflags/gflags.h"

DEFINE_int64(n, 1000, "matrix size");

struct MatrixWeirdo {
    MatrixWeirdo(size_t m, size_t n) : m_(m), n_(n), data_(new double[m_ * n_]) {}

    double& at(size_t i, size_t j) {
        return data_[i*n_ + j];
    }

    const double& at(size_t i, size_t j) const {
        return data_[i*n_ + j];
    }

    void fillStupid() {
        for (size_t i = 0; i < m_; ++i) {
            for (size_t j = 0; j < n_; ++j) {
                at(i, j) = rand01<double>();
            }
        }
    }

    void fill() {
        double* p = data_.get();
        for (size_t i = 0; i < m_*n_; ++i) {
            *p = rand01<double>();
            ++p;
        }
    }

    double sum() const {
        double result = 0;
        double* p = data_.get();
        for (size_t i = 0; i < m_*n_; ++i) {
            result += *p;
            ++p;
        }
        return result;
    }

    MatrixWeirdo t() const {
        MatrixWeirdo result(n_, m_);
        for (size_t i = 0; i < m_; ++i) {
            for (size_t j = 0; j < n_; ++j) {
                result.at(j, i) = at(i, j);
            }
        }
        return result;
    }

    size_t m_;
    size_t n_;
    std::unique_ptr<double[]> data_;
};

ostream& operator<<(ostream& s, const MatrixWeirdo& m) {
    s << "[";
    for (size_t i = 0; i < m.m_; ++i) {
        if (i) {
            cout << "\n";
        }
        cout << "[";
        for (size_t j = 0; j < m.n_; ++j) {
            if (j) {
                cout << ", ";
            }
            cout << m.at(i, j);
        }
        cout << "]";
    }
    s << "]" << endl;
    return s;
}

MatrixWeirdo mulStupid(const MatrixWeirdo& a, const MatrixWeirdo& b) {
    ASSERTEQ(a.n_, b.m_);
    MatrixWeirdo result(a.m_, b.n_);
    for (size_t i = 0; i < a.m_; ++i) {
        for (size_t j = 0; j < b.n_; ++j) {
            double sum = 0.;
            for (size_t k = 0; k < a.n_; ++k) {
                sum += a.at(i, k) * b.at(k, j);
            }
            result.at(i, j) = sum;
        }
    }
    return result;
}

MatrixWeirdo mulT(const MatrixWeirdo& a, const MatrixWeirdo& b) {
    ASSERTEQ(a.n_, b.m_);
    MatrixWeirdo result(a.m_, b.n_);
    auto bt = b.t();
    for (size_t i = 0; i < a.m_; ++i) {
        for (size_t j = 0; j < b.n_; ++j) {
            double sum = 0.;
            for (size_t k = 0; k < a.n_; ++k) {
                sum += a.at(i, k) * bt.at(j, k);
            }
            result.at(i, j) = sum;
        }
    }
    return result;
}

MatrixWeirdo mul(const MatrixWeirdo& a, const MatrixWeirdo& b) {
    ASSERTEQ(a.n_, b.m_);
    MatrixWeirdo result(a.m_, b.n_);
    for (size_t i = 0; i < a.m_; ++i) {
        for (size_t j = 0; j < b.n_; ++j) {
            double sum = 0.;
            const double* toA = &a.at(i, 0);
            const double* toB = &b.at(0, j);
            for (size_t k = 0; k < a.n_; ++k) {
                sum += *toA * *toB;
                ++toA;
                toB += b.n_;
            }
            result.at(i, j) = sum;
        }
    }
    return result;
}

int main() {
    size_t n = FLAGS_n;

    for (size_t iTest = 0; iTest < 3; ++iTest) {
        MatrixWeirdo a(n, n);
        MatrixWeirdo b(n, n);
        {
            Timer t("Fill stupid");
            a.fillStupid();
            b.fillStupid();
        }
        {
            Timer t("Fill");
            a.fillStupid();
            b.fillStupid();
        }
        {
            Timer t("Mul stupid");
            cout << "Mul stupid result: " << mulStupid(a, b).sum() << endl;
        }
        {
            Timer t("Mul");
            cout << "Mul result: " << mul(a, b).sum() << endl;
        }
        {
            Timer t("MulT");
            cout << "MulT result: " << mulT(a, b).sum() << endl;
        }
    }

    return 0;
}
