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

int main() {
    size_t n = FLAGS_n;
    MatrixWeirdo a(n, n);
    MatrixWeirdo b(n, n);

    for (size_t iTest = 0; iTest < 3; ++iTest) {
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
    }

    return 0;
}
