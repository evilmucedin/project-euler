// Tests for lib/calculus. They double as usage examples: each test shows a
// formula, what the library returns for it, and how to check the result.

#include "lib/calculus/calculus.h"

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

#include "gtest/gtest.h"

using namespace calculus;

namespace {

std::string derivative(const std::string& formula) { return differentiate(parse(formula)).toString(); }

std::string antiderivative(const std::string& formula) { return integrate(parse(formula)).toString(); }

// d/dx of the antiderivative must give back the integrand. Checked numerically
// at a few points, since the two forms need not be structurally identical.
void expectAntiderivative(const std::string& formula, const std::vector<double>& points) {
    Expression f = parse(formula);
    Expression F = integrate(f);
    Expression dF = differentiate(F);
    for (double x : points) {
        EXPECT_NEAR(dF.evaluate("x", x), f.evaluate("x", x), 1e-9 * std::max(1.0, std::fabs(f.evaluate("x", x))))
            << formula << " -> " << F.toString() << " at x = " << x;
        EXPECT_NEAR(numericDerivative(F, "x", x), f.evaluate("x", x), 1e-6)
            << formula << " -> " << F.toString() << " at x = " << x;
    }
}

}  // namespace

// The example from the task: x*x.
TEST(Calculus, SquareExample) {
    Expression f = parse("x*x");
    EXPECT_EQ(f.toString(), "x^2");
    EXPECT_EQ(differentiate(f).toString(), "2*x");
    EXPECT_EQ(integrate(f).toString(), "x^3/3");
    EXPECT_DOUBLE_EQ(integrate(f, "x", 0, 3), 9);
    EXPECT_DOUBLE_EQ(f.evaluate("x", 5), 25);
}

// Formulas can also be assembled in C++ instead of parsed.
TEST(Calculus, BuildWithOperators) {
    Expression x = Expression::variable("x");
    Expression f = 3 * pow(x, 2) + 2 * x + 1;
    EXPECT_EQ(f.toString(), "3*x^2 + 2*x + 1");
    EXPECT_EQ(differentiate(f).toString(), "6*x + 2");
    EXPECT_EQ(integrate(f).toString(), "x^3 + x^2 + x");
    EXPECT_EQ(parse("3*x^2 + 2*x + 1"), f);
}

TEST(Parse, PrecedenceAndAssociativity) {
    EXPECT_DOUBLE_EQ(parse("2 + 3*4").evaluate(), 14);
    EXPECT_DOUBLE_EQ(parse("(2 + 3)*4").evaluate(), 20);
    EXPECT_DOUBLE_EQ(parse("2^3^2").evaluate(), 512);  // 2^(3^2)
    EXPECT_DOUBLE_EQ(parse("-2^2").evaluate(), -4);    // -(2^2)
    EXPECT_DOUBLE_EQ(parse("2^-1").evaluate(), 0.5);
    EXPECT_DOUBLE_EQ(parse("8/4/2").evaluate(), 1);
    EXPECT_DOUBLE_EQ(parse("1 - 2 - 3").evaluate(), -4);
    EXPECT_DOUBLE_EQ(parse("1.5e2").evaluate(), 150);
}

TEST(Parse, FunctionsAndConstants) {
    EXPECT_NEAR(parse("sin(pi/2)").evaluate(), 1, 1e-15);
    EXPECT_NEAR(parse("ln(e)").evaluate(), 1, 1e-15);
    EXPECT_NEAR(parse("sqrt(16) + abs(-3) + exp(0) + cos(0) + tan(0)").evaluate(), 9, 1e-15);
}

