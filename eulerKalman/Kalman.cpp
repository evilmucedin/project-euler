#include "lib/header.h"
#include "lib/random.h"

#include <armadillo/armadillo>

struct Kalman {
   public:
    Kalman(size_t n_out, size_t n_states) : n_out_(n_out) {
        x_ = arma::colvec(n_states, arma::fill::zeros);

        a_ = arma::mat(n_states, n_states, arma::fill::zeros);
        for (size_t i = 0; i < n_states; ++i) {
            for (size_t j = i; j < n_states; ++j) {
                a_(i, j) = 1.0 / std::pow(10.0, j - i);
            }
        }

        i_ = arma::eye(n_states, n_states);

        h_ = arma::mat(n_out, n_states, arma::fill::zeros);
        for (size_t i = 0; i < n_out; ++i) {
            h_(i, i) = 1.0;
        }

        q_ = 0.1 * arma::eye(n_states, n_states);
        r_ = 0.1 * arma::eye(n_out, n_out);
        p_ = arma::zeros(n_states, n_states);
    }

    void update(const DoubleVector& z) {
        x_hat_ = a_ * x_;
        p_ = a_ * p_ * a_.t() + q_;
        const arma::colvec y = arma::colvec(z) - h_ * x_hat_;
        const arma::mat s = h_ * p_ * h_.t() + r_;
        k_ = p_ * h_.t() * s.i();
        x_hat_ = x_hat_ + k_ * y;
        p_ = (i_ - k_ * h_) * p_;
        x_ = x_hat_;
    }

    DoubleVector state() const {
        DoubleVector result(n_out_);
        const arma::vec s = h_ * x_;
        for (size_t i = 0; i < n_out_; ++i) {
            result[i] = s(i);
        }
        return result;
    }

   private:
    size_t n_out_;
    arma::colvec x_;
    arma::colvec x_hat_;
    arma::mat a_;
    arma::mat h_;
    arma::mat q_;
    arma::mat r_;
    arma::mat k_;
    arma::mat p_;
    arma::mat i_;
};

int main() {
    Kalman k(1, 2);
    double sum1 = 0;
    double sum2 = 0;
    for (size_t i = 0; i < 100000; ++i) {
        const double value = std::sin(static_cast<double>(i) / 1000.);
        const double noise_value = value + 0.1 * randNorm01<double>();
        k.update({noise_value});
        sum1 += std::abs(noise_value - value);
        sum2 += std::abs(k.state()[0] - value);
        std::cout << (noise_value - value) << " " << (k.state()[0] - value) << std::endl;
    }
    std::cout << sum2 / sum1 << std::endl;
    return 0;
}
