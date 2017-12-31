#include "lib/fft.h"
#include "lib/random.h"

#include "gtest/gtest.h"

using namespace fft;

namespace {
static double maxLogError = -INFINITY;

ComplexVector randomComplexes(size_t n) {
    ComplexVector result(n);
    for (size_t i = 0; i < n; i++) {
        result[i] = complex<double>(rand1<double>(), rand1<double>());
    }
    return result;
}

ComplexVector naiveDft(const ComplexVector& input, bool inverse) {
    int n = static_cast<int>(input.size());
    ComplexVector output;
    double coef = (inverse ? 2 : -2) * M_PI / n;
    for (int k = 0; k < n; k++) {
        complex<double> sum(0);
        for (int t = 0; t < n; t++) {
            double angle = coef * (static_cast<long long>(t) * k % n);
            sum += input[t] * std::exp(complex<double>(0, angle));
        }
        output.emplace_back(sum);
    }
    return output;
}

double log10RmsErr(const ComplexVector& xvec, const ComplexVector& yvec) {
    int n = static_cast<int>(xvec.size());
    double err = std::pow(10, -99 * 2);
    for (int i = 0; i < n; i++) {
        err += std::norm(xvec.at(i) - yvec.at(i));
    }
    err /= n > 0 ? n : 1;
    err = std::sqrt(err);
    err = std::log10(err);
    maxLogError = std::max(err, maxLogError);
    return err;
}

void testFft(int n) {
    const auto input = randomComplexes(n);
    const auto refout = naiveDft(input, false);
    ComplexVector actualout = input;
    fft::transform(actualout);
    cout << "fftsize=" << std::setw(4) << std::setfill(' ') << n << "  "
         << "logerr=" << std::setw(5) << std::setprecision(3) << std::setiosflags(std::ios::showpoint)
         << log10RmsErr(refout, actualout) << endl;
}

ComplexVector naiveConvolve(const ComplexVector& xvec, const ComplexVector& yvec) {
    int n = static_cast<int>(xvec.size());
    ComplexVector outvec(n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int k = (i + j) % n;
            outvec[k] += xvec[i] * yvec[j];
        }
    }
    return outvec;
}

void testConvolution(int n) {
    const auto input0 = randomComplexes(n);
    const auto input1 = randomComplexes(n);
    const auto refout = naiveConvolve(input0, input1);
    ComplexVector actualout(n);
    fft::convolve(input0, input1, actualout);
    cout << "convsize=" << std::setw(4) << std::setfill(' ') << n << "  "
         << "logerr=" << std::setw(5) << std::setprecision(3) << std::setiosflags(std::ios::showpoint)
         << log10RmsErr(refout, actualout) << endl;
}
}

TEST(Fft, Simple) {
    for (int i = 0; i <= 12; i++) {
        testFft(1 << i);
    }

    for (int i = 0; i < 30; i++) {
        testFft(i);
    }

    for (int i = 0, prev = 0; i <= 100; i++) {
        int n = static_cast<int>(std::lround(std::pow(1500.0, i / 100.0)));
        if (n > prev) {
            testFft(n);
            prev = n;
        }
    }

    for (int i = 0; i <= 12; i++) {
        testConvolution(1 << i);
    }

    for (int i = 0, prev = 0; i <= 100; i++) {
        int n = static_cast<int>(std::lround(std::pow(1500.0, i / 100.0)));
        if (n > prev) {
            testConvolution(n);
            prev = n;
        }
    }

    cout << endl;
    cout << "Max log err = " << std::setprecision(3) << maxLogError << endl;
    cout << "Test " << (maxLogError < -10 ? "passed" : "failed") << endl;
    EXPECT_LT(maxLogError, -10);
}
