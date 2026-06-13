// Randomized cross-check tests: validate bignum results against values
// computed directly by the CPU using native integer / floating-point types.
//
//   BigInt   vs  int64_t and __int128
//   BigFloat vs  double
//
// A fixed RNG seed keeps the runs reproducible.
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <random>
#include <string>

#include "bignum/bigint.h"
#include "bignum/bigfloat.h"

#include "gtest/gtest.h"

using bignum::BigInt;
using bignum::BigFloat;

namespace {

constexpr int kIterations = 5000;

std::mt19937_64& rng() {
  static std::mt19937_64 gen(0xC0FFEEULL);  // fixed seed -> reproducible
  return gen;
}

// Decimal string for a 128-bit integer (used to check products beyond 64 bits).
std::string i128_to_string(__int128 v) {
  if (v == 0) return "0";
  bool neg = v < 0;
  unsigned __int128 u =
      neg ? (~static_cast<unsigned __int128>(v) + 1) : static_cast<unsigned __int128>(v);
  std::string s;
  while (u) {
    s.push_back(static_cast<char>('0' + static_cast<int>(u % 10)));
    u /= 10;
  }
  if (neg) s.push_back('-');
  std::reverse(s.begin(), s.end());
  return s;
}

double to_double(const BigFloat& f) { return std::stod(f.to_string()); }

}  // namespace

// ===========================================================================
// BigInt vs native integer types
// ===========================================================================

TEST(CrossCheckBigInt, AddSubAgainstInt64) {
  // Keep operands within +/-2e18 so sum/difference stay within int64 range.
  std::uniform_int_distribution<int64_t> dist(-2'000'000'000'000'000'000LL,
                                              2'000'000'000'000'000'000LL);
  for (int i = 0; i < kIterations; ++i) {
    int64_t a = dist(rng()) / 2;  // /2 guarantees a+b cannot overflow int64
    int64_t b = dist(rng()) / 2;
    EXPECT_EQ((BigInt(a) + BigInt(b)).to_string(), std::to_string(a + b));
    EXPECT_EQ((BigInt(a) - BigInt(b)).to_string(), std::to_string(a - b));
  }
}

TEST(CrossCheckBigInt, MultiplyAgainstInt128) {
  std::uniform_int_distribution<int64_t> dist(INT64_MIN / 2, INT64_MAX / 2);
  for (int i = 0; i < kIterations; ++i) {
    int64_t a = dist(rng());
    int64_t b = dist(rng());
    __int128 prod = static_cast<__int128>(a) * static_cast<__int128>(b);
    EXPECT_EQ((BigInt(a) * BigInt(b)).to_string(), i128_to_string(prod))
        << "a=" << a << " b=" << b;
  }
}

TEST(CrossCheckBigInt, DivModAgainstInt64) {
  std::uniform_int_distribution<int64_t> dist(-1'000'000'000'000LL, 1'000'000'000'000LL);
  for (int i = 0; i < kIterations; ++i) {
    int64_t a = dist(rng());
    int64_t b = dist(rng());
    if (b == 0) continue;
    // C++ integer division truncates toward zero; remainder takes the
    // dividend's sign -- the same contract BigInt::divmod implements.
    EXPECT_EQ((BigInt(a) / BigInt(b)).to_string(), std::to_string(a / b))
        << "a=" << a << " b=" << b;
    EXPECT_EQ((BigInt(a) % BigInt(b)).to_string(), std::to_string(a % b))
        << "a=" << a << " b=" << b;
    // Fundamental identity: (a/b)*b + a%b == a.
    BigInt q = BigInt(a) / BigInt(b);
    BigInt r = BigInt(a) % BigInt(b);
    EXPECT_EQ((q * BigInt(b) + r).to_string(), std::to_string(a));
  }
}

