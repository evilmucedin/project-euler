#pragma once

// Symbolic differentiation and integration of formulas.
//
//   using namespace calculus;
//   Expression f = parse("x*x");
//   differentiate(f).toString();          // "2*x"
//   integrate(f).toString();              // "x^3/3"
//   integrate(f, "x", 0, 3);              // 9
//   f.evaluate("x", 5);                   // 25
//
// Formulas are built by parse() or with the overloaded operators and the
// sin/cos/... helpers below. Every constructor simplifies locally (constant
// folding, x+0, x*1, x*x -> x^2, ...), so results come out readable.
//
// Grammar accepted by parse():
//   numbers    1, 2.5, 1e-3        constants  pi, e
//   operators  + - * / ^ (right associative), unary -, parentheses
//   functions  sin cos tan exp log (natural; "ln" is an alias) sqrt abs
//              asin acos atan (also arcsin, arccos, arctan)
//              sinh cosh tanh asinh acosh atanh
//              sec csc cot log10 log2 (rewritten as 1/cos(x), log(x)/log(10), ...)
//   anything else made of letters, digits and '_' is a variable.
// Unary minus binds looser than ^, so -x^2 is -(x^2).

#include <map>
#include <memory>
#include <stdexcept>
#include <string>

namespace calculus {

class ParseError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

// Thrown by the symbolic integrate() when no rule matches the integrand.
class IntegrationError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

// Thrown by evaluate() when a variable has no value.
class EvaluationError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

enum class Op {
    Constant,
    Variable,
    Add,
    Sub,
    Mul,
    Div,
    Pow,
    Neg,
    Sin,
    Cos,
    Tan,
    Exp,
    Log,
    Sqrt,
    Abs,
    Asin,
    Acos,
    Atan,
    Sinh,
    Cosh,
    Tanh,
    Asinh,
    Acosh,
    Atanh,
};

struct Node;

using Variables = std::map<std::string, double>;

// Immutable expression tree; copies share nodes and are cheap.
class Expression {
public:
    Expression(double value = 0);

    static Expression variable(const std::string& name);

    double evaluate(const Variables& vars = {}) const;
    // Shorthand for a formula with a single variable.
    double evaluate(const std::string& var, double value) const;

    std::string toString() const;

    // True when the expression does not mention `var`.
    bool isFreeOf(const std::string& var) const;
    bool isConstant() const;
    // Numeric value; only meaningful when isConstant().
    double value() const;

    Op op() const;
    // Operands of unary (left only) and binary nodes.
    Expression left() const;
    Expression right() const;

    // Structural equality, e.g. parse("2*x") == parse("x*2") after
    // simplification moves constants to the left.
    bool operator==(const Expression& other) const;
    bool operator!=(const Expression& other) const { return !(*this == other); }

    // Raw node without any simplification; `b` is ignored for unary ops.
    static Expression make(Op op, Expression a, Expression b = Expression());

private:
    explicit Expression(std::shared_ptr<const Node> node);

    std::shared_ptr<const Node> node_;
};

Expression parse(const std::string& text);

Expression operator+(const Expression& a, const Expression& b);
Expression operator-(const Expression& a, const Expression& b);
Expression operator*(const Expression& a, const Expression& b);
Expression operator/(const Expression& a, const Expression& b);
Expression operator-(const Expression& a);
Expression pow(const Expression& base, const Expression& exponent);
Expression sin(const Expression& a);
Expression cos(const Expression& a);
Expression tan(const Expression& a);
Expression exp(const Expression& a);
Expression log(const Expression& a);
Expression sqrt(const Expression& a);
Expression abs(const Expression& a);
Expression asin(const Expression& a);
Expression acos(const Expression& a);
Expression atan(const Expression& a);
Expression sinh(const Expression& a);
Expression cosh(const Expression& a);
Expression tanh(const Expression& a);
Expression asinh(const Expression& a);
Expression acosh(const Expression& a);
Expression atanh(const Expression& a);

// Replaces every occurrence of `var` with `value`.
Expression substitute(const Expression& e, const std::string& var, const Expression& value);

// Multiplies out products and small integer powers of sums, e.g.
// (x+1)^2 -> x^2 + 2*x + 1.
Expression expand(const Expression& e);

// Symbolic derivative d e / d var.
Expression differentiate(const Expression& e, const std::string& var = "x");
// n-th derivative.
Expression differentiate(const Expression& e, const std::string& var, int order);

// Symbolic antiderivative (without the +C). Throws IntegrationError when none
// of the rules apply. Rules, tried roughly in this order:
//   - linearity and polynomials;
//   - every supported function of a linear argument a*x + b, including the
//     inverse trigonometric and (inverse) hyperbolic ones;
//   - integer powers of sin, cos, tan, sinh, cosh, tanh, e.g. sin(x)^3,
//     1/cos(x)^2, 1/sin(x), by reduction formulas;
//   - rational functions P(x)/Q(x) with numeric coefficients and Q of degree
//     1 or 2 (polynomial division, then log / atan);
//   - (A*x + B)/sqrt(Q), sqrt(Q) for quadratic Q (asin, asinh, log);
//   - substitution u = g(x) for any subexpression g, e.g.
//     cos(x)/(1 + sin(x)^2) -> atan(sin(x));
//   - products sin*cos, sin*sin, cos*cos, exp*sin, exp*cos of linear arguments;
//   - integration by parts of polynomial * exp/sin/cos/sinh/cosh/c^x and of
//     polynomial * log/atan/asin/...;
//   - expanding products and powers of sums.
Expression integrate(const Expression& e, const std::string& var = "x");

// Definite integral over [a, b]. Uses the antiderivative when one is found
// and it is finite at both ends, otherwise falls back to numericIntegral.
// The integrand is assumed continuous on [a, b]: the antiderivative of 1/x^2
// happily "integrates" across the pole at 0.
double integrate(const Expression& e, const std::string& var, double a, double b, const Variables& vars = {});

// Adaptive Simpson quadrature; other variables are taken from `vars`.
double numericIntegral(const Expression& e, const std::string& var, double a, double b,
                       double tolerance = 1e-10, const Variables& vars = {});

// Central finite difference, for cross-checking symbolic results.
double numericDerivative(const Expression& e, const std::string& var, double at,
                         const Variables& vars = {});

}  // namespace calculus
