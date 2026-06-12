// Unit tests for bignum::BigInt and bignum::BigFloat.
#include <sstream>
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
BigFloat ComputePi(int digits) {
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

}  // namespace

// ===========================================================================
// BigInt
// ===========================================================================

TEST(BigInt, DefaultIsZero) {
  BigInt z;
  EXPECT_TRUE(z.is_zero());
  EXPECT_EQ(z.sign(), 0);
  EXPECT_EQ(z.to_string(), "0");
}

TEST(BigInt, ConstructFromIntegers) {
  EXPECT_EQ(BigInt(0).to_string(), "0");
  EXPECT_EQ(BigInt(123).to_string(), "123");
  EXPECT_EQ(BigInt(-123).to_string(), "-123");
  EXPECT_EQ(BigInt(9223372036854775807LL).to_string(), "9223372036854775807");
  // Most negative long long must round-trip without UB.
  EXPECT_EQ(BigInt(-9223372036854775807LL - 1).to_string(),
            "-9223372036854775808");
  EXPECT_EQ(BigInt(18446744073709551615ULL).to_string(),
            "18446744073709551615");
}

TEST(BigInt, ParseAndToString) {
  EXPECT_EQ(BigInt("0").to_string(), "0");
  EXPECT_EQ(BigInt("-0").to_string(), "0");
  EXPECT_EQ(BigInt("+42").to_string(), "42");
  EXPECT_EQ(BigInt("  -1000000000000000000000000  ").to_string(),
            "-1000000000000000000000000");
  std::string big = "123456789012345678901234567890123456789012345678901234567890";
  EXPECT_EQ(BigInt(big).to_string(), big);
}

TEST(BigInt, ParseHex) {
  EXPECT_EQ(BigInt("0xff").to_string(), "255");
  EXPECT_EQ(BigInt("-0x10").to_string(), "-16");
  EXPECT_EQ(BigInt::parse("0xDEADBEEF").to_string(), "3735928559");
}

TEST(BigInt, ParseRejectsGarbage) {
  EXPECT_THROW(BigInt::parse("abc"), std::invalid_argument);
  EXPECT_THROW(BigInt::parse("+"), std::invalid_argument);
  EXPECT_THROW(BigInt::parse(""), std::invalid_argument);
}

TEST(BigInt, Addition) {
  EXPECT_EQ((BigInt(2) + BigInt(3)).to_string(), "5");
  EXPECT_EQ((BigInt(-2) + BigInt(3)).to_string(), "1");
  EXPECT_EQ((BigInt(2) + BigInt(-3)).to_string(), "-1");
  EXPECT_EQ((BigInt(-2) + BigInt(-3)).to_string(), "-5");
  EXPECT_EQ((BigInt("999999999999999999999") + BigInt(1)).to_string(),
            "1000000000000000000000");
  EXPECT_EQ((BigInt(5) + BigInt(-5)).to_string(), "0");
}

TEST(BigInt, Subtraction) {
  EXPECT_EQ((BigInt(10) - BigInt(3)).to_string(), "7");
  EXPECT_EQ((BigInt(3) - BigInt(10)).to_string(), "-7");
  EXPECT_EQ((BigInt(0) - BigInt("12345678901234567890")).to_string(),
            "-12345678901234567890");
}

TEST(BigInt, Multiplication) {
  EXPECT_EQ((BigInt(12) * BigInt(12)).to_string(), "144");
  EXPECT_EQ((BigInt(-12) * BigInt(12)).to_string(), "-144");
  EXPECT_EQ((BigInt(-12) * BigInt(-12)).to_string(), "144");
  EXPECT_EQ((BigInt(0) * BigInt("99999999999999")).to_string(), "0");
  EXPECT_EQ((BigInt("123456789") * BigInt("987654321")).to_string(),
            "121932631112635269");
}

TEST(BigInt, Factorial50) {
  BigInt f(1);
  for (int i = 2; i <= 50; ++i) f *= BigInt(i);
  EXPECT_EQ(f.to_string(),
            "30414093201713378043612608166064768844377641568960512000000000000");
}

