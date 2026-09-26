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

// Replaces every occurrence of `var` with `value`.
Expression substitute(const Expression& e, const std::string& var, const Expression& value);

// Multiplies out products and small integer powers of sums, e.g.
// (x+1)^2 -> x^2 + 2*x + 1.
Expression expand(const Expression& e);

// Symbolic derivative d e / d var.
Expression differentiate(const Expression& e, const std::string& var = "x");
// n-th derivative.
Expression differentiate(const Expression& e, const std::string& var, int order);

// Symbolic antiderivative (without the +C). Supports linearity, powers,
// exp/sin/cos/tan/log/sqrt of linear arguments, polynomials, integration by
// parts of polynomial * exp/sin/cos, and substitution f(g(x)) * g'(x).
// Throws IntegrationError when none of these apply.
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
