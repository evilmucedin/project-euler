// Project Euler Problem 57 -- Square root convergents
// https://projecteuler.net/problem=57
//
// The infinite continued fraction for sqrt(2) yields the successive
// approximations 3/2, 7/5, 17/12, 41/29, ... where each fraction n/d advances
// to (n + 2d) / (n + d). In the first one-thousand expansions, how many
// fractions contain a numerator with more digits than the denominator?
//
// Solved with the in-repo arbitrary-precision library (//bignum:bignum): the
// numerators and denominators quickly exceed 64-bit range, so we keep them as
// BigInt and compare decimal lengths via to_string().
#include <iostream>

#include "bignum/bigint.h"

using bignum::BigInt;

int main() {
  BigInt n(3), d(2);  // first expansion: 3/2
  int count = 0;
  for (int i = 1; i <= 1000; ++i) {
    if (n.to_string().size() > d.to_string().size()) ++count;
    // Advance: n/d -> (n + 2d) / (n + d).
    BigInt next_n = n + d + d;
    BigInt next_d = n + d;
    n = next_n;
    d = next_d;
  }
  std::cout << count << std::endl;
  return 0;
}
