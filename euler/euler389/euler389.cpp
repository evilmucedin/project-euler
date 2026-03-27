#include <cstdio>

#include <vector>

using namespace std;

typedef vector<double> TDoubleVector;

TDoubleVector facts;

static void Dice(const TDoubleVector& init, size_t nDice, TDoubleVector* result) {
    const size_t maxSum = (init.size() - 1)*nDice + 1;
    result->resize(maxSum, 0.);
    TDoubleVector counts(maxSum);
    counts[0] = 1.;
    for (size_t i = 1; i < init.size(); ++i) {
        if (i % 100 == 0)
            fprintf(stderr, "%d/%d\n", (int)i, (int)init.size());
        TDoubleVector nextCounts(maxSum);
        for (size_t k = 0; k < maxSum; ++k) {
            if (counts[k]) {
                for (size_t j = 1; j <= nDice; ++j) {
                    if (k + j < maxSum)
                        nextCounts[k + j] += counts[k]/nDice;
                }
            }
        }
        counts.swap(nextCounts);
        for (size_t j = 0; j < maxSum; ++j)
            (*result)[j] += init[i]*counts[j];
    }
    double sum = 0.;
    for (size_t i = 0; i < result->size(); ++i)
        sum += result->at(i);
    fprintf(stderr, "Sum: %lf\n", sum);
}

template<typename T>
T Sqr(T x) {
    return x*x;
}

static double Variance(const TDoubleVector& v) {
    double mean = 0.;
    for (size_t i = 0; i < v.size(); ++i)
        mean += v[i]*i;
    double variance = 0.;
    for (size_t i = 0; i < v.size(); ++i)
        variance += v[i]*Sqr(mean - (double)i);
    fprintf(stderr, "Mean: %lf\n", mean);
    return variance;
}

int main() {
    TDoubleVector step0(2, 0.);
    step0[1] = 1.0;

    TDoubleVector step1;
    Dice(step0, 4, &step1);
    TDoubleVector step2;
    Dice(step1, 6, &step2);
    TDoubleVector step3;
    Dice(step2, 8, &step3);
    printf("%.4lf\n", Variance(step3));
    TDoubleVector step4;
    Dice(step3, 12, &step4);
    printf("%.4lf\n", Variance(step4));
    TDoubleVector step5;
    Dice(step4, 20, &step5);
    printf("%.4lf\n", Variance(step5));

    return 0;
}