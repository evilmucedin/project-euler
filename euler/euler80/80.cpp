// Project Euler Problem 80 -- Square root digital expansion
// https://projecteuler.net/problem=80
//
// "It is well known that if the square root of a natural number is not an
//  integer, then it is irrational. For the first one hundred natural numbers,
//  find the total of the digital sums of the first one hundred decimal digits
//  for all the irrational square roots."
//
// Solved with the in-repo arbitrary-precision library (//bignum:bignum). For a
// non-square n, the first 100 significant decimal digits of sqrt(n) are exactly
// the digits of floor(sqrt(n * 10^198)): since 1 <= n <= 99 gives
// 1 <= sqrt(n) < 10, the value sqrt(n) * 10^99 lands in [10^99, 10^100), a
// 100-digit integer. We obtain that integer with BigInt::isqrt and sum its
// digits.
#include <iostream>

#include "bignum/bigint.h"

using bignum::BigInt;

int main() {
  const int kDigits = 100;
  const BigInt scale = BigInt::pow10(2 * (kDigits - 1));  // 10^198

  long long total = 0;
  for (int n = 1; n <= 100; ++n) {
    BigInt root = BigInt(n).isqrt();
    if (root * root == BigInt(n)) continue;  // perfect square -> rational

    std::string digits = (BigInt(n) * scale).isqrt().to_string();
    // digits has exactly `kDigits` characters for n in [2, 99].
    for (char c : digits) total += c - '0';
  }

  std::cout << total << std::endl;
  return 0;
}