TEST(CrossCheckBigInt, ComparisonsAgainstInt64) {
  std::uniform_int_distribution<int64_t> dist(INT64_MIN, INT64_MAX);
  for (int i = 0; i < kIterations; ++i) {
    int64_t a = dist(rng());
    int64_t b = dist(rng());
    BigInt ba(a), bb(b);
    EXPECT_EQ(ba < bb, a < b);
    EXPECT_EQ(ba <= bb, a <= b);
    EXPECT_EQ(ba > bb, a > b);
    EXPECT_EQ(ba >= bb, a >= b);
    EXPECT_EQ(ba == bb, a == b);
    EXPECT_EQ(ba != bb, a != b);
  }
}

TEST(CrossCheckBigInt, RoundTripThroughLongLong) {
  std::uniform_int_distribution<int64_t> dist(INT64_MIN, INT64_MAX);
  for (int i = 0; i < kIterations; ++i) {
    int64_t a = dist(rng());
    EXPECT_EQ(BigInt(a).to_ll(), static_cast<long long>(a));
    EXPECT_EQ(BigInt(std::to_string(a)).to_string(), std::to_string(a));
  }
}

// ===========================================================================
// BigFloat vs native double
// ===========================================================================

namespace {

// Build a decimal string with `frac` fractional digits from a double so that
// both the CPU (via stod) and BigFloat parse the exact same value.
std::string decimal_string(double sign_mag) {
  char buf[64];
  std::snprintf(buf, sizeof(buf), "%.6f", sign_mag);
  return buf;
}

}  // namespace

TEST(CrossCheckBigFloat, AddSubMulAgainstDouble) {
  std::uniform_real_distribution<double> dist(-100000.0, 100000.0);
  for (int i = 0; i < kIterations; ++i) {
    std::string sa = decimal_string(dist(rng()));
    std::string sb = decimal_string(dist(rng()));
    double da = std::stod(sa);
    double db = std::stod(sb);

    EXPECT_NEAR(to_double(BigFloat(sa) + BigFloat(sb)), da + db, 1e-3);
    EXPECT_NEAR(to_double(BigFloat(sa) - BigFloat(sb)), da - db, 1e-3);
    // Products can be ~1e10; scale tolerance with magnitude.
    double prod = da * db;
    EXPECT_NEAR(to_double(BigFloat(sa) * BigFloat(sb)), prod,
                std::max(1e-3, std::fabs(prod) * 1e-9));
  }
}

TEST(CrossCheckBigFloat, DivideAgainstDouble) {
  std::uniform_real_distribution<double> dist(-100000.0, 100000.0);
  for (int i = 0; i < kIterations; ++i) {
    std::string sa = decimal_string(dist(rng()));
    std::string sb = decimal_string(dist(rng()));
    double da = std::stod(sa);
    double db = std::stod(sb);
    if (db == 0.0) continue;
    double quot = da / db;
    BigFloat bq = BigFloat::divide(BigFloat(sa), BigFloat(sb), 40);
    EXPECT_NEAR(to_double(bq), quot, std::max(1e-9, std::fabs(quot) * 1e-12));
  }
}

TEST(CrossCheckBigFloat, SqrtAgainstStdSqrt) {
  std::uniform_real_distribution<double> dist(0.0, 1'000'000.0);
  for (int i = 0; i < kIterations; ++i) {
    std::string sx = decimal_string(dist(rng()));
    double dx = std::stod(sx);
    double expected = std::sqrt(dx);
    BigFloat bs = BigFloat::sqrt(BigFloat(sx), 40);
    EXPECT_NEAR(to_double(bs), expected, std::max(1e-9, expected * 1e-12));
  }
}

TEST(CrossCheckBigFloat, ComparisonsAgainstDouble) {
  std::uniform_real_distribution<double> dist(-1000.0, 1000.0);
  for (int i = 0; i < kIterations; ++i) {
    std::string sa = decimal_string(dist(rng()));
    std::string sb = decimal_string(dist(rng()));
    double da = std::stod(sa);
    double db = std::stod(sb);
    BigFloat ba(sa), bb(sb);
    EXPECT_EQ(ba < bb, da < db) << sa << " vs " << sb;
    EXPECT_EQ(ba > bb, da > db) << sa << " vs " << sb;
    EXPECT_EQ(ba == bb, da == db) << sa << " vs " << sb;
  }
}
