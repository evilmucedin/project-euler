// bigint.h - Arbitrary precision signed integer.
//
// First-class implementation: sign-magnitude representation with base 2^32
// limbs stored little-endian in a std::vector<uint32_t>. Provides the full set
// of arithmetic, comparison, stream, and string-conversion operations.
//
// Header-only; C++17.
#ifndef BIGNUM_BIGINT_H
#define BIGNUM_BIGINT_H

#include <cstdint>
#include <cstddef>
#include <cctype>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <istream>
#include <ostream>
#include <utility>

namespace bignum {

class BigInt {
 public:
  using limb = uint32_t;
  using wide = uint64_t;
  static constexpr wide kBase = wide(1) << 32;

  // --- Construction ---------------------------------------------------------
  BigInt() : neg_(false) {}

  BigInt(long long value) : neg_(false) { assign_signed(value); }
  BigInt(int value) : neg_(false) { assign_signed(value); }
  BigInt(unsigned long long value) : neg_(false) { assign_unsigned(value); }

  // Parse from a decimal (optionally signed, optional "0x" hex) string.
  explicit BigInt(const std::string& text) { *this = parse(text); }

  // --- Factory helpers ------------------------------------------------------
  static BigInt zero() { return BigInt(); }
  static BigInt one() { return BigInt(1); }

  // 10^exp as a BigInt.
  static BigInt pow10(unsigned exp) {
    BigInt r(1);
    for (unsigned i = 0; i < exp; ++i) r.mul_small(10);
    return r;
  }

  // base^exp (exponentiation by squaring).
  static BigInt pow(BigInt base, unsigned long long exp) {
    BigInt result(1);
    while (exp) {
      if (exp & 1ull) result *= base;
      exp >>= 1;
      if (exp) base *= base;
    }
    return result;
  }

  // --- State queries --------------------------------------------------------
  bool is_zero() const { return mag_.empty(); }
  bool is_negative() const { return neg_; }
  int sign() const { return is_zero() ? 0 : (neg_ ? -1 : 1); }
  size_t limb_count() const { return mag_.size(); }

  BigInt abs() const {
    BigInt r(*this);
    r.neg_ = false;
    return r;
  }

  // --- Unary ----------------------------------------------------------------
  BigInt operator-() const {
    BigInt r(*this);
    if (!r.is_zero()) r.neg_ = !r.neg_;
    return r;
  }
  BigInt operator+() const { return *this; }

  // --- Compound arithmetic --------------------------------------------------
  BigInt& operator+=(const BigInt& o) {
    if (neg_ == o.neg_) {
      mag_ = add_mag(mag_, o.mag_);
    } else {
      int c = cmp_mag(mag_, o.mag_);
      if (c == 0) {
        mag_.clear();
        neg_ = false;
      } else if (c > 0) {
        mag_ = sub_mag(mag_, o.mag_);
      } else {
        mag_ = sub_mag(o.mag_, mag_);
        neg_ = o.neg_;
      }
    }
    normalize();
    return *this;
  }

  BigInt& operator-=(const BigInt& o) { return *this += (-o); }

  BigInt& operator*=(const BigInt& o) {
    if (is_zero() || o.is_zero()) {
      mag_.clear();
      neg_ = false;
      return *this;
    }
    mag_ = mul_mag(mag_, o.mag_);
    neg_ = (neg_ != o.neg_);
    normalize();
    return *this;
  }

  BigInt& operator/=(const BigInt& o) {
    *this = divmod(*this, o).first;
    return *this;
  }
  BigInt& operator%=(const BigInt& o) {
    *this = divmod(*this, o).second;
    return *this;
  }

  // --- Binary arithmetic ----------------------------------------------------
  friend BigInt operator+(BigInt a, const BigInt& b) { a += b; return a; }
  friend BigInt operator-(BigInt a, const BigInt& b) { a -= b; return a; }
  friend BigInt operator*(BigInt a, const BigInt& b) { a *= b; return a; }
  friend BigInt operator/(BigInt a, const BigInt& b) { a /= b; return a; }
  friend BigInt operator%(BigInt a, const BigInt& b) { a %= b; return a; }