TEST(Parse, InverseAndHyperbolicFunctions) {
    EXPECT_NEAR(parse("asin(1) + acos(1) + atan(1)").evaluate(), std::acos(-1.0) * 3 / 4, 1e-15);
    EXPECT_NEAR(parse("arctan(1)").evaluate(), std::atan(1.0), 1e-15);
    EXPECT_NEAR(parse("cosh(1)^2 - sinh(1)^2").evaluate(), 1, 1e-14);
    EXPECT_NEAR(parse("tanh(atanh(0.3)) + asinh(sinh(2)) + acosh(cosh(2))").evaluate(), 4.3, 1e-14);
    // sec, csc, cot, log10, log2 are rewritten in terms of the others.
    EXPECT_EQ(parse("sec(x)").toString(), "1/cos(x)");
    EXPECT_EQ(parse("cot(x)").toString(), "1/tan(x)");
    EXPECT_NEAR(parse("log10(1000) + log2(8)").evaluate(), 6, 1e-14);
}

TEST(Parse, SeveralVariables) {
    Expression f = parse("x^2*y + sin(y)");
    EXPECT_DOUBLE_EQ(f.evaluate({{"x", 3}, {"y", 0}}), 0);
    EXPECT_NEAR(f.evaluate({{"x", 2}, {"y", 1}}), 4 + std::sin(1.0), 1e-15);
    EXPECT_THROW(f.evaluate("x", 1), EvaluationError);
}

TEST(Parse, Errors) {
    EXPECT_THROW(parse(""), ParseError);
    EXPECT_THROW(parse("x +"), ParseError);
    EXPECT_THROW(parse("(x + 1"), ParseError);
    EXPECT_THROW(parse("x + 1)"), ParseError);
    EXPECT_THROW(parse("foo(x)"), ParseError);
    EXPECT_THROW(parse("x $ 2"), ParseError);
}

// Printed formulas parse back to the same value.
TEST(Parse, RoundTrip) {
    for (const char* formula : {"x^2/3 - 2*x", "-x^2", "(x + 1)^(x - 1)", "2^-x", "1/(2*sqrt(x))", "x/(y*z)",
                                "-(x + y)", "x - (y - z)", "exp(-x)*sin(3*x + 1)", "(-x)^3"}) {
        Expression f = parse(formula);
        Expression g = parse(f.toString());
        Variables at = {{"x", 1.7}, {"y", 0.3}, {"z", -2.1}};
        EXPECT_NEAR(f.evaluate(at), g.evaluate(at), 1e-12) << formula << " printed as " << f.toString();
    }
}

TEST(Simplify, Basics) {
    EXPECT_EQ(parse("x + 0").toString(), "x");
    EXPECT_EQ(parse("1*x*1").toString(), "x");
    EXPECT_EQ(parse("0*sin(x)").toString(), "0");
    EXPECT_EQ(parse("x - x").toString(), "0");
    EXPECT_EQ(parse("x + x").toString(), "2*x");
    EXPECT_EQ(parse("x*2*3").toString(), "6*x");
    EXPECT_EQ(parse("x^2*x^3").toString(), "x^5");
    EXPECT_EQ(parse("x^3/x").toString(), "x^2");
    EXPECT_EQ(parse("2 + 3*4").toString(), "14");
    EXPECT_EQ(parse("--x").toString(), "x");
    EXPECT_EQ(parse("x + -2").toString(), "x - 2");
    EXPECT_EQ(parse("log(exp(x))").toString(), "x");
    EXPECT_EQ(parse("6*x/4").toString(), "3*x/2");
    EXPECT_EQ(parse("sin(asin(x))").toString(), "x");
    EXPECT_EQ(parse("cos(-x) + sin(-2*x)").toString(), "cos(x) - sin(2*x)");  // even and odd functions
    EXPECT_EQ(parse("x/(-4)").toString(), "-x/4");
    EXPECT_EQ(parse("exp(x)^2").toString(), "exp(2*x)");
}