TEST(BigInt, DivisionAndModulo) {
  EXPECT_EQ((BigInt(17) / BigInt(5)).to_string(), "3");
  EXPECT_EQ((BigInt(17) % BigInt(5)).to_string(), "2");
  // Truncation toward zero; remainder takes sign of dividend.
  EXPECT_EQ((BigInt(-17) / BigInt(5)).to_string(), "-3");
  EXPECT_EQ((BigInt(-17) % BigInt(5)).to_string(), "-2");
  EXPECT_EQ((BigInt(17) / BigInt(-5)).to_string(), "-3");
  EXPECT_EQ((BigInt(17) % BigInt(-5)).to_string(), "2");

  BigInt big("123456789012345678901234567890");
  BigInt d("987654321");
  BigInt q = big / d;
  BigInt r = big % d;
  EXPECT_EQ((q * d + r).to_string(), big.to_string());
  EXPECT_TRUE(r.abs() < d.abs());
}

TEST(BigInt, DivisionByZeroThrows) {
  EXPECT_THROW(BigInt(1) / BigInt(0), std::domain_error);
  EXPECT_THROW(BigInt(1) % BigInt(0), std::domain_error);
}

TEST(BigInt, Power) {
  EXPECT_EQ(BigInt::pow(BigInt(2), 10).to_string(), "1024");
  EXPECT_EQ(BigInt::pow(BigInt(2), 100).to_string(),
            "1267650600228229401496703205376");
  EXPECT_EQ(BigInt::pow(BigInt(10), 0).to_string(), "1");
  EXPECT_EQ(BigInt::pow10(20).to_string(), "100000000000000000000");
}

TEST(BigInt, Comparisons) {
  EXPECT_TRUE(BigInt(-1) < BigInt(0));
  EXPECT_TRUE(BigInt(0) < BigInt(1));
  EXPECT_TRUE(BigInt("1000000000000000000000") > BigInt("999999999999999999999"));
  EXPECT_TRUE(BigInt(-5) < BigInt(-4));
  EXPECT_EQ(BigInt(42), BigInt("42"));
  EXPECT_NE(BigInt(42), BigInt(-42));
  EXPECT_LE(BigInt(5), BigInt(5));
  EXPECT_GE(BigInt(5), BigInt(5));
}

TEST(BigInt, IncrementDecrement) {
  BigInt x(0);
  EXPECT_EQ((++x).to_string(), "1");
  EXPECT_EQ((x++).to_string(), "1");
  EXPECT_EQ(x.to_string(), "2");
  EXPECT_EQ((--x).to_string(), "1");
  EXPECT_EQ((x--).to_string(), "1");
  EXPECT_EQ(x.to_string(), "0");
  EXPECT_EQ((--x).to_string(), "-1");
}

TEST(BigInt, UnaryAndAbs) {
  EXPECT_EQ((-BigInt(5)).to_string(), "-5");
  EXPECT_EQ((-BigInt(-5)).to_string(), "5");
  EXPECT_EQ((-BigInt(0)).to_string(), "0");
  EXPECT_EQ(BigInt(-7).abs().to_string(), "7");
}

TEST(BigInt, HexOutput) {
  EXPECT_EQ(BigInt(255).to_hex(), "ff");
  EXPECT_EQ(BigInt(0).to_hex(), "0");
  EXPECT_EQ(BigInt(-16).to_hex(), "-10");
  EXPECT_EQ(BigInt("3735928559").to_hex(), "deadbeef");
}

TEST(BigInt, ToLongLong) {
  EXPECT_EQ(BigInt(0).to_ll(), 0);
  EXPECT_EQ(BigInt(-12345).to_ll(), -12345);
  EXPECT_EQ(BigInt(9223372036854775807LL).to_ll(), 9223372036854775807LL);
  EXPECT_THROW(BigInt("99999999999999999999999999").to_ll(), std::overflow_error);
}

TEST(BigInt, StreamRoundTrip) {
  std::stringstream ss;
  ss << BigInt("-98765432109876543210");
  EXPECT_EQ(ss.str(), "-98765432109876543210");
  BigInt v;
  ss >> v;
  EXPECT_EQ(v.to_string(), "-98765432109876543210");
}

TEST(BigInt, StreamReadMultiple) {
  std::stringstream ss("100 -200 300");
  BigInt a, b, c;
  ss >> a >> b >> c;
  EXPECT_EQ(a.to_string(), "100");
  EXPECT_EQ(b.to_string(), "-200");
  EXPECT_EQ(c.to_string(), "300");
}

// ===========================================================================
// BigFloat
// ===========================================================================

TEST(BigFloat, DefaultIsZero) {
  EXPECT_TRUE(BigFloat().is_zero());
  EXPECT_EQ(BigFloat().to_string(), "0");
}

