#pragma once

#include <complex>

#include "lib/header.h"

namespace fft {
using ComplexVector = vector<complex<double>>;

void transform(ComplexVector& vec);

void inverseTransform(ComplexVector& vec);

void transformRadix2(ComplexVector& vec);

void transformBluestein(ComplexVector& vec);

void convolve(const ComplexVector& vecx, const ComplexVector& vecy, ComplexVector& vecout);
}
