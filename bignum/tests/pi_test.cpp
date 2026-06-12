// Tests that generate a long value of Pi using fast mathematical algorithms
// and verify the digits against the known constant.
//
//   * Machin's formula        pi = 16*arctan(1/5) - 4*arctan(1/239)
//   * Gauss-Legendre (AGM)    quadratically convergent
#include <string>

#include "bignum/bigint.h"
#include "bignum/bigfloat.h"

#include "gtest/gtest.h"

using bignum::BigInt;
using bignum::BigFloat;

namespace {

// Fixed-point arctan(1/x) scaled by `scale`, i.e. returns round(scale*atan(1/x)).
// Uses the Taylor series atan(1/x) = sum_{k>=0} (-1)^k / ((2k+1) * x^(2k+1)),
// which converges quickly for large x (e.g. x = 5 and x = 239 in Machin's
// formula). All arithmetic is integer-only via BigInt.
BigInt ScaledArctanInv(long long x, const BigInt& scale) {
  BigInt x2(x * x);
  BigInt power = scale / BigInt(x);  // scale / x  (k = 0 term)
  BigInt sum = power;
  for (long long k = 1;; ++k) {
    power /= x2;  // scale / x^(2k+1)
    BigInt term = power / BigInt(2 * k + 1);
    if (term.is_zero()) break;
    if (k & 1) sum -= term; else sum += term;
  }
  return sum;
}

// Compute pi to `digits` decimal places using Machin's formula:
//   pi = 16*arctan(1/5) - 4*arctan(1/239)
// A handful of guard digits are used internally and then trimmed.
BigFloat ComputePiMachin(int digits) {
  const int guard = 12;
  BigInt scale = BigInt::pow10(static_cast<unsigned>(digits + guard));
  BigInt pi_scaled =
      BigInt(16) * ScaledArctanInv(5, scale) - BigInt(4) * ScaledArctanInv(239, scale);
  return BigFloat(pi_scaled, -(static_cast<long long>(digits) + guard));
}

// Compute pi to `digits` decimal places using the Gauss-Legendre (AGM)
// algorithm. Each iteration roughly doubles the number of correct digits
// (quadratic convergence), so ~log2(digits) iterations suffice.
BigFloat ComputePiGaussLegendre(int digits) {
  const int work = digits + 10;  // guard digits
  BigFloat a(1);
  BigFloat b = BigFloat::sqrt(BigFloat::divide(BigFloat(1), BigFloat(2), work), work);
  BigFloat t = BigFloat::divide(BigFloat(1), BigFloat(4), work);
  BigFloat p(1);

  // log2(digits) + a couple of safety iterations.
  int iters = 2;
  for (int v = digits; v > 0; v >>= 1) ++iters;
  for (int i = 0; i < iters; ++i) {
    BigFloat a_next = BigFloat::divide(a + b, BigFloat(2), work);
    BigFloat b_next = BigFloat::sqrt(a * b, work);
    BigFloat diff = a - a_next;
    t = t - p * (diff * diff);
    a = a_next;
    b = b_next;
    p = p * BigFloat(2);
  }
  BigFloat num = (a + b) * (a + b);
  return BigFloat::divide(num, BigFloat(4) * t, digits + 2);
}

// Pi to 100 decimal places (reference constant), as "3." + 100 fractional
// digits (102 characters total).
const char* const kPi100 =
    "3."
    "1415926535897932384626433832795028841971"
    "6939937510582097494459230781640628620899"
    "86280348253421170679";

}  // namespace

TEST(Pi, Machin100Digits) {
  BigFloat pi = ComputePiMachin(100);
  EXPECT_EQ(pi.to_string().substr(0, 102), kPi100);
}

TEST(Pi, Machin1000DigitsSpotCheck) {
  BigFloat pi = ComputePiMachin(1000);
  std::string s = pi.to_string();
  EXPECT_EQ(s.substr(0, 12), "3.1415926535");
  // Pi to 1000 decimal places famously ends with "...2164201989".
  // Digits 991..1000 (after the decimal point) are "2164201989".
  ASSERT_GE(s.size(), 1002u);  // "3." + at least 1000 digits
  EXPECT_EQ(s.substr(2 + 990, 10), "2164201989");
}

// Gauss-Legendre (AGM) is quadratically convergent: it roughly doubles the
// number of correct digits each iteration, far fewer iterations than Machin.
TEST(Pi, GaussLegendre100Digits) {
  BigFloat pi = ComputePiGaussLegendre(100);
  EXPECT_EQ(pi.to_string().substr(0, 102), kPi100);
}

// Both algorithms must agree with each other to 100 digits.
TEST(Pi, MachinAndGaussLegendreAgree) {
  EXPECT_EQ(ComputePiMachin(100).to_string().substr(0, 102),
            ComputePiGaussLegendre(100).to_string().substr(0, 102));
}
