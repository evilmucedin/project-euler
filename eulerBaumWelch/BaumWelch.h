/*
 * HMMarma.cpp
 *
 *  Created on: 3 févr. 2012
 *      Author: Pierre Pudlo
 *     (modified by: Aurelien Garivier)
 *
 *  Another way of programming the Baum-Welch algorithm for HMM
 *  with Armadillo, see http://arma.sourceforge.net/
 *
 *  Compile with:
 *  g++ -O3 HMMarma.cpp -o HMMarma -larmadillo
 *
 */

// disable run-time bound checks
#define ARMA_NO_DEBUG

#include <cstdlib>
#include <ctime>
#include <limits>
#include <utility>

#include <armadillo/armadillo>

using namespace arma;

class HMMSample {
   public:
    void sample(vec &nu, mat &Q, mat &g, size_t n);
    ivec x_;  // hidden states
    ivec y_;  // observations
};

void HMMSample::sample(vec &nu, mat &Q, mat &g, size_t n) {
    mat cQ = cumsum(Q, 1);
    mat cg = cumsum(g, 1);

    x_ = zeros<ivec>(n);
    y_ = zeros<ivec>(n);

    double random = (double)rand() / RAND_MAX;
    x_(0) = as_scalar(accu(cumsum(nu) < random));
    random = (double)rand() / RAND_MAX;
    y_(0) = as_scalar(accu(cg.row(x_(0)) < random));
    for (size_t j = 1; j < n; ++j) {
        random = (double)rand() / RAND_MAX;
        x_(j) = as_scalar(accu(cQ.row(x_(j - 1)) < random));
        random = (double)rand() / RAND_MAX;
        y_(j) = as_scalar(accu(cg.row(x_(j)) < random));
    }
}

class HMMFilter {
   public:
    void compute(ivec &y, vec &nu, mat &Q, mat &g);
    mat phi_;
    vec c_;
};

void HMMFilter::compute(ivec &y, vec &nu, mat &Q, mat &g) {
    const size_t n = y.n_rows;
    phi_ = zeros<mat>(Q.n_rows, n);
    c_ = zeros<vec>(n);

    vec Z = nu % g.col(y(0));
    c_(0) = accu(Z);
    phi_.col(0) = Z / c_(0);

    for (size_t t = 1; t < n; ++t) {
        Z = trans(trans(phi_.col(t - 1)) * Q) % g.col(y(t));
        c_(t) = accu(Z);
        phi_.col(t) = Z / c_(t);
    }
}

class HMMSmoother {
   public:
    void compute(ivec &y, mat &Q, mat &g, vec &c);
    mat betaa_;
};

void HMMSmoother::compute(ivec &y, mat &Q, mat &g, vec &c) {
    int n = y.n_rows;
    betaa_ = ones<mat>(Q.n_rows, n);
    for (int t = n - 2; t >= 0; --t) {
        betaa_.col(t) = Q * (g.col(y(t + 1)) % betaa_.col(t + 1)) / c(t + 1);
    }
}

class HMMBaumWelch {
   public:
    void compute(ivec &y, vec &nu, double tol = 1e-4, int maxIt = 100);
    mat q_;
    mat g_;
    double l_;
};