TEST(Expand, Polynomials) {
    EXPECT_EQ(expand(parse("(x + 1)^2")).toString(), "x^2 + 2*x + 1");
    EXPECT_EQ(expand(parse("(x + 1)*(x - 1)")).toString(), "x^2 - 1");
    EXPECT_EQ(expand(parse("(x^2 + 1)/x")).toString(), "x + 1/x");
    Expression cube = expand(parse("(2*x - 3)^3"));
    for (double x : {-1.0, 0.0, 0.5, 2.0}) {
        EXPECT_NEAR(cube.evaluate("x", x), std::pow(2 * x - 3, 3), 1e-9);
    }
}

TEST(Differentiate, Polynomials) {
    EXPECT_EQ(derivative("5"), "0");
    EXPECT_EQ(derivative("x"), "1");
    EXPECT_EQ(derivative("x^3"), "3*x^2");
    EXPECT_EQ(derivative("x^3/3"), "x^2");
    EXPECT_EQ(derivative("4*x^3 - 2*x + 7"), "12*x^2 - 2");
    EXPECT_EQ(derivative("1/x"), "-1/x^2");
    EXPECT_EQ(derivative("sqrt(x)"), "1/(2*sqrt(x))");
}

TEST(Differentiate, ElementaryFunctions) {
    EXPECT_EQ(derivative("sin(x)"), "cos(x)");
    EXPECT_EQ(derivative("cos(x)"), "-sin(x)");
    EXPECT_EQ(derivative("tan(x)"), "1/cos(x)^2");
    EXPECT_EQ(derivative("exp(x)"), "exp(x)");
    EXPECT_EQ(derivative("log(x)"), "1/x");
    EXPECT_EQ(derivative("2^x"), "0.693147180559945*2^x");  // log(2)*2^x
}

TEST(Differentiate, InverseAndHyperbolicFunctions) {
    EXPECT_EQ(derivative("asin(x)"), "1/sqrt(1 - x^2)");
    EXPECT_EQ(derivative("acos(x)"), "-1/sqrt(1 - x^2)");
    EXPECT_EQ(derivative("atan(x)"), "1/(x^2 + 1)");
    EXPECT_EQ(derivative("sinh(x)"), "cosh(x)");
    EXPECT_EQ(derivative("cosh(x)"), "sinh(x)");
    EXPECT_EQ(derivative("tanh(x)"), "1/cosh(x)^2");
    EXPECT_EQ(derivative("asinh(x)"), "1/sqrt(x^2 + 1)");
    EXPECT_EQ(derivative("acosh(x)"), "1/sqrt(x^2 - 1)");
    EXPECT_EQ(derivative("atanh(x)"), "1/(1 - x^2)");
    EXPECT_EQ(derivative("atan(x^2)"), "2*x/(x^4 + 1)");
    EXPECT_EQ(derivative("sec(x)"), "sin(x)/cos(x)^2");
}

TEST(Differentiate, RulesOfCalculus) {
    EXPECT_EQ(derivative("x*sin(x)"), "sin(x) + x*cos(x)");        // product rule
    EXPECT_EQ(derivative("sin(x^2)"), "2*cos(x^2)*x");            // chain rule
    EXPECT_EQ(derivative("exp(3*x)"), "3*exp(3*x)");              // linear chain
    EXPECT_EQ(derivative("sin(x)/x"), "(cos(x)*x - sin(x))/x^2");  // quotient rule
    // x^x = e^(x log x): d/dx = x^x * (log x + 1).
    Expression d = differentiate(parse("x^x"));
    EXPECT_NEAR(d.evaluate("x", 2), 4 * (std::log(2.0) + 1), 1e-12);
}

TEST(Differentiate, PartialAndHigherOrder) {
    Expression f = parse("x^2*y + y^3");
    EXPECT_EQ(differentiate(f, "x").toString(), "2*x*y");
    EXPECT_EQ(differentiate(f, "y").toString(), "x^2 + 3*y^2");
    EXPECT_EQ(differentiate(parse("x^4"), "x", 2).toString(), "12*x^2");
    EXPECT_EQ(differentiate(parse("sin(x)"), "x", 4).toString(), "sin(x)");
}