  // Truncated division toward zero: quotient and remainder. The remainder has
  // the sign of the dividend (C++ semantics).
  static std::pair<BigInt, BigInt> divmod(const BigInt& a, const BigInt& b) {
    if (b.is_zero()) throw std::domain_error("BigInt division by zero");
    std::vector<limb> q, r;
    divmod_mag(a.mag_, b.mag_, q, r);
    BigInt quotient, remainder;
    quotient.mag_ = std::move(q);
    remainder.mag_ = std::move(r);
    quotient.neg_ = (a.neg_ != b.neg_);
    remainder.neg_ = a.neg_;
    quotient.normalize();
    remainder.normalize();
    return {std::move(quotient), std::move(remainder)};
  }

  // --- Increment / decrement ------------------------------------------------
  BigInt& operator++() { return *this += one(); }
  BigInt& operator--() { return *this -= one(); }
  BigInt operator++(int) { BigInt t(*this); ++*this; return t; }
  BigInt operator--(int) { BigInt t(*this); --*this; return t; }

  // --- Comparison -----------------------------------------------------------
  int compare(const BigInt& o) const {
    if (neg_ != o.neg_) return neg_ ? -1 : 1;
    int c = cmp_mag(mag_, o.mag_);
    return neg_ ? -c : c;
  }
  friend bool operator==(const BigInt& a, const BigInt& b) { return a.compare(b) == 0; }
  friend bool operator!=(const BigInt& a, const BigInt& b) { return a.compare(b) != 0; }
  friend bool operator<(const BigInt& a, const BigInt& b) { return a.compare(b) < 0; }
  friend bool operator<=(const BigInt& a, const BigInt& b) { return a.compare(b) <= 0; }
  friend bool operator>(const BigInt& a, const BigInt& b) { return a.compare(b) > 0; }
  friend bool operator>=(const BigInt& a, const BigInt& b) { return a.compare(b) >= 0; }

  // --- String conversion ----------------------------------------------------
  std::string to_string() const {
    if (is_zero()) return "0";
    std::vector<limb> tmp = mag_;
    std::string digits;
    // Extract base-1e9 chunks for efficiency.
    while (!tmp.empty()) {
      limb rem = div_small_inplace(tmp, 1000000000u);
      for (int i = 0; i < 9; ++i) {
        digits.push_back(char('0' + rem % 10));
        rem /= 10;
      }
    }
    // Strip leading zeros introduced by fixed 9-digit chunking.
    while (digits.size() > 1 && digits.back() == '0') digits.pop_back();
    if (neg_) digits.push_back('-');
    std::reverse(digits.begin(), digits.end());
    return digits;
  }

  // Hexadecimal representation (without sign for zero), e.g. "-1a".
  std::string to_hex() const {
    if (is_zero()) return "0";
    std::string s;
    for (size_t i = 0; i < mag_.size(); ++i) {
      limb v = mag_[i];
      for (int nib = 0; nib < 8; ++nib) {
        int d = v & 0xF;
        v >>= 4;
        s.push_back(d < 10 ? char('0' + d) : char('a' + d - 10));
      }
    }
    while (s.size() > 1 && s.back() == '0') s.pop_back();
    if (neg_) s.push_back('-');
    std::reverse(s.begin(), s.end());
    return s;
  }

  static BigInt parse(const std::string& text) {
    size_t i = 0, n = text.size();
    while (i < n && std::isspace(static_cast<unsigned char>(text[i]))) ++i;
    bool neg = false;
    if (i < n && (text[i] == '+' || text[i] == '-')) {
      neg = (text[i] == '-');
      ++i;
    }
    BigInt result;
    bool hex = false;
    if (i + 1 < n && text[i] == '0' && (text[i + 1] == 'x' || text[i + 1] == 'X')) {
      hex = true;
      i += 2;
    }
    bool any = false;
    if (hex) {
      for (; i < n; ++i) {
        char c = text[i];
        int d;
        if (c >= '0' && c <= '9') d = c - '0';
        else if (c >= 'a' && c <= 'f') d = c - 'a' + 10;
        else if (c >= 'A' && c <= 'F') d = c - 'A' + 10;
        else break;
        result.mul_small(16);
        result.add_small(static_cast<limb>(d));
        any = true;
      }
    } else {
      for (; i < n; ++i) {
        char c = text[i];
        if (c < '0' || c > '9') break;
        result.mul_small(10);
        result.add_small(static_cast<limb>(c - '0'));
        any = true;
      }
    }
    if (!any) throw std::invalid_argument("BigInt::parse: no digits in '" + text + "'");
    if (neg && !result.is_zero()) result.neg_ = true;
    return result;
  }

