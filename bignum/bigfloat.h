// bigfloat.h - Arbitrary precision decimal floating point.
//
// Second-class implementation built on top of BigInt. A value is represented
// as  significand * 10^exponent  where `significand` is a BigInt and
// `exponent` is a signed integer. This is a decimal (base-10) floating point,
// so values such as 0.1 are represented exactly.
//
// Division is performed to a configurable number of significant digits
// (default kDefaultDivPrecision) since exact results are generally
// non-terminating.
//
// Header-only; C++17. Depends only on bigint.h and the standard library.
#ifndef BIGNUM_BIGFLOAT_H
#define BIGNUM_BIGFLOAT_H

#include <algorithm>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <string>

#include "bignum/bigint.h"

namespace bignum {

class BigFloat {
 public:
  // Number of significant digits kept when dividing.
  static constexpr int kDefaultDivPrecision = 50;

  // --- Construction ---------------------------------------------------------
  BigFloat() : sig_(0), exp_(0) {}
  BigFloat(const BigInt& integer) : sig_(integer), exp_(0) {}
  BigFloat(long long value) : sig_(value), exp_(0) {}
  BigFloat(int value) : sig_(value), exp_(0) {}
  BigFloat(BigInt sig, long long exp) : sig_(std::move(sig)), exp_(exp) {}

  explicit BigFloat(const std::string& text) { *this = parse(text); }

  // --- State queries --------------------------------------------------------
  bool is_zero() const { return sig_.is_zero(); }
  int sign() const { return sig_.sign(); }
  const BigInt& significand() const { return sig_; }
  long long exponent() const { return exp_; }

  BigFloat abs() const { return BigFloat(sig_.abs(), exp_); }
  BigFloat operator-() const { return BigFloat(-sig_, exp_); }
  BigFloat operator+() const { return *this; }

  // --- Arithmetic -----------------------------------------------------------
  BigFloat& operator+=(const BigFloat& o) {
    BigInt a = sig_, b = o.sig_;
    long long e = align(a, exp_, b, o.exp_);
    sig_ = a + b;
    exp_ = e;
    trim();
    return *this;
  }
  BigFloat& operator-=(const BigFloat& o) { return *this += (-o); }

  BigFloat& operator*=(const BigFloat& o) {
    sig_ *= o.sig_;
    exp_ += o.exp_;
    trim();
    return *this;
  }

  BigFloat& operator/=(const BigFloat& o) {
    *this = divide(*this, o, kDefaultDivPrecision);
    return *this;
  }

  friend BigFloat operator+(BigFloat a, const BigFloat& b) { a += b; return a; }
  friend BigFloat operator-(BigFloat a, const BigFloat& b) { a -= b; return a; }
  friend BigFloat operator*(BigFloat a, const BigFloat& b) { a *= b; return a; }
  friend BigFloat operator/(BigFloat a, const BigFloat& b) { a /= b; return a; }

  // Divide a/b keeping at least `digits` significant digits in the result.
  static BigFloat divide(const BigFloat& a, const BigFloat& b, int digits) {
    if (b.is_zero()) throw std::domain_error("BigFloat division by zero");
    if (a.is_zero()) return BigFloat();
    if (digits < 1) digits = 1;
    // Scale the numerator up so the quotient has enough significant digits.
    long long shift = static_cast<long long>(digits) + 2;
    BigInt num = a.sig_ * BigInt::pow10(static_cast<unsigned>(shift));
    BigInt q = BigInt::divmod(num, b.sig_).first;
    BigFloat result(q, a.exp_ - b.exp_ - shift);
    result.trim();
    return result.round_to_significant(digits);
  }

  // Square root to `digits` significant decimal digits. Throws for negatives.
  // Computed exactly via integer sqrt of a scaled significand:
  //   sqrt(sig * 10^e) = isqrt(sig * 10^(e + 2P)) * 10^-P
  static BigFloat sqrt(const BigFloat& x, int digits = kDefaultDivPrecision) {
    if (x.sign() < 0) throw std::domain_error("BigFloat::sqrt of negative");
    if (x.is_zero()) return BigFloat();
    if (digits < 1) digits = 1;
    long long P = static_cast<long long>(digits) + 5;  // guard digits
    long long shift = x.exp_ + 2 * P;
    BigInt scaled = x.sig_;
    if (shift >= 0) {
      scaled *= BigInt::pow10(static_cast<unsigned>(shift));
    } else {
      scaled = BigInt::divmod(scaled, BigInt::pow10(static_cast<unsigned>(-shift))).first;
    }
    BigFloat result(scaled.isqrt(), -P);
    result.trim();
    return result.round_to_significant(digits);
  }

  // --- Comparison -----------------------------------------------------------
  int compare(const BigFloat& o) const {
    if (sign() != o.sign()) return sign() < o.sign() ? -1 : 1;
    BigInt a = sig_, b = o.sig_;
    align(a, exp_, b, o.exp_);
    return a.compare(b);
  }
  friend bool operator==(const BigFloat& a, const BigFloat& b) { return a.compare(b) == 0; }
  friend bool operator!=(const BigFloat& a, const BigFloat& b) { return a.compare(b) != 0; }
  friend bool operator<(const BigFloat& a, const BigFloat& b) { return a.compare(b) < 0; }
  friend bool operator<=(const BigFloat& a, const BigFloat& b) { return a.compare(b) <= 0; }
  friend bool operator>(const BigFloat& a, const BigFloat& b) { return a.compare(b) > 0; }
  friend bool operator>=(const BigFloat& a, const BigFloat& b) { return a.compare(b) >= 0; }