TEST(BigFloat, ParseAndToString) {
  EXPECT_EQ(BigFloat("0").to_string(), "0");
  EXPECT_EQ(BigFloat("3.14").to_string(), "3.14");
  EXPECT_EQ(BigFloat("-0.001").to_string(), "-0.001");
  EXPECT_EQ(BigFloat("100.00").to_string(), "100");
  EXPECT_EQ(BigFloat("0.10").to_string(), "0.1");
  EXPECT_EQ(BigFloat("12.3400").to_string(), "12.34");
  EXPECT_EQ(BigFloat(".5").to_string(), "0.5");
  EXPECT_EQ(BigFloat("-0").to_string(), "0");
}

TEST(BigFloat, ParseScientific) {
  EXPECT_EQ(BigFloat("1.5e3").to_string(), "1500");
  EXPECT_EQ(BigFloat("1.5e-3").to_string(), "0.0015");
  EXPECT_EQ(BigFloat("2E10").to_string(), "20000000000");
  EXPECT_EQ(BigFloat("-4.2e-2").to_string(), "-0.042");
}

TEST(BigFloat, ParseRejectsGarbage) {
  EXPECT_THROW(BigFloat::parse("1.2.3"), std::invalid_argument);
  EXPECT_THROW(BigFloat::parse("abc"), std::invalid_argument);
  EXPECT_THROW(BigFloat::parse("1e"), std::invalid_argument);
}

TEST(BigFloat, Addition) {
  EXPECT_EQ((BigFloat("0.1") + BigFloat("0.2")).to_string(), "0.3");
  EXPECT_EQ((BigFloat("1.5") + BigFloat("2.5")).to_string(), "4");
  EXPECT_EQ((BigFloat("100") + BigFloat("0.001")).to_string(), "100.001");
  EXPECT_EQ((BigFloat("1.25") + BigFloat("-1.25")).to_string(), "0");
}

TEST(BigFloat, Subtraction) {
  EXPECT_EQ((BigFloat("0.3") - BigFloat("0.1")).to_string(), "0.2");
  EXPECT_EQ((BigFloat("1") - BigFloat("0.0001")).to_string(), "0.9999");
  EXPECT_EQ((BigFloat("5") - BigFloat("10")).to_string(), "-5");
}

TEST(BigFloat, Multiplication) {
  EXPECT_EQ((BigFloat("0.1") * BigFloat("0.1")).to_string(), "0.01");
  EXPECT_EQ((BigFloat("1.5") * BigFloat("4")).to_string(), "6");
  EXPECT_EQ((BigFloat("-2.5") * BigFloat("2")).to_string(), "-5");
  EXPECT_EQ((BigFloat("123.456") * BigFloat("1000")).to_string(), "123456");
}

TEST(BigFloat, Division) {
  EXPECT_EQ((BigFloat("1") / BigFloat("4")).to_string(), "0.25");
  EXPECT_EQ((BigFloat("10") / BigFloat("2")).to_string(), "5");
  // 1/3 to default precision begins with many 3s.
  std::string third = (BigFloat("1") / BigFloat("3")).to_string();
  EXPECT_EQ(third.substr(0, 12), "0.3333333333");
  EXPECT_GT(third.size(), 40u);
}

TEST(BigFloat, DivisionWithPrecision) {
  BigFloat q = BigFloat::divide(BigFloat("2"), BigFloat("3"), 5);
  EXPECT_EQ(q.to_string(), "0.66667");
  BigFloat q2 = BigFloat::divide(BigFloat("1"), BigFloat("7"), 10);
  EXPECT_EQ(q2.to_string().substr(0, 12), "0.1428571429");
}

TEST(BigFloat, DivisionByZeroThrows) {
  EXPECT_THROW(BigFloat("1") / BigFloat("0"), std::domain_error);
}

TEST(BigFloat, Comparisons) {
  EXPECT_TRUE(BigFloat("0.1") < BigFloat("0.2"));
  EXPECT_TRUE(BigFloat("-0.5") < BigFloat("0"));
  EXPECT_TRUE(BigFloat("1.50") == BigFloat("1.5"));
  EXPECT_TRUE(BigFloat("100") > BigFloat("99.999"));
  EXPECT_NE(BigFloat("0.1"), BigFloat("0.10001"));
  EXPECT_LE(BigFloat("2"), BigFloat("2.0"));
}