  // --- Stream operators -----------------------------------------------------
  friend std::ostream& operator<<(std::ostream& os, const BigInt& v) {
    return os << v.to_string();
  }
  friend std::istream& operator>>(std::istream& is, BigInt& v) {
    std::string token;
    if (is >> token) {
      try {
        v = parse(token);
      } catch (const std::exception&) {
        is.setstate(std::ios::failbit);
      }
    }
    return is;
  }

  // Best-effort conversion to long long (throws if out of range).
  long long to_ll() const {
    const wide limit = is_negative() ? (wide(1) << 63)
                                     : ((wide(1) << 63) - 1);
    wide acc = 0;
    for (size_t i = mag_.size(); i-- > 0;) {
      if (acc > (limit - mag_[i]) / kBase)
        throw std::overflow_error("BigInt::to_ll overflow");
      acc = acc * kBase + mag_[i];
    }
    if (neg_) return -static_cast<long long>(acc);
    return static_cast<long long>(acc);
  }

 private:
  std::vector<limb> mag_;  // little-endian magnitude, no trailing zero limbs
  bool neg_;               // sign; never true when mag_ is empty (zero)

  void normalize() {
    while (!mag_.empty() && mag_.back() == 0) mag_.pop_back();
    if (mag_.empty()) neg_ = false;
  }

  void assign_signed(long long value) {
    neg_ = value < 0;
    unsigned long long mag = neg_ ? (~static_cast<unsigned long long>(value) + 1ull)
                                  : static_cast<unsigned long long>(value);
    assign_unsigned_mag(mag);
    if (mag_.empty()) neg_ = false;
  }
  void assign_unsigned(unsigned long long value) {
    neg_ = false;
    assign_unsigned_mag(value);
  }
  void assign_unsigned_mag(unsigned long long mag) {
    mag_.clear();
    while (mag) {
      mag_.push_back(static_cast<limb>(mag & 0xFFFFFFFFull));
      mag >>= 32;
    }
  }

  // --- small-value helpers (operate on the magnitude in place) --------------
  void mul_small(limb m) {
    wide carry = 0;
    for (size_t i = 0; i < mag_.size(); ++i) {
      wide cur = static_cast<wide>(mag_[i]) * m + carry;
      mag_[i] = static_cast<limb>(cur & 0xFFFFFFFFull);
      carry = cur >> 32;
    }
    while (carry) {
      mag_.push_back(static_cast<limb>(carry & 0xFFFFFFFFull));
      carry >>= 32;
    }
  }
  void add_small(limb a) {
    wide carry = a;
    for (size_t i = 0; i < mag_.size() && carry; ++i) {
      wide cur = static_cast<wide>(mag_[i]) + carry;
      mag_[i] = static_cast<limb>(cur & 0xFFFFFFFFull);
      carry = cur >> 32;
    }
    while (carry) {
      mag_.push_back(static_cast<limb>(carry & 0xFFFFFFFFull));
      carry >>= 32;
    }
  }
  static limb div_small_inplace(std::vector<limb>& mag, limb d) {
    wide rem = 0;
    for (size_t i = mag.size(); i-- > 0;) {
      wide cur = (rem << 32) | mag[i];
      mag[i] = static_cast<limb>(cur / d);
      rem = cur % d;
    }
    while (!mag.empty() && mag.back() == 0) mag.pop_back();
    return static_cast<limb>(rem);
  }

