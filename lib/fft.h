#pragma once

#include <complex>

#include "lib/header.h"

namespace fft {
using ComplexVector = vector<complex<double>>;

void transformNotNorm(ComplexVector& vec);
void inverseTransformNotNorm(ComplexVector& vec);
void transform(ComplexVector& vec);
void inverseTransform(ComplexVector& vec);
void convolve(const ComplexVector& vecx, const ComplexVector& vecy, ComplexVector& vecout);
}