TEST(BigFloat, Negation) {
  EXPECT_EQ((-BigFloat("3.14")).to_string(), "-3.14");
  EXPECT_EQ((-BigFloat("-3.14")).to_string(), "3.14");
  EXPECT_EQ(BigFloat("-2.5").abs().to_string(), "2.5");
}

TEST(BigFloat, Scientific) {
  EXPECT_EQ(BigFloat("12345").to_scientific(3), "1.23e4");
  EXPECT_EQ(BigFloat("0.00042").to_scientific(2), "4.2e-4");
  EXPECT_EQ(BigFloat("0").to_scientific(), "0e0");
}

TEST(BigFloat, FromBigInt) {
  BigInt big("123456789012345678901234567890");
  BigFloat f(big);
  EXPECT_EQ(f.to_string(), "123456789012345678901234567890");
  EXPECT_EQ((f * BigFloat("0.5")).to_string(), "61728394506172839450617283945");
}

TEST(BigFloat, StreamRoundTrip) {
  std::stringstream ss;
  ss << BigFloat("-273.15");
  EXPECT_EQ(ss.str(), "-273.15");
  BigFloat v;
  ss >> v;
  EXPECT_EQ(v.to_string(), "-273.15");
}

// ===========================================================================
// Fun: compute a long value of Pi with a fast algorithm (Machin's formula)
// ===========================================================================

TEST(Pi, Machin100Digits) {
  // Reference: pi to 100 decimal places.
  const std::string kPi100 =
      "3."
      "1415926535897932384626433832795028841971"
      "6939937510582097494459230781640628620899"
      "86280348253421170679";

  BigFloat pi = ComputePi(100);
  std::string s = pi.to_string();
  // Compare "3." + 100 fractional digits (102 characters).
  EXPECT_EQ(s.substr(0, 102), kPi100);
}

TEST(Pi, Machin1000DigitsSpotCheck) {
  BigFloat pi = ComputePi(1000);
  std::string s = pi.to_string();
  EXPECT_EQ(s.substr(0, 12), "3.1415926535");
  // Pi to 1000 decimal places famously ends with "...2164201989".
  // Digits 991..1000 (after the decimal point) are "2164201989".
  ASSERT_GE(s.size(), 1002u);  // "3." + at least 1000 digits
  EXPECT_EQ(s.substr(2 + 990, 10), "2164201989");
}

TEST(BigInt, IntegerSqrt) {
  EXPECT_EQ(BigInt(0).isqrt().to_string(), "0");
  EXPECT_EQ(BigInt(1).isqrt().to_string(), "1");
  EXPECT_EQ(BigInt(15).isqrt().to_string(), "3");
  EXPECT_EQ(BigInt(16).isqrt().to_string(), "4");
  EXPECT_EQ(BigInt("1000000000000000000000000").isqrt().to_string(),
            "1000000000000");
  BigInt n = BigInt::pow10(100) * BigInt(2);  // 2e100
  BigInt r = n.isqrt();
  EXPECT_TRUE(r * r <= n);
  EXPECT_TRUE((r + BigInt(1)) * (r + BigInt(1)) > n);
  EXPECT_THROW(BigInt(-4).isqrt(), std::domain_error);
}

TEST(BigFloat, Sqrt) {
  EXPECT_EQ(BigFloat::sqrt(BigFloat("4"), 20).to_string().substr(0, 1), "2");
  EXPECT_EQ(BigFloat::sqrt(BigFloat("2"), 30).to_string().substr(0, 17),
            "1.414213562373095");
  EXPECT_EQ(BigFloat::sqrt(BigFloat("0"), 10).to_string(), "0");
  EXPECT_EQ(BigFloat::sqrt(BigFloat("0.25"), 10).to_string(), "0.5");
  EXPECT_THROW(BigFloat::sqrt(BigFloat("-1"), 10), std::domain_error);
}

// Gauss-Legendre (AGM) is quadratically convergent: it roughly doubles the
// number of correct digits each iteration, far fewer iterations than Machin.
TEST(Pi, GaussLegendre100Digits) {
  const std::string kPi100 =
      "3."
      "1415926535897932384626433832795028841971"
      "6939937510582097494459230781640628620899"
      "86280348253421170679";
  BigFloat pi = ComputePiGaussLegendre(100);
  EXPECT_EQ(pi.to_string().substr(0, 102), kPi100);
}