TEST(Differentiate, MatchesFiniteDifferences) {
    for (const char* formula : {"x^3 - 2*x", "sin(x)*exp(x)", "log(x^2 + 1)", "sqrt(x)/(1 + x)", "x^x",
                                "tan(x/2)", "abs(x - 3)", "cos(sin(x))", "asin(x/3)", "acos(x^2/9)",
                                "atan(2*x + 1)", "sinh(x)*cosh(2*x)", "tanh(x^2)", "asinh(x^3)", "acosh(x + 2)",
                                "atanh(x/4)", "cot(x) + sec(x) + csc(x)"}) {
        Expression f = parse(formula);
        Expression df = differentiate(f);
        for (double x : {0.4, 1.1, 2.5}) {
            const double expected = numericDerivative(f, "x", x);
            EXPECT_NEAR(df.evaluate("x", x), expected, 1e-7 * std::max(1.0, std::abs(expected)))
                << formula << " -> " << df.toString() << " at x = " << x;
        }
    }
}

TEST(Integrate, Polynomials) {
    EXPECT_EQ(antiderivative("3"), "3*x");
    EXPECT_EQ(antiderivative("x"), "x^2/2");
    EXPECT_EQ(antiderivative("3*x^2"), "x^3");
    EXPECT_EQ(antiderivative("x^2 + 2*x + 1"), "x^3/3 + x^2 + x");
    EXPECT_EQ(antiderivative("1/x^2"), "-1/x");
    EXPECT_EQ(antiderivative("1/x"), "log(abs(x))");
    EXPECT_EQ(antiderivative("sqrt(x)"), "2*x^1.5/3");
    // Products and powers of sums are multiplied out first.
    expectAntiderivative("(x + 1)*(x - 2)", {-1, 0, 2});
    expectAntiderivative("(2*x + 1)^3", {-1, 0, 2});
    expectAntiderivative("(x^2 + 1)/x", {0.5, 1, 3});
}

TEST(Integrate, ElementaryFunctions) {
    EXPECT_EQ(antiderivative("sin(x)"), "-cos(x)");
    EXPECT_EQ(antiderivative("cos(x)"), "sin(x)");
    EXPECT_EQ(antiderivative("exp(x)"), "exp(x)");
    EXPECT_EQ(antiderivative("log(x)"), "x*log(x) - x");
    EXPECT_EQ(antiderivative("tan(x)"), "-log(abs(cos(x)))");
    expectAntiderivative("2^x", {-1, 0, 1.5});
    expectAntiderivative("abs(x)", {-2, -0.5, 0.5, 2});
}

// f(a*x + b) integrates to F(a*x + b)/a.
TEST(Integrate, LinearArguments) {
    EXPECT_EQ(antiderivative("exp(2*x)"), "exp(2*x)/2");
    EXPECT_EQ(antiderivative("sin(3*x + 1)"), "-cos(3*x + 1)/3");
    EXPECT_EQ(antiderivative("1/(2*x + 1)"), "log(abs(2*x + 1))/2");
    expectAntiderivative("(3*x - 1)^5", {-1, 0, 1});
    expectAntiderivative("sqrt(4*x + 1)", {0, 1, 2});
}