  // --- String conversion ----------------------------------------------------
  // Plain decimal notation (no exponent), e.g. "-12.3400" -> "-12.34".
  std::string to_string() const {
    if (is_zero()) return "0";
    bool neg = sig_.is_negative();
    std::string digits = sig_.abs().to_string();
    std::string out;
    if (exp_ >= 0) {
      out = digits + std::string(static_cast<size_t>(exp_), '0');
    } else {
      long long frac = -exp_;
      if (static_cast<long long>(digits.size()) > frac) {
        size_t point = digits.size() - static_cast<size_t>(frac);
        out = digits.substr(0, point) + "." + digits.substr(point);
      } else {
        out = "0." + std::string(static_cast<size_t>(frac) - digits.size(), '0') + digits;
      }
      // Strip trailing zeros / dangling point.
      while (out.back() == '0') out.pop_back();
      if (out.back() == '.') out.pop_back();
    }
    return neg ? "-" + out : out;
  }

  // Scientific notation with `digits` significant figures, e.g. "1.23e4".
  std::string to_scientific(int digits = 17) const {
    if (is_zero()) return "0e0";
    if (digits < 1) digits = 1;
    BigFloat r = round_to_significant(digits);
    bool neg = r.sig_.is_negative();
    std::string d = r.sig_.abs().to_string();
    long long point_exp = r.exp_ + static_cast<long long>(d.size()) - 1;
    std::string mant(1, d[0]);
    if (d.size() > 1) mant += "." + d.substr(1);
    std::string out = mant + "e" + std::to_string(point_exp);
    return neg ? "-" + out : out;
  }

  // Parse decimal text with optional sign, fraction, and exponent:
  //   [+-]ddd[.ddd][(e|E)[+-]ddd]
  static BigFloat parse(const std::string& text) {
    size_t i = 0, n = text.size();
    while (i < n && std::isspace(static_cast<unsigned char>(text[i]))) ++i;
    std::string mant;
    bool neg = false;
    if (i < n && (text[i] == '+' || text[i] == '-')) {
      neg = (text[i] == '-');
      ++i;
    }
    long long frac_digits = 0;
    bool seen_point = false, any_digit = false;
    for (; i < n; ++i) {
      char c = text[i];
      if (c == '.') {
        if (seen_point) throw std::invalid_argument("BigFloat::parse: multiple '.'");
        seen_point = true;
      } else if (c >= '0' && c <= '9') {
        mant.push_back(c);
        if (seen_point) ++frac_digits;
        any_digit = true;
      } else {
        break;
      }
    }
    if (!any_digit) throw std::invalid_argument("BigFloat::parse: no digits in '" + text + "'");
    long long exp10 = 0;
    if (i < n && (text[i] == 'e' || text[i] == 'E')) {
      ++i;
      bool eneg = false;
      if (i < n && (text[i] == '+' || text[i] == '-')) {
        eneg = (text[i] == '-');
        ++i;
      }
      bool any_e = false;
      long long val = 0;
      for (; i < n; ++i) {
        char c = text[i];
        if (c < '0' || c > '9') break;
        val = val * 10 + (c - '0');
        any_e = true;
      }
      if (!any_e) throw std::invalid_argument("BigFloat::parse: malformed exponent");
      exp10 = eneg ? -val : val;
    }
    if (mant.empty()) mant = "0";
    BigInt sig = BigInt::parse((neg ? "-" : "") + mant);
    BigFloat result(sig, exp10 - frac_digits);
    result.trim();
    return result;
  }

  // --- Stream operators -----------------------------------------------------
  friend std::ostream& operator<<(std::ostream& os, const BigFloat& v) {
    return os << v.to_string();
  }
  friend std::istream& operator>>(std::istream& is, BigFloat& v) {
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

 private:
  BigInt sig_;     // significand
  long long exp_;  // power of ten

  // Remove trailing zero digits from the significand, raising the exponent.
  void trim() {
    if (sig_.is_zero()) {
      exp_ = 0;
      return;
    }
    BigInt ten(10);
    while (true) {
      auto qr = BigInt::divmod(sig_, ten);
      if (!qr.second.is_zero()) break;
      sig_ = qr.first;
      ++exp_;
    }
  }

  // Round to `digits` significant figures (round-half-up on magnitude).
  BigFloat round_to_significant(int digits) const {
    if (is_zero() || digits < 1) return *this;
    std::string d = sig_.abs().to_string();
    int len = static_cast<int>(d.size());
    if (len <= digits) return *this;
    int drop = len - digits;
    BigInt scale = BigInt::pow10(static_cast<unsigned>(drop));
    auto qr = BigInt::divmod(sig_.abs(), scale);
    BigInt q = qr.first;
    BigInt twice_rem = qr.second * BigInt(2);
    if (twice_rem >= scale) q += BigInt(1);  // round half up
    if (sig_.is_negative()) q = -q;
    BigFloat result(q, exp_ + drop);
    result.trim();
    return result;
  }

  // Rewrite a (at exponent ea) and b (at exponent eb) to a common exponent
  // (the smaller of the two) and return that exponent.
  static long long align(BigInt& a, long long ea, BigInt& b, long long eb) {
    if (ea == eb) return ea;
    if (ea > eb) {
      a *= BigInt::pow10(static_cast<unsigned>(ea - eb));
      return eb;
    }
    b *= BigInt::pow10(static_cast<unsigned>(eb - ea));
    return ea;
  }
};

}  // namespace bignum

#endif  // BIGNUM_BIGFLOAT_H
