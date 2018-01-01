#include "fft.h"

namespace {
using namespace fft;

size_t reverseBits(size_t x, int n) {
    size_t result = 0;
    for (int i = 0; i < n; i++, x >>= 1) {
        result = (result << 1) | (x & 1U);
    }
    return result;
}

void transformRadix2(ComplexVector& vec) {
    size_t n = vec.size();
    int levels = 0;
    for (size_t temp = n; temp > 1U; temp >>= 1) {
        ++levels;
    }
    if ((static_cast<size_t>(1U) << levels) != n) {
        throw Exception("Length is not a power of 2");
    }

    ComplexVector expTable(n / 2);
    for (size_t i = 0; i + i < n; ++i) {
        expTable[i] = std::exp(complex<double>(0, (-2.0 * M_PI * i) / n));
    }

    for (size_t i = 0; i < n; ++i) {
        size_t j = reverseBits(i, levels);
        if (j > i) {
            std::swap(vec[i], vec[j]);
        }
    }

    for (size_t size = 2; size <= n; size *= 2) {
        size_t halfsize = size / 2;
        size_t tablestep = n / size;
        for (size_t i = 0; i < n; i += size) {
            for (size_t j = i, k = 0; j < i + halfsize; j++, k += tablestep) {
                complex<double> temp = vec[j + halfsize] * expTable[k];
                vec[j + halfsize] = vec[j] - temp;
                vec[j] += temp;
            }
        }
        if (size == n) {
            break;
        }
    }
}

void transformBluestein(ComplexVector& vec) {
    size_t n = vec.size();
    size_t m = 1;
    while (m / 2 <= n) {
        if (m > SIZE_MAX / 2) {
            throw Exception("Vector too large");
        }
        m *= 2;
    }

    ComplexVector expTable(n);
    for (size_t i = 0; i < n; i++) {
        unsigned long long temp = static_cast<unsigned long long>(i) * i;
        temp %= static_cast<unsigned long long>(n) * 2;
        double angle = M_PI * temp / n;
        expTable[i] = std::exp(complex<double>(0, -angle));
    }

    ComplexVector av(m);
    for (size_t i = 0; i < n; ++i) {
        av[i] = vec[i] * expTable[i];
    }
    ComplexVector bv(m);
    bv[0] = expTable[0];
    for (size_t i = 1; i < n; ++i) {
        bv[i] = bv[m - i] = std::conj(expTable[i]);
    }

    ComplexVector cv(m);
    convolve(av, bv, cv);

    for (size_t i = 0; i < n; ++i) {
        vec[i] = cv[i] * expTable[i];
    }
}

}  // namespace

namespace fft {

void normalize(ComplexVector& vct) {
    for (auto& x : vct) {
        x /= vct.size();
    }
}

void transformNotNorm(ComplexVector& vec) {
    size_t n = vec.size();
    if (n == 0) {
        return;
    } else if ((n & (n - 1)) == 0) {
        transformRadix2(vec);
    } else {
        transformBluestein(vec);
    }
}

void inverseTransformNotNorm(ComplexVector& vec) {
    for (auto& x : vec) {
        x = std::conj(x);
    }
    transformNotNorm(vec);
    for (auto& x : vec) {
        x = std::conj(x);
    }
}
void transform(ComplexVector& vec) {
    transformNotNorm(vec);
    normalize(vec);
}

void inverseTransform(ComplexVector& vec) {
    inverseTransformNotNorm(vec);
    normalize(vec);
}

void convolve(const ComplexVector& xvec, const ComplexVector& yvec, ComplexVector& outvec) {
    size_t n = xvec.size();
    if ((n != yvec.size()) || (n != outvec.size())) {
        throw Exception("Mismatched lengths");
    }
    ComplexVector xv = xvec;
    ComplexVector yv = yvec;
    transformNotNorm(xv);
    transformNotNorm(yv);
    for (size_t i = 0; i < n; ++i) {
        xv[i] *= yv[i];
    }
    inverseTransformNotNorm(xv);
    for (size_t i = 0; i < n; ++i) {
        outvec[i] = xv[i] / static_cast<double>(n);
    }
}

}  // namespace