// ∫ h(g(x)) g'(x) dx = H(g(x)).
TEST(Integrate, Substitution) {
    EXPECT_EQ(antiderivative("2*x*cos(x^2)"), "sin(x^2)");
    EXPECT_EQ(antiderivative("sin(x)*cos(x)"), "sin(x)^2/2");
    EXPECT_EQ(antiderivative("x*exp(x^2)"), "exp(x^2)/2");
    EXPECT_EQ(antiderivative("2*x/(x^2 + 1)"), "log(abs(x^2 + 1))");
    expectAntiderivative("cos(x)*exp(sin(x))", {-1, 0, 2});
    expectAntiderivative("3*x^2*(x^3 + 1)^4", {-1, 0, 1});
    // The substitution may be any subexpression, leading to atan, log, ...
    EXPECT_EQ(antiderivative("cos(x)/(1 + sin(x)^2)"), "atan(sin(x))");
    EXPECT_EQ(antiderivative("2*x/(1 + x^4)"), "atan(x^2)");        // u = x^2
    EXPECT_EQ(antiderivative("exp(x)/(1 + exp(2*x))"), "atan(exp(x))");  // u = exp(x)
    EXPECT_EQ(antiderivative("1/(x*log(x))"), "log(abs(log(x)))");
    EXPECT_EQ(antiderivative("log(x)/x"), "log(x)^2/2");
    EXPECT_EQ(antiderivative("atan(x)/(1 + x^2)"), "atan(x)^2/2");
    EXPECT_EQ(antiderivative("exp(sqrt(x))/sqrt(x)"), "2*exp(sqrt(x))");
    EXPECT_EQ(antiderivative("x*sqrt(x^2 + 1)"), "(x^2 + 1)^1.5/3");
}

// ∫ p(x) t(x) dx for polynomial p and t = exp, sin, cos or log.
TEST(Integrate, ByParts) {
    EXPECT_EQ(antiderivative("x*exp(x)"), "x*exp(x) - exp(x)");
    EXPECT_EQ(antiderivative("x*sin(x)"), "sin(x) - x*cos(x)");
    expectAntiderivative("x^2*cos(x)", {-1, 0, 2});
    expectAntiderivative("x^3*exp(-2*x)", {-1, 0, 2});
    expectAntiderivative("x*log(x)", {0.5, 1, 3});
    EXPECT_EQ(antiderivative("x*atan(x)"), "x^2*atan(x)/2 - x/2 + atan(x)/2");
    EXPECT_EQ(antiderivative("x*sinh(x)"), "x*cosh(x) - sinh(x)");
    expectAntiderivative("x^2*cosh(2*x)", {-1, 0, 1});
    expectAntiderivative("asin(x)*3", {-0.5, 0, 0.5});
}

TEST(Integrate, InverseAndHyperbolicFunctions) {
    EXPECT_EQ(antiderivative("atan(x)"), "x*atan(x) - log(x^2 + 1)/2");
    EXPECT_EQ(antiderivative("asin(x)"), "x*asin(x) + sqrt(1 - x^2)");
    EXPECT_EQ(antiderivative("acos(x)"), "x*acos(x) - sqrt(1 - x^2)");
    EXPECT_EQ(antiderivative("sinh(x)"), "cosh(x)");
    EXPECT_EQ(antiderivative("cosh(2*x)"), "sinh(2*x)/2");
    EXPECT_EQ(antiderivative("tanh(x)"), "log(cosh(x))");
    EXPECT_EQ(antiderivative("asinh(x)"), "x*asinh(x) - sqrt(x^2 + 1)");
    expectAntiderivative("acosh(x)", {1.5, 2, 4});
    expectAntiderivative("atanh(x)", {-0.5, 0, 0.5});
    expectAntiderivative("atan(3*x - 1)", {-1, 0, 2});
}