void HMMBaumWelch::compute(ivec &y, vec &nu, double tol, int maxIt) {
    size_t k = nu.n_rows;
    size_t r = max(y) + 1;
    size_t n = y.n_rows;
    imat Y = zeros<imat>(n, r);
    for (size_t i = 0; i < n; ++i) Y(i, y(i)) = 1;

    q_ = randu<mat>(k, k);
    q_ = q_ / (sum(q_, 1) * ones<mat>(1, k));
    g_ = randu<mat>(k, r);
    g_ = g_ / (sum(g_, 1) * ones<mat>(1, r));

    double it = 0;
    mat oldQ = q_;
    mat oldg = g_ + tol + 1;
    HMMFilter my_filter;
    HMMSmoother my_smoother;
    mat gaty = zeros<mat>(k, n - 1);
    while ((norm(oldQ - q_, 1) + norm(oldg - g_, 1) > tol) && (it < maxIt)) {
        ++it;
        // compute the posterior distribution for the current parameters

        my_filter.compute(y, nu, q_, g_);

        my_smoother.compute(y, q_, g_, my_filter.c_);
        mat post = my_filter.phi_ % my_smoother.betaa_;

        // expectation of the number of transitions under the current parameters

        for (size_t j = 0; j < n - 1; ++j) gaty.col(j) = g_.col(y(j + 1));

        mat N = q_ % (my_filter.phi_.cols(0, n - 2) * trans(my_smoother.betaa_.cols(1, n - 1) % gaty /
                                                            (ones<mat>(k, 1) * trans(my_filter.c_.subvec(1, n - 1)))));
        // cout << "N = "<< N << endl;
        // expectation of the number of emissions
        mat M = post * Y;
        // cout << "M = " << M << endl;

        // re-estimation
        oldQ = q_;
        oldg = g_;
        q_ = N / (sum(N, 1) * ones<mat>(1, k));
        g_ = M / (sum(M, 1) * ones<mat>(1, r));
    }
    l_ = accu(log(my_filter.c_));
}

/*
int main() {
    srand(time(NULL));
    // scenario 1

    vec nu = "0. 1.";
    mat Q = "0.8 0.2; 0.1 0.9";
    mat g = "0.25 0.25 0.25 0.25; 0.1 0.1 0.4 0.4";

    // scenario 2
    // int N = 7;
    // double epsilon=0.1;
    // vec nu = zeros<vec>(N);
    // nu(1)=1;
    // mat Q = zeros<mat>(N, N);
    // for (int i=0; i<N; i++) for(int j=0; j<N; j++)
    //         if (i==j) Q(i,j) = 1-2*epsilon;
    //         else if ((i==0 && j==1) || (i==N-1 && j==N-2)) Q(i,j) = 2*epsilon;
    //         else if (abs(i-j)==1) Q(i,j) = epsilon;
    //         else Q(i,j) = 0;
    // mat g=Q;

    int n = 10000;
    int nbReps = 10;
    mat tps = zeros<mat>(nbReps, 3);
    clock_t begin, end;

    for (int rep = 0; rep < nbReps; ++rep) {
        cout << "Replicate number " << rep + 1 << " over " << nbReps << endl;
        HMMsample my_sample;
        begin = clock();
        for (int j = 0; j < 100; ++j) my_sample.sample(nu, Q, g, n);
        end = clock();
        tps(rep, 0) = (end - begin) / (CLOCKS_PER_SEC / 1000.0) / 100;
        HMMfilter my_filter;
        HMMsmoother my_smoother;
        begin = clock();
        for (int j = 0; j < 1000; ++j) {
            my_filter.compute(my_sample._y, nu, Q, g);
            my_smoother.compute(my_sample._y, Q, g, my_filter._c);
            mat post = my_filter._phi % my_smoother._betaa;
        }
        end = clock();
        tps(rep, 1) = (end - begin) / (CLOCKS_PER_SEC / 1000.0) / 1000;
        // double l0 = accu(log(my_filter._c)); // log-likelihood of the observation
        // cout << "Log-likelihood of the observation : " << l0 << endl;

        // estimate parameters from observations only
        int nbTrials = 10;
        double bestl = -1e30;
        begin = clock();
        HMMbaumwelch mybw;
        HMMbaumwelch best;
        for (int j = 0; j < nbTrials; ++j) {
            mybw.compute(my_sample._y, nu);
            if (mybw._l > bestl) {
                best = mybw;
                bestl = mybw._l;
            }
        }
        end = clock();
        tps(rep, 2) = (end - begin) / (CLOCKS_PER_SEC / 1000.0);
        // cout << "Likelihood = " << bestl << endl;
        // cout << "hat Q = " << best._Q << endl;
        // cout << "hat g =" << best._g << endl;
    }

    cout << "Timer:" << endl;
    cout << tps << endl;

    cout << "Average :" << endl;
    cout << mean(tps) << endl;

    return 0;
}
*/
