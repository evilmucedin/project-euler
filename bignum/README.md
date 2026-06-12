# bignum

A small, dependency-free (standard library only) C++17 arbitrary-precision
numeric library.

## Components

### `BigInt` (`bigint.h`) — first-class implementation

Signed arbitrary-precision integer using a sign-magnitude representation with
base-2^32 limbs stored little-endian.

- Construction from `int` / `long long` / `unsigned long long` and from strings
  (decimal or `0x` hex).
- Arithmetic: `+ - * / %`, unary `-`, `++`/`--`, `BigInt::pow`, `BigInt::pow10`,
  `BigInt::divmod` (truncating toward zero, remainder takes the dividend's sign).
- Comparison: full set of relational operators.
- Conversion: `to_string`, `to_hex`, `parse`, `to_ll` (range-checked).
- I/O: `operator<<` / `operator>>`.

### `BigFloat` (`bigfloat.h`) — second-class implementation

Arbitrary-precision **decimal** floating point, represented as
`significand * 10^exponent` where the significand is a `BigInt`. Because it is
base-10, values such as `0.1` are exact. Built entirely on top of `BigInt`.

- Construction from integers, `BigInt`, and strings (decimal and scientific
  notation, e.g. `1.5e-3`).
- Arithmetic: `+ - *`, and `/` to a configurable number of significant digits
  (`BigFloat::divide`, default `kDefaultDivPrecision`).
- Comparison, negation, `abs`.
- Conversion: `to_string` (plain decimal), `to_scientific`, `parse`.
- I/O: `operator<<` / `operator>>`.

## Build & test

```sh
# Ninja smoke build + run
python3 scripts/generate_ninja.py
ninja bignum/tests/bignumTest
./build-ninja/bin/bignum/tests/bignumTest

# Buck2
buck2 build //bignum/tests:bignumTest
```