// P(x)/Q(x) with Q of degree 1 or 2: polynomial division, then log and atan.
TEST(Integrate, RationalFunctions) {
    EXPECT_EQ(antiderivative("1/(x^2 + 1)"), "atan(x)");
    EXPECT_EQ(antiderivative("1/(x^2 + 4)"), "atan(x/2)/2");
    EXPECT_EQ(antiderivative("1/(x^2 + 2*x + 5)"), "atan((x + 1)/2)/2");
    EXPECT_EQ(antiderivative("1/(x^2 - 1)"), "log(abs((x - 1)/(x + 1)))/2");
    EXPECT_EQ(antiderivative("1/(x + 1)^2"), "-1/(x + 1)");
    EXPECT_EQ(antiderivative("x/(x + 1)"), "x - log(abs(x + 1))");
    EXPECT_EQ(antiderivative("x^2/(x^2 + 1)"), "x - atan(x)");
    EXPECT_EQ(antiderivative("(x + 3)/(x^2 + 2*x + 5)"), "log(abs(x^2 + 2*x + 5))/2 + atan((x + 1)/2)");
    EXPECT_EQ(antiderivative("(x^2 + 1)/(x - 2)"), "x^2/2 + 2*x + 5*log(abs(x - 2))");
    expectAntiderivative("x^3/(x^2 + 1)", {-1, 0, 2});
    expectAntiderivative("(2*x^3 - x + 4)/(3*x^2 + x + 1)", {-1, 0, 2});
    expectAntiderivative("1/(4 - x^2)", {-1, 0, 1});
    expectAntiderivative("5/(x^2 - 4*x + 4)", {-1, 0, 1});
}

// sqrt of a quadratic and its reciprocal: asin, asinh, log.
TEST(Integrate, SquareRootsOfQuadratics) {
    EXPECT_EQ(antiderivative("1/sqrt(1 - x^2)"), "asin(x)");
    EXPECT_EQ(antiderivative("1/sqrt(4 - x^2)"), "asin(x/2)");
    EXPECT_EQ(antiderivative("1/sqrt(x^2 + 1)"), "asinh(x)");
    EXPECT_EQ(antiderivative("1/sqrt(x^2 - 1)"), "log(abs(x + sqrt(x^2 - 1)))");
    EXPECT_EQ(antiderivative("1/sqrt(x^2 + 2*x + 2)"), "asinh(x + 1)");
    EXPECT_EQ(antiderivative("x/sqrt(x^2 + 1)"), "sqrt(x^2 + 1)");
    EXPECT_EQ(antiderivative("sqrt(1 - x^2)"), "x*sqrt(1 - x^2)/2 + asin(x)/2");
    EXPECT_EQ(antiderivative("sqrt(x^2 + 1)"), "x*sqrt(x^2 + 1)/2 + asinh(x)/2");
    expectAntiderivative("(x + 1)/sqrt(1 - x^2)", {-0.5, 0, 0.5});
    expectAntiderivative("sqrt(x^2 - 1)", {1.5, 2, 4});
    expectAntiderivative("sqrt(3 + 2*x - x^2)", {0, 1, 2});
    expectAntiderivative("1/sqrt(2*x^2 + 3)", {-1, 0, 2});
}

// Integer powers of trigonometric and hyperbolic functions (reduction formulas).
TEST(Integrate, TrigonometricPowers) {
    EXPECT_EQ(antiderivative("sin(x)^2"), "x/2 - sin(x)*cos(x)/2");
    EXPECT_EQ(antiderivative("tan(x)^2"), "tan(x) - x");
    EXPECT_EQ(antiderivative("1/cos(x)^2"), "tan(x)");
    EXPECT_EQ(antiderivative("sec(x)"), "log(abs(1/cos(x) + tan(x)))");
    EXPECT_EQ(antiderivative("csc(x)"), "log(abs(tan(x/2)))");
    EXPECT_EQ(antiderivative("cot(x)"), "log(abs(sin(x)))");
    EXPECT_EQ(antiderivative("1/cosh(x)^2"), "tanh(x)");
    EXPECT_EQ(antiderivative("tanh(x)^2"), "x - tanh(x)");
    for (const char* formula : {"cos(x)^2", "sin(x)^3", "cos(2*x)^5", "tan(x)^4", "sec(x)^3", "csc(x)^4",
                                "cot(x)^3", "sinh(x)^3", "cosh(x)^4"}) {
        expectAntiderivative(formula, {0.3, 0.7, 1.2});
    }
}