  // --- magnitude arithmetic -------------------------------------------------
  static int cmp_mag(const std::vector<limb>& a, const std::vector<limb>& b) {
    if (a.size() != b.size()) return a.size() < b.size() ? -1 : 1;
    for (size_t i = a.size(); i-- > 0;) {
      if (a[i] != b[i]) return a[i] < b[i] ? -1 : 1;
    }
    return 0;
  }
  static std::vector<limb> add_mag(const std::vector<limb>& a, const std::vector<limb>& b) {
    const std::vector<limb>& big = a.size() >= b.size() ? a : b;
    const std::vector<limb>& small = a.size() >= b.size() ? b : a;
    std::vector<limb> r;
    r.reserve(big.size() + 1);
    wide carry = 0;
    for (size_t i = 0; i < big.size(); ++i) {
      wide cur = static_cast<wide>(big[i]) + carry + (i < small.size() ? small[i] : 0);
      r.push_back(static_cast<limb>(cur & 0xFFFFFFFFull));
      carry = cur >> 32;
    }
    if (carry) r.push_back(static_cast<limb>(carry));
    return r;
  }
  // Requires a >= b (by magnitude).
  static std::vector<limb> sub_mag(const std::vector<limb>& a, const std::vector<limb>& b) {
    std::vector<limb> r;
    r.reserve(a.size());
    int64_t borrow = 0;
    for (size_t i = 0; i < a.size(); ++i) {
      int64_t cur = static_cast<int64_t>(a[i]) - borrow - (i < b.size() ? b[i] : 0);
      if (cur < 0) {
        cur += static_cast<int64_t>(kBase);
        borrow = 1;
      } else {
        borrow = 0;
      }
      r.push_back(static_cast<limb>(cur));
    }
    while (!r.empty() && r.back() == 0) r.pop_back();
    return r;
  }
  static std::vector<limb> mul_mag(const std::vector<limb>& a, const std::vector<limb>& b) {
    std::vector<limb> r(a.size() + b.size(), 0);
    for (size_t i = 0; i < a.size(); ++i) {
      wide carry = 0;
      wide ai = a[i];
      for (size_t j = 0; j < b.size(); ++j) {
        wide cur = static_cast<wide>(r[i + j]) + ai * b[j] + carry;
        r[i + j] = static_cast<limb>(cur & 0xFFFFFFFFull);
        carry = cur >> 32;
      }
      r[i + b.size()] += static_cast<limb>(carry);
    }
    while (!r.empty() && r.back() == 0) r.pop_back();
    return r;
  }

  // Bit helpers for binary long division.
  static size_t bit_length(const std::vector<limb>& a) {
    if (a.empty()) return 0;
    size_t top = a.size() - 1;
    limb v = a[top];
    size_t bits = 0;
    while (v) { ++bits; v >>= 1; }
    return top * 32 + bits;
  }
  static bool test_bit(const std::vector<limb>& a, size_t i) {
    size_t w = i >> 5, b = i & 31;
    if (w >= a.size()) return false;
    return (a[w] >> b) & 1u;
  }
  static void set_bit(std::vector<limb>& a, size_t i) {
    size_t w = i >> 5, b = i & 31;
    if (w >= a.size()) a.resize(w + 1, 0);
    a[w] |= (limb(1) << b);
  }
  static void shl1(std::vector<limb>& a) {  // a <<= 1
    limb carry = 0;
    for (size_t i = 0; i < a.size(); ++i) {
      limb next = a[i] >> 31;
      a[i] = (a[i] << 1) | carry;
      carry = next;
    }
    if (carry) a.push_back(carry);
  }

  // Magnitude division: computes q = floor(a / b), r = a mod b (b != 0).
  static void divmod_mag(const std::vector<limb>& a, const std::vector<limb>& b,
                         std::vector<limb>& q, std::vector<limb>& r) {
    q.clear();
    r.clear();
    if (cmp_mag(a, b) < 0) {
      r = a;
      while (!r.empty() && r.back() == 0) r.pop_back();
      return;
    }
    // Fast path: single-limb divisor.
    if (b.size() == 1) {
      q = a;
      limb rem = div_small_inplace(q, b[0]);
      if (rem) r.push_back(rem);
      return;
    }
    size_t n = bit_length(a);
    for (size_t i = n; i-- > 0;) {
      shl1(r);
      if (test_bit(a, i)) {
        if (r.empty()) r.push_back(0);
        r[0] |= 1u;
      }
      if (cmp_mag(r, b) >= 0) {
        r = sub_mag(r, b);
        set_bit(q, i);
      }
    }
    while (!q.empty() && q.back() == 0) q.pop_back();
    while (!r.empty() && r.back() == 0) r.pop_back();
  }
};

}  // namespace bignum

#endif  // BIGNUM_BIGINT_H