// sin*cos products of different arguments (product-to-sum) and exp*sin, exp*cos.
TEST(Integrate, TrigonometricProducts) {
    EXPECT_EQ(antiderivative("sin(2*x)*cos(3*x)"), "cos(x)/2 - cos(5*x)/10");
    EXPECT_EQ(antiderivative("sin(x)*sin(3*x)"), "sin(2*x)/4 - sin(4*x)/8");
    EXPECT_EQ(antiderivative("cos(x)*cos(2*x)"), "sin(x)/2 + sin(3*x)/6");
    EXPECT_EQ(antiderivative("exp(x)*sin(x)"), "exp(x)*(sin(x) - cos(x))/2");
    EXPECT_EQ(antiderivative("exp(2*x)*cos(3*x)"), "exp(2*x)*(2*cos(3*x) + 3*sin(3*x))/13");
    EXPECT_EQ(antiderivative("exp(x)*exp(2*x)"), "exp(3*x)/3");
}

TEST(Integrate, OtherVariables) {
    Expression f = parse("a*x^2 + y");
    EXPECT_EQ(integrate(f, "x").toString(), "a*x^3/3 + y*x");
    EXPECT_EQ(integrate(f, "y").toString(), "a*x^2*y + y^2/2");
    EXPECT_DOUBLE_EQ(integrate(f, "x", 0, 3, {{"a", 2}, {"y", 1}}), 21);
}

// No elementary antiderivative exists for these, or no rule covers them.
TEST(Integrate, Unsupported) {
    EXPECT_THROW(integrate(parse("exp(x^2)")), IntegrationError);
    EXPECT_THROW(integrate(parse("sin(x)/x")), IntegrationError);
    EXPECT_THROW(integrate(parse("1/(x^3 + 1)")), IntegrationError);  // denominators of degree > 2
}

TEST(DefiniteIntegral, Symbolic) {
    EXPECT_NEAR(integrate(parse("sin(x)"), "x", 0, std::acos(-1.0)), 2, 1e-12);
    EXPECT_NEAR(integrate(parse("1/x"), "x", 1, std::exp(1.0)), 1, 1e-12);
    EXPECT_NEAR(integrate(parse("x*exp(x)"), "x", 0, 1), 1, 1e-12);
    EXPECT_NEAR(integrate(parse("x^2"), "x", 3, 0), -9, 1e-12);  // reversed bounds
    const double pi = std::acos(-1.0);
    EXPECT_NEAR(integrate(parse("4/(1 + x^2)"), "x", 0, 1), pi, 1e-12);
    EXPECT_NEAR(integrate(parse("sqrt(1 - x^2)"), "x", -1, 1), pi / 2, 1e-12);  // half the unit disk
    EXPECT_NEAR(integrate(parse("sin(x)^2"), "x", 0, pi), pi / 2, 1e-12);
}

// When no antiderivative is found the definite integral is computed numerically.
TEST(DefiniteIntegral, NumericFallback) {
    const double pi = std::acos(-1.0);
    EXPECT_NEAR(integrate(parse("sin(x)/x"), "x", 1, 2), 0.659329906435512, 1e-9);  // Si(2) - Si(1)
    EXPECT_NEAR(integrate(parse("exp(-x^2)"), "x", -6, 6), std::sqrt(pi), 1e-9);  // Gaussian
    EXPECT_NEAR(numericIntegral(parse("x^2"), "x", 0, 3), 9, 1e-9);
    EXPECT_NEAR(numericIntegral(parse("sin(x)^2"), "x", 0, 2 * pi), pi, 1e-9);
}

TEST(Substitute, ReplacesVariable) {
    Expression f = parse("x^2 + y");
    EXPECT_EQ(substitute(f, "x", parse("t + 1")).toString(), "(t + 1)^2 + y");
    EXPECT_EQ(substitute(f, "x", Expression(3)).toString(), "y + 9");
}
