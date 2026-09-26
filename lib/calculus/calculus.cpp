#include "lib/calculus/calculus.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <functional>
#include <iomanip>
#include <numeric>
#include <optional>
#include <sstream>
#include <utility>
#include <vector>

namespace calculus {

struct Node {
    Op op = Op::Constant;
    double value = 0;
    std::string name;
    std::shared_ptr<const Node> a;
    std::shared_ptr<const Node> b;
};

namespace {

// Prefix of the placeholder variable used by integration by substitution; the
// recursion depth is appended so nested substitutions do not collide.
const char* const kSubstitutionVar = "__u";

// Guards the mutually recursive integration rules.
constexpr int kMaxIntegrationDepth = 64;

// Largest integer power of a sum that expand() multiplies out.
constexpr double kMaxExpandPower = 32;

// Largest degree read by polynomialCoefficients().
constexpr int kMaxPolynomialDegree = 32;

// Largest |n| for the reduction formulas of sin(x)^n and friends.
constexpr double kMaxReductionPower = 32;

// Most subexpressions tried as g in a substitution u = g(x).
constexpr size_t kMaxSubstitutionCandidates = 32;

bool isUnary(Op op) { return op >= Op::Neg; }

bool isValue(const Expression& e, double v) { return e.isConstant() && e.value() == v; }

bool isInteger(double v) { return std::isfinite(v) && v == std::floor(v); }

bool isNegativeConstant(const Expression& e) { return e.isConstant() && e.value() < 0; }

// c*r with a negative numeric c.
bool hasNegativeCoefficient(const Expression& e) {
    return e.op() == Op::Mul && isNegativeConstant(e.left());
}

bool isNegativePower(const Expression& e) { return e.op() == Op::Pow && isNegativeConstant(e.right()); }

// Splits e into numeric coefficient * rest, e.g. 3*x^2 -> (3, x^2), x/2 -> (0.5, x).
std::pair<double, Expression> splitCoefficient(const Expression& e) {
    switch (e.op()) {
        case Op::Constant:
            return {e.value(), Expression(1)};
        case Op::Mul:
            if (e.left().isConstant()) {
                return {e.left().value(), e.right()};
            }
            break;
        case Op::Neg: {
            auto inner = splitCoefficient(e.left());
            return {-inner.first, inner.second};
        }
        case Op::Div:
            if (e.right().isConstant() && e.right().value() != 0) {
                auto inner = splitCoefficient(e.left());
                return {inner.first / e.right().value(), inner.second};
            }
            break;
        default:
            break;
    }
    return {1, e};
}

// Splits e into base^exponent with a numeric exponent: x^3 -> (x, 3), 1/x -> (x, -1), x -> (x, 1).
std::pair<Expression, double> splitPower(const Expression& e) {
    if (e.op() == Op::Pow && e.right().isConstant()) {
        return {e.left(), e.right().value()};
    }
    if (e.op() == Op::Div && isValue(e.left(), 1)) {
        return {e.right(), -1};
    }
    return {e, 1};
}

struct FunctionInfo {
    Op op;
    const char* name;
    double (*evaluate)(double);
    Expression (*build)(const Expression&);
};

double absoluteValue(double v) { return std::fabs(v); }

const FunctionInfo kFunctionTable[] = {
    {Op::Sin, "sin", std::sin, sin},       {Op::Cos, "cos", std::cos, cos},
    {Op::Tan, "tan", std::tan, tan},       {Op::Exp, "exp", std::exp, exp},
    {Op::Log, "log", std::log, log},       {Op::Sqrt, "sqrt", std::sqrt, sqrt},
    {Op::Abs, "abs", absoluteValue, abs},  {Op::Asin, "asin", std::asin, asin},
    {Op::Acos, "acos", std::acos, acos},   {Op::Atan, "atan", std::atan, atan},
    {Op::Sinh, "sinh", std::sinh, sinh},   {Op::Cosh, "cosh", std::cosh, cosh},
    {Op::Tanh, "tanh", std::tanh, tanh},   {Op::Asinh, "asinh", std::asinh, asinh},
    {Op::Acosh, "acosh", std::acosh, acosh}, {Op::Atanh, "atanh", std::atanh, atanh},
};

// Only called for unary ops other than Neg.
const FunctionInfo& functionInfo(Op op) {
    for (const FunctionInfo& info : kFunctionTable) {
        if (info.op == op) {
            return info;
        }
    }
    throw std::logic_error("not a function");
}

Expression build(Op op, const Expression& a, const Expression& b) {
    switch (op) {
        case Op::Add:
            return a + b;
        case Op::Sub:
            return a - b;
        case Op::Mul:
            return a * b;
        case Op::Div:
            return a / b;
        case Op::Pow:
            return pow(a, b);
        case Op::Neg:
            return -a;
        case Op::Constant:
        case Op::Variable:
            return a;
        default:
            return functionInfo(op).build(a);
    }
}

double evaluateNode(const Node& n, const Variables& vars) {
    auto arg = [&](const std::shared_ptr<const Node>& child) { return evaluateNode(*child, vars); };
    switch (n.op) {
        case Op::Constant:
            return n.value;
        case Op::Variable: {
            auto it = vars.find(n.name);
            if (it == vars.end()) {
                throw EvaluationError("no value for variable '" + n.name + "'");
            }
            return it->second;
        }
        case Op::Add:
            return arg(n.a) + arg(n.b);
        case Op::Sub:
            return arg(n.a) - arg(n.b);
        case Op::Mul:
            return arg(n.a) * arg(n.b);
        case Op::Div:
            return arg(n.a) / arg(n.b);
        case Op::Pow:
            return std::pow(arg(n.a), arg(n.b));
        case Op::Neg:
            return -arg(n.a);
        default:
            return functionInfo(n.op).evaluate(arg(n.a));
    }
}

// ---------------------------------------------------------------------------
// Printing

std::string formatNumber(double v) {
    if (isInteger(v) && std::fabs(v) < 1e15) {
        return std::to_string(static_cast<long long>(v));
    }
    std::ostringstream os;
    os << std::setprecision(15) << v;
    return os.str();
}

// Binding strength of the printed form; higher binds tighter.
enum Precedence { kSum = 1, kProduct = 2, kUnaryMinus = 3, kPower = 4, kAtom = 5 };

struct Printed {
    std::string text;
    int precedence;
    bool negative;  // starts with a unary minus
};

Printed print(const Expression& e);

std::string wrap(const Printed& p, int minPrecedence, bool parenthesizeNegative) {
    if (p.precedence < minPrecedence || (parenthesizeNegative && p.negative)) {
        return "(" + p.text + ")";
    }
    return p.text;
}

// base^exponent where exponent > 0.
std::string printPower(const Expression& base, double exponent) {
    std::string text = wrap(print(base), kAtom, true);
    if (exponent != 1) {
        text += "^" + formatNumber(exponent);
    }
    return text;
}

Printed print(const Expression& e) {
    switch (e.op()) {
        case Op::Constant: {
            bool negative = e.value() < 0;
            return {formatNumber(e.value()), negative ? kUnaryMinus : kAtom, negative};
        }
        case Op::Variable:
            return {e.toString(), kAtom, false};
        case Op::Add:
            return {wrap(print(e.left()), kSum, false) + " + " + wrap(print(e.right()), kSum, true), kSum, false};
        case Op::Sub:
            return {wrap(print(e.left()), kSum, false) + " - " + wrap(print(e.right()), kProduct, true), kSum,
                    false};
        case Op::Mul: {
            Printed left = print(e.left());
            return {wrap(left, kProduct, false) + "*" + wrap(print(e.right()), kProduct, true), kProduct,
                    left.negative};
        }
        case Op::Div: {
            Printed left = print(e.left());
            return {wrap(left, kProduct, false) + "/" + wrap(print(e.right()), kUnaryMinus, true), kProduct,
                    left.negative};
        }
        case Op::Pow:
            if (isNegativePower(e)) {
                return {"1/" + printPower(e.left(), -e.right().value()), kProduct, false};
            }
            return {wrap(print(e.left()), kAtom, true) + "^" + wrap(print(e.right()), kPower, true), kPower, false};
        case Op::Neg:
            return {"-" + wrap(print(e.left()), kProduct, true), kUnaryMinus, true};
        default:
            return {std::string(functionInfo(e.op()).name) + "(" + print(e.left()).text + ")", kAtom, false};
    }
}

// ---------------------------------------------------------------------------
// Parsing

class Parser {
public:
    explicit Parser(const std::string& text) : text_(text) {}

    Expression parse() {
        Expression result = parseSum();
        skipSpaces();
        if (pos_ != text_.size()) {
            fail("unexpected '" + std::string(1, text_[pos_]) + "'");
        }
        return result;
    }

private:
    Expression parseSum() {
        Expression result = parseProduct();
        while (true) {
            if (accept('+')) {
                result = result + parseProduct();
            } else if (accept('-')) {
                result = result - parseProduct();
            } else {
                return result;
            }
        }
    }

    Expression parseProduct() {
        Expression result = parseUnary();
        while (true) {
            if (accept('*')) {
                result = result * parseUnary();
            } else if (accept('/')) {
                result = result / parseUnary();
            } else {
                return result;
            }
        }
    }

    Expression parseUnary() {
        if (accept('-')) {
            return -parseUnary();
        }
        if (accept('+')) {
            return parseUnary();
        }
        return parsePower();
    }

    Expression parsePower() {
        Expression base = parsePrimary();
        if (accept('^')) {
            return pow(base, parseUnary());
        }
        return base;
    }

    Expression parsePrimary() {
        skipSpaces();
        if (pos_ == text_.size()) {
            fail("unexpected end of formula");
        }
        char c = text_[pos_];
        if (accept('(')) {
            Expression inner = parseSum();
            expect(')');
            return inner;
        }
        if (std::isdigit(static_cast<unsigned char>(c)) || c == '.') {
            const char* begin = text_.c_str() + pos_;
            char* end = nullptr;
            double value = std::strtod(begin, &end);
            if (end == begin) {
                fail("bad number");
            }
            pos_ += end - begin;
            return Expression(value);
        }
        if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
            size_t start = pos_;
            while (pos_ < text_.size() &&
                   (std::isalnum(static_cast<unsigned char>(text_[pos_])) || text_[pos_] == '_')) {
                ++pos_;
            }
            std::string name = text_.substr(start, pos_ - start);
            if (accept('(')) {
                Expression arg = parseSum();
                expect(')');
                return applyFunction(name, arg, start);
            }
            if (name == "pi") {
                return Expression(std::acos(-1.0));
            }
            if (name == "e") {
                return Expression(std::exp(1.0));
            }
            return Expression::variable(name);
        }
        fail("unexpected '" + std::string(1, c) + "'");
        return Expression();
    }

    Expression applyFunction(const std::string& name, const Expression& arg, size_t at) {
        for (const FunctionInfo& info : kFunctionTable) {
            if (name == info.name) {
                return info.build(arg);
            }
        }
        static const std::map<std::string, std::function<Expression(const Expression&)>> kAliases = {
            {"ln", [](const Expression& u) { return log(u); }},
            {"arcsin", [](const Expression& u) { return asin(u); }},
            {"arccos", [](const Expression& u) { return acos(u); }},
            {"arctan", [](const Expression& u) { return atan(u); }},
            {"sec", [](const Expression& u) { return 1 / cos(u); }},
            {"csc", [](const Expression& u) { return 1 / sin(u); }},
            {"cot", [](const Expression& u) { return 1 / tan(u); }},
            {"log10", [](const Expression& u) { return log(u) / std::log(10.0); }},
            {"log2", [](const Expression& u) { return log(u) / std::log(2.0); }},
        };
        auto it = kAliases.find(name);
        if (it == kAliases.end()) {
            pos_ = at;
            fail("unknown function '" + name + "'");
        }
        return it->second(arg);
    }

    void skipSpaces() {
        while (pos_ < text_.size() && std::isspace(static_cast<unsigned char>(text_[pos_]))) {
            ++pos_;
        }
    }

    bool accept(char c) {
        skipSpaces();
        if (pos_ < text_.size() && text_[pos_] == c) {
            ++pos_;
            return true;
        }
        return false;
    }

    void expect(char c) {
        if (!accept(c)) {
            fail(std::string("expected '") + c + "'");
        }
    }

    [[noreturn]] void fail(const std::string& message) const {
        throw ParseError(message + " at position " + std::to_string(pos_) + " in \"" + text_ + "\"");
    }

    const std::string& text_;
    size_t pos_ = 0;
};

// ---------------------------------------------------------------------------
// Expansion

void collectTerms(const Expression& e, bool negate, std::vector<Expression>& out) {
    switch (e.op()) {
        case Op::Add:
            collectTerms(e.left(), negate, out);
            collectTerms(e.right(), negate, out);
            return;
        case Op::Sub:
            collectTerms(e.left(), negate, out);
            collectTerms(e.right(), !negate, out);
            return;
        case Op::Neg:
            collectTerms(e.left(), !negate, out);
            return;
        default:
            out.push_back(negate ? -e : e);
    }
}

std::vector<Expression> termsOf(const Expression& e) {
    std::vector<Expression> terms;
    collectTerms(e, false, terms);
    return terms;
}

// Sums terms, merging the ones that differ only by a numeric coefficient.
Expression sumOf(const std::vector<Expression>& terms) {
    struct Like {
        Expression original;
        double coefficient;
        Expression rest;
        bool merged;
    };
    std::vector<Like> likes;
    for (const Expression& term : terms) {
        auto [coefficient, rest] = splitCoefficient(term);
        bool found = false;
        for (Like& like : likes) {
            if (like.rest == rest) {
                like.coefficient += coefficient;
                like.merged = true;
                found = true;
                break;
            }
        }
        if (!found) {
            likes.push_back({term, coefficient, rest, false});
        }
    }
    Expression result(0);
    for (const Like& like : likes) {
        result = result + (like.merged ? Expression(like.coefficient) * like.rest : like.original);
    }
    return result;
}

std::vector<Expression> multiplyTerms(const std::vector<Expression>& a, const std::vector<Expression>& b) {
    std::vector<Expression> result;
    for (const Expression& x : a) {
        for (const Expression& y : b) {
            collectTerms(x * y, false, result);
        }
    }
    return result;
}

// ---------------------------------------------------------------------------
// Integration

// Returns d u / d var when u is linear in var (a*var + b with a, b free of var).
std::optional<Expression> linearSlope(const Expression& u, const std::string& var) {
    Expression slope = differentiate(u, var);
    if (slope.isFreeOf(var) && !isValue(slope, 0)) {
        return slope;
    }
    return std::nullopt;
}

// A product split into the part free of var and the remaining factors.
struct Factors {
    Expression coefficient{1};
    std::vector<Expression> factors;
};

Factors factorsOf(const Expression& e, const std::string& var);

void collectFactors(const Expression& e, const std::string& var, Factors& out) {
    if (e.isFreeOf(var)) {
        out.coefficient = out.coefficient * e;
        return;
    }
    switch (e.op()) {
        case Op::Mul:
            collectFactors(e.left(), var, out);
            collectFactors(e.right(), var, out);
            return;
        case Op::Neg:
            out.coefficient = -out.coefficient;
            collectFactors(e.left(), var, out);
            return;
        case Op::Div: {
            collectFactors(e.left(), var, out);
            Factors den = factorsOf(e.right(), var);
            out.coefficient = out.coefficient / den.coefficient;
            for (const Expression& factor : den.factors) {
                collectFactors(pow(factor, -1), var, out);
            }
            return;
        }
        case Op::Pow:
            // (a*b)^n = a^n * b^n for integer n.
            if (e.left().op() == Op::Mul && e.right().isConstant() && isInteger(e.right().value())) {
                collectFactors(pow(e.left().left(), e.right()), var, out);
                collectFactors(pow(e.left().right(), e.right()), var, out);
                return;
            }
            out.factors.push_back(e);
            return;
        default:
            out.factors.push_back(e);
    }
}

Factors factorsOf(const Expression& e, const std::string& var) {
    Factors result;
    collectFactors(e, var, result);
    return result;
}

Expression productOf(const std::vector<Expression>& factors) {
    Expression result(1);
    for (const Expression& f : factors) {
        result = result * f;
    }
    return result;
}

Expression integrateImpl(const Expression& e, const std::string& var, int depth);

// Removes the factors of `part` from `whole`; nullopt unless all of them are present.
std::optional<std::vector<Expression>> removeFactors(std::vector<Expression> whole,
                                                     const std::vector<Expression>& part) {
    for (const Expression& factor : part) {
        auto it = std::find(whole.begin(), whole.end(), factor);
        if (it == whole.end()) {
            return std::nullopt;
        }
        whole.erase(it);
    }
    return whole;
}

bool isPolynomial(const Expression& e, const std::string& var) {
    if (e.isFreeOf(var)) {
        return true;
    }
    switch (e.op()) {
        case Op::Variable:
            return true;
        case Op::Add:
        case Op::Sub:
        case Op::Mul:
            return isPolynomial(e.left(), var) && isPolynomial(e.right(), var);
        case Op::Neg:
            return isPolynomial(e.left(), var);
        case Op::Div:
            return isPolynomial(e.left(), var) && e.right().isFreeOf(var);
        case Op::Pow:
            return e.right().isConstant() && isInteger(e.right().value()) && e.right().value() >= 0 &&
                   isPolynomial(e.left(), var);
        default:
            return false;
    }
}

// Numeric coefficients c[0] + c[1]*x + c[2]*x^2 + ... of a polynomial, from
// its Taylor series at 0. nullopt when e is not a polynomial with numeric
// coefficients.
std::optional<std::vector<double>> polynomialCoefficients(const Expression& e, const std::string& var) {
    if (!isPolynomial(e, var)) {
        return std::nullopt;
    }
    std::vector<double> result;
    Expression derivative = e;
    double factorial = 1;
    for (int k = 0; !isValue(derivative, 0); ++k) {
        if (k > kMaxPolynomialDegree) {
            return std::nullopt;
        }
        Expression atZero = substitute(derivative, var, Expression(0));
        if (!atZero.isConstant()) {
            return std::nullopt;
        }
        if (k > 0) {
            factorial *= k;
        }
        result.push_back(atZero.value() / factorial);
        derivative = differentiate(derivative, var);
    }
    while (!result.empty() && result.back() == 0) {
        result.pop_back();
    }
    return result;
}

// c * term, written as n*term/d when c is close to a fraction n/d.
Expression scaled(double c, const Expression& term) {
    constexpr int kMaxDenominator = 64;
    for (int d = 1; d <= kMaxDenominator; ++d) {
        double n = std::round(c * d);
        if (std::fabs(c * d - n) <= 1e-12 * std::max(1.0, std::fabs(n))) {
            return n * term / d;
        }
    }
    return c * term;
}

// p*x^2 + r*x + s written as p*((x + h)^2 + k).
struct Quadratic {
    double p;
    double h;
    double k;
};

std::optional<Quadratic> quadraticOf(const Expression& q, const std::string& var) {
    auto c = polynomialCoefficients(q, var);
    if (!c || c->size() != 3) {
        return std::nullopt;
    }
    double p = (*c)[2];
    double h = (*c)[1] / (2 * p);
    double k = (*c)[0] / p - h * h;
    if (std::fabs(k) < 1e-12 * std::max(1.0, h * h)) {
        k = 0;
    }
    return Quadratic{p, h, k};
}

// ∫ 1/q dx with u = x + h.
Expression integrateReciprocalQuadratic(const Quadratic& q, const Expression& u) {
    if (q.k > 0) {
        double m = std::sqrt(q.k);
        return atan(u / m) / (q.p * m);
    }
    if (q.k < 0) {
        double m = std::sqrt(-q.k);
        return log(abs((u - m) / (u + m))) / (2 * m * q.p);
    }
    return -1 / (q.p * u);
}

// ∫ num/den dx for polynomials with numeric coefficients and den of degree 1
// or 2: polynomial division, then log and atan for the remainder.
std::optional<Expression> integrateRational(const Expression& num, const Expression& den, const std::string& var) {
    auto n = polynomialCoefficients(num, var);
    auto d = polynomialCoefficients(den, var);
    if (!n || !d || d->size() < 2 || d->size() > 3) {
        return std::nullopt;
    }
    const size_t degree = d->size() - 1;
    std::vector<double> remainder = *n;
    std::vector<double> quotient(remainder.size() > degree ? remainder.size() - degree : 0, 0.0);
    for (size_t i = quotient.size(); i-- > 0;) {
        quotient[i] = remainder[i + degree] / d->back();
        for (size_t j = 0; j <= degree; ++j) {
            remainder[i + j] -= quotient[i] * (*d)[j];
        }
    }
    remainder.resize(std::min(remainder.size(), degree));
    // remainder = a*x + b
    const double a = remainder.size() > 1 ? remainder[1] : 0;
    const double b = remainder.empty() ? 0 : remainder[0];

    // ∫ quotient, term by term.
    Expression result(0);
    for (size_t k = quotient.size(); k-- > 0;) {
        result = result + scaled(quotient[k] / (k + 1), pow(Expression::variable(var), static_cast<double>(k + 1)));
    }
    if (degree == 1) {
        return result + b * log(abs(den)) / (*d)[1];
    }
    auto q = quadraticOf(den, var);
    // a*x + b = a/(2p) * den' + (b - a*h)
    if (a != 0) {
        result = result + a * log(abs(den)) / (2 * q->p);
    }
    if (b - a * q->h != 0) {
        result = result + (b - a * q->h) * integrateReciprocalQuadratic(*q, Expression::variable(var) + q->h);
    }
    return result;
}

// ∫ 1/sqrt(q) dx with u = x + h.
std::optional<Expression> integrateReciprocalSqrtQuadratic(const Quadratic& q, const Expression& u,
                                                           const Expression& sqrtQ) {
    if (q.p > 0) {
        double sp = std::sqrt(q.p);
        if (q.k > 0) {
            return asinh(u / std::sqrt(q.k)) / sp;
        }
        if (q.k < 0) {
            return log(abs(u + sqrtQ / sp)) / sp;
        }
        return std::nullopt;
    }
    if (q.k < 0) {
        return asin(u / std::sqrt(-q.k)) / std::sqrt(-q.p);
    }
    return std::nullopt;
}

// ∫ sqrt(q) dx with u = x + h.
std::optional<Expression> integrateSqrtQuadratic(const Quadratic& q, const Expression& u, const Expression& sqrtQ) {
    if (q.p > 0) {
        double sp = std::sqrt(q.p);
        if (q.k > 0) {
            return u * sqrtQ / 2 + sp * q.k * asinh(u / std::sqrt(q.k)) / 2;
        }
        if (q.k < 0) {
            return u * sqrtQ / 2 + sp * q.k * log(abs(u + sqrtQ / sp)) / 2;
        }
        return std::nullopt;
    }
    if (q.k < 0) {
        return u * sqrtQ / 2 + std::sqrt(-q.p) * -q.k * asin(u / std::sqrt(-q.k)) / 2;
    }
    return std::nullopt;
}

// ∫ (a*x + b)/sqrt(q) dx = a/p * sqrt(q) + (b - a*h) * ∫ 1/sqrt(q) dx.
std::optional<Expression> integrateOverSqrtQuadratic(const Expression& num, const Expression& qExpr,
                                                     const std::string& var) {
    auto q = quadraticOf(qExpr, var);
    auto n = polynomialCoefficients(num, var);
    if (!q || !n || n->size() > 2) {
        return std::nullopt;
    }
    const double a = n->size() > 1 ? (*n)[1] : 0;
    const double b = n->empty() ? 0 : (*n)[0];
    const Expression sqrtQ = sqrt(qExpr);
    Expression result = a * sqrtQ / q->p;
    if (b - a * q->h != 0) {
        auto inverse = integrateReciprocalSqrtQuadratic(*q, Expression::variable(var) + q->h, sqrtQ);
        if (!inverse) {
            return std::nullopt;
        }
        result = result + (b - a * q->h) * *inverse;
    }
    return result;
}

// q for sqrt(q) and q^0.5.
std::optional<Expression> sqrtArgument(const Expression& e) {
    if (e.op() == Op::Sqrt) {
        return e.left();
    }
    if (e.op() == Op::Pow && isValue(e.right(), 0.5)) {
        return e.left();
    }
    return std::nullopt;
}

// sqrt(q), q^-0.5 and q^-1 for a quadratic q.
std::optional<Expression> integrateQuadraticPower(const Expression& e, const std::string& var) {
    if (auto qExpr = sqrtArgument(e)) {
        if (auto q = quadraticOf(*qExpr, var)) {
            return integrateSqrtQuadratic(*q, Expression::variable(var) + q->h, sqrt(*qExpr));
        }
        return std::nullopt;
    }
    if (e.op() != Op::Pow) {
        return std::nullopt;
    }
    if (isValue(e.right(), -0.5)) {
        return integrateOverSqrtQuadratic(Expression(1), e.left(), var);
    }
    if (isValue(e.right(), -1)) {
        return integrateRational(Expression(1), e.left(), var);
    }
    return std::nullopt;
}

// ∫ f(u)^n dx for f one of sin, cos, tan, sinh, cosh, tanh, u = a*x + b and
// an integer n, by the usual reduction formulas.
std::optional<Expression> integrateFunctionPower(const Expression& e, const std::string& var, int depth) {
    const Expression base = e.left();
    const Op f = base.op();
    if (!e.right().isConstant() || !isInteger(e.right().value()) || std::fabs(e.right().value()) > kMaxReductionPower ||
        (f != Op::Sin && f != Op::Cos && f != Op::Tan && f != Op::Sinh && f != Op::Cosh && f != Op::Tanh)) {
        return std::nullopt;
    }
    const Expression u = base.left();
    const auto k = linearSlope(u, var);
    if (!k) {
        return std::nullopt;
    }
    const double n = e.right().value();
    const double m = -n;
    auto power = [&](double exponent) { return pow(base, exponent); };
    auto rest = [&](double exponent) { return integrateImpl(power(exponent), var, depth + 1); };
    switch (f) {
        case Op::Sin:
            if (n >= 2) {
                return -(power(n - 1) * cos(u)) / (n * *k) + (n - 1) * rest(n - 2) / n;
            }
            if (n == -1) {
                return log(abs(tan(u / 2))) / *k;
            }
            if (n == -2) {
                return -(cos(u) / sin(u)) / *k;
            }
            // ∫ csc^m = -csc^(m-2) cot / (m-1) + (m-2)/(m-1) ∫ csc^(m-2)
            return -(cos(u) * power(1 - m)) / ((m - 1) * *k) + (m - 2) * rest(2 - m) / (m - 1);
        case Op::Cos:
            if (n >= 2) {
                return power(n - 1) * sin(u) / (n * *k) + (n - 1) * rest(n - 2) / n;
            }
            if (n == -1) {
                return log(abs(1 / cos(u) + tan(u))) / *k;
            }
            if (n == -2) {
                return tan(u) / *k;
            }
            // ∫ sec^m = sec^(m-2) tan / (m-1) + (m-2)/(m-1) ∫ sec^(m-2)
            return sin(u) * power(1 - m) / ((m - 1) * *k) + (m - 2) * rest(2 - m) / (m - 1);
        case Op::Tan:
            if (n >= 2) {
                return power(n - 1) / ((n - 1) * *k) - rest(n - 2);
            }
            if (n == -1) {
                return log(abs(sin(u))) / *k;
            }
            // ∫ cot^m = -cot^(m-1) / (m-1) - ∫ cot^(m-2)
            return -power(1 - m) / ((m - 1) * *k) - rest(2 - m);
        case Op::Sinh:
            if (n >= 2) {
                return power(n - 1) * cosh(u) / (n * *k) - (n - 1) * rest(n - 2) / n;
            }
            if (n == -2) {
                return -(cosh(u) / sinh(u)) / *k;
            }
            return std::nullopt;
        case Op::Cosh:
            if (n >= 2) {
                return power(n - 1) * sinh(u) / (n * *k) + (n - 1) * rest(n - 2) / n;
            }
            if (n == -2) {
                return tanh(u) / *k;
            }
            return std::nullopt;
        case Op::Tanh:
            if (n >= 2) {
                return -power(n - 1) / ((n - 1) * *k) + rest(n - 2);
            }
            if (n == -1) {
                return log(abs(sinh(u))) / *k;
            }
            return std::nullopt;
        default:
            return std::nullopt;
    }
}

// Antiderivatives of f(a*x + b) for the elementary functions: F(u) / a.
std::optional<Expression> integrateTable(const Expression& e, const std::string& var, int depth) {
    if (e.op() == Op::Pow) {
        if (auto result = integrateFunctionPower(e, var, depth)) {
            return result;
        }
        const Expression& base = e.left();
        const Expression& exponent = e.right();
        if (exponent.isFreeOf(var)) {
            if (auto k = linearSlope(base, var)) {
                if (isValue(exponent, -1)) {
                    return log(abs(base)) / *k;
                }
                return pow(base, exponent + 1) / ((exponent + 1) * *k);
            }
        } else if (base.isFreeOf(var)) {
            if (auto k = linearSlope(exponent, var)) {
                return e / (log(base) * *k);
            }
        }
        return std::nullopt;
    }
    if (!isUnary(e.op()) || e.op() == Op::Neg) {
        return std::nullopt;
    }
    const Expression& u = e.left();
    auto k = linearSlope(u, var);
    if (!k) {
        return std::nullopt;
    }
    switch (e.op()) {
        case Op::Sin:
            return -cos(u) / *k;
        case Op::Cos:
            return sin(u) / *k;
        case Op::Tan:
            return -log(abs(cos(u))) / *k;
        case Op::Exp:
            return exp(u) / *k;
        case Op::Log:
            return (u * log(u) - u) / *k;
        case Op::Sqrt:
            return 2 * pow(u, 1.5) / (3 * *k);
        case Op::Abs:
            return u * abs(u) / (2 * *k);
        case Op::Asin:
            return (u * asin(u) + sqrt(1 - pow(u, 2))) / *k;
        case Op::Acos:
            return (u * acos(u) - sqrt(1 - pow(u, 2))) / *k;
        case Op::Atan:
            return (u * atan(u) - log(pow(u, 2) + 1) / 2) / *k;
        case Op::Sinh:
            return cosh(u) / *k;
        case Op::Cosh:
            return sinh(u) / *k;
        case Op::Tanh:
            return log(cosh(u)) / *k;
        case Op::Asinh:
            return (u * asinh(u) - sqrt(pow(u, 2) + 1)) / *k;
        case Op::Acosh:
            return (u * acosh(u) - sqrt(pow(u, 2) - 1)) / *k;
        case Op::Atanh:
            return (u * atanh(u) + log(1 - pow(u, 2)) / 2) / *k;
        default:
            return std::nullopt;
    }
}

// Subexpressions of e that depend on var non-linearly: the candidates for g
// in a substitution u = g(x).
void collectSubexpressions(const Expression& e, const std::string& var, std::vector<Expression>& out) {
    if (e.isFreeOf(var) || e.op() == Op::Variable || out.size() >= kMaxSubstitutionCandidates) {
        return;
    }
    if (!linearSlope(e, var) && std::find(out.begin(), out.end(), e) == out.end()) {
        out.push_back(e);
    }
    collectSubexpressions(e.left(), var, out);
    if (!isUnary(e.op())) {
        collectSubexpressions(e.right(), var, out);
    }
}

// Replaces every occurrence of the subexpression `from` with `to`.
Expression replace(const Expression& e, const Expression& from, const Expression& to, const std::string& var) {
    if (e == from) {
        return to;
    }
    // v^b = (v^a)^(b/a)
    if (from.op() == Op::Pow && e.op() == Op::Pow && from.right().isConstant() && e.right().isConstant() &&
        e.left() == from.left()) {
        double ratio = e.right().value() / from.right().value();
        if (isInteger(ratio)) {
            return pow(to, ratio);
        }
    }
    // exp(r*v) = exp(v)^r
    if (from.op() == Op::Exp && e.op() == Op::Exp) {
        auto kFrom = linearSlope(from.left(), var);
        auto kE = linearSlope(e.left(), var);
        if (kFrom && kE) {
            Expression ratio = *kE / *kFrom;
            if (ratio.isConstant() && isInteger(ratio.value()) && ratio * from.left() == e.left()) {
                return pow(to, ratio);
            }
        }
    }
    if (e.op() == Op::Constant || e.op() == Op::Variable) {
        return e;
    }
    return build(e.op(), replace(e.left(), from, to, var),
                 isUnary(e.op()) ? Expression() : replace(e.right(), from, to, var));
}

// ∫ h(g(x)) * c*g'(x) dx = c * H(g(x)), where H is an antiderivative of h:
// for each candidate g, divide g' out of the integrand and check that what is
// left depends on x only through g.
std::optional<Expression> integrateBySubstitution(const Expression& e, const std::string& var, int depth) {
    const std::string uName = kSubstitutionVar + std::to_string(depth);
    const Expression u = Expression::variable(uName);
    Factors f = factorsOf(e, var);
    std::vector<Expression> candidates;
    const Expression x = Expression::variable(var);
    for (const Expression& factor : f.factors) {
        collectSubexpressions(factor, var, candidates);
        // A factor x^m is the derivative of g = x^(m+1), up to a constant.
        auto [base, power] = splitPower(factor);
        if (base == x && power != -1) {
            Expression g = pow(x, power + 1);
            if (std::find(candidates.begin(), candidates.end(), g) == candidates.end()) {
                candidates.push_back(g);
            }
        }
    }
    for (const Expression& inner : candidates) {
        Factors derivative = factorsOf(differentiate(inner, var), var);
        auto rest = removeFactors(f.factors, derivative.factors);
        if (!rest) {
            continue;
        }
        Expression outer(1);
        for (const Expression& factor : *rest) {
            outer = outer * replace(factor, inner, u, var);
        }
        if (!outer.isFreeOf(var)) {
            continue;
        }
        try {
            Expression antiderivative = integrateImpl(outer, uName, depth + 1);
            return f.coefficient * substitute(antiderivative, uName, inner) / derivative.coefficient;
        } catch (const IntegrationError&) {
        }
    }
    return std::nullopt;
}

// sin*sin, sin*cos, cos*cos, exp*sin, exp*cos and exp*exp of linear arguments.
std::optional<Expression> integrateSpecialProduct(const Expression& e, const std::string& var, int depth) {
    Factors f = factorsOf(e, var);
    if (f.factors.size() != 2) {
        return std::nullopt;
    }
    auto rank = [](Op op) { return op == Op::Exp ? 0 : op == Op::Sin ? 1 : op == Op::Cos ? 2 : 3; };
    Expression a = f.factors[0];
    Expression b = f.factors[1];
    if (rank(b.op()) < rank(a.op())) {
        std::swap(a, b);
    }
    if (rank(a.op()) == 3 || rank(b.op()) == 3) {
        return std::nullopt;
    }
    const Expression u = a.left();
    const Expression v = b.left();
    const auto ku = linearSlope(u, var);
    const auto kv = linearSlope(v, var);
    if (!ku || !kv) {
        return std::nullopt;
    }
    Expression rewritten;
    if (a.op() == Op::Exp) {
        if (b.op() == Op::Exp) {
            rewritten = exp(u + v);
        } else {
            // ∫ e^u sin v = e^u (ku sin v - kv cos v) / (ku^2 + kv^2), same for cos.
            Expression norm = pow(*ku, 2) + pow(*kv, 2);
            if (b.op() == Op::Sin) {
                return f.coefficient * a * (*ku * sin(v) - *kv * cos(v)) / norm;
            }
            return f.coefficient * a * (*ku * cos(v) + *kv * sin(v)) / norm;
        }
    } else if (a.op() == Op::Sin && b.op() == Op::Sin) {
        rewritten = cos(u - v) / 2 - cos(u + v) / 2;
    } else if (a.op() == Op::Cos) {
        rewritten = cos(u - v) / 2 + cos(u + v) / 2;
    } else {
        rewritten = sin(u + v) / 2 + sin(u - v) / 2;
    }
    return f.coefficient * integrateImpl(rewritten, var, depth + 1);
}

enum class PartsRole { None, Integrate, Differentiate };

// How t is treated in ∫ p(x) t(x) dx for a polynomial p.
PartsRole partsRole(const Expression& t, const std::string& var) {
    switch (t.op()) {
        case Op::Exp:
        case Op::Sin:
        case Op::Cos:
        case Op::Sinh:
        case Op::Cosh:
            return linearSlope(t.left(), var) ? PartsRole::Integrate : PartsRole::None;
        case Op::Pow:
            return t.left().isFreeOf(var) && linearSlope(t.right(), var) ? PartsRole::Integrate : PartsRole::None;
        case Op::Log:
        case Op::Asin:
        case Op::Acos:
        case Op::Atan:
        case Op::Asinh:
        case Op::Acosh:
        case Op::Atanh:
            return linearSlope(t.left(), var) ? PartsRole::Differentiate : PartsRole::None;
        default:
            return PartsRole::None;
    }
}

// ∫ p(x) * t(x) dx for a polynomial p and t one of exp, sin, cos, sinh, cosh,
// c^x (integrated) or log, atan, asin, ... (differentiated) of a linear argument.
std::optional<Expression> integrateByParts(const Expression& e, const std::string& var, int depth) {
    Factors f = factorsOf(e, var);
    std::optional<size_t> target;
    std::vector<Expression> polynomial;
    for (size_t i = 0; i < f.factors.size(); ++i) {
        if (!target && partsRole(f.factors[i], var) != PartsRole::None) {
            target = i;
        } else if (isPolynomial(f.factors[i], var)) {
            polynomial.push_back(f.factors[i]);
        } else {
            return std::nullopt;
        }
    }
    if (!target || polynomial.empty()) {
        return std::nullopt;
    }
    Expression p = f.coefficient * productOf(polynomial);
    const Expression& t = f.factors[*target];
    if (partsRole(t, var) == PartsRole::Differentiate) {
        Expression antiP = integrateImpl(p, var, depth + 1);
        return antiP * t - integrateImpl(antiP * differentiate(t, var), var, depth + 1);
    }
    // Differentiate the polynomial; its degree drops on every step.
    Expression antiT = integrateImpl(t, var, depth + 1);
    return p * antiT - integrateImpl(differentiate(p, var) * antiT, var, depth + 1);
}

std::optional<Expression> integrateQuotient(const Expression& e, const std::string& var, int depth) {
    const Expression num = e.left();
    const Expression den = e.right();
    if (num.isFreeOf(var)) {
        if (auto k = linearSlope(den, var)) {
            return num * log(abs(den)) / *k;
        }
        if (!num.isConstant()) {
            try {
                return num * integrateImpl(1 / den, var, depth + 1);
            } catch (const IntegrationError&) {
                return std::nullopt;
            }
        }
        // 1/cos(x), 1/sin(x)^2, ...
        Expression inverse = pow(den, -1);
        if (inverse.op() == Op::Pow) {
            if (auto result = integrateFunctionPower(inverse, var, depth)) {
                return num * *result;
            }
        }
    }
    if (auto result = integrateRational(num, den, var)) {
        return result;
    }
    if (auto q = sqrtArgument(den)) {
        return integrateOverSqrtQuadratic(num, *q, var);
    }
    return std::nullopt;
}

Expression integrateImpl(const Expression& e, const std::string& var, int depth) {
    if (depth > kMaxIntegrationDepth) {
        throw IntegrationError("integration is too deep for " + e.toString());
    }
    if (e.isFreeOf(var)) {
        return e * Expression::variable(var);
    }
    const int next = depth + 1;
    switch (e.op()) {
        case Op::Variable:
            return pow(e, 2) / 2;
        case Op::Add:
            return integrateImpl(e.left(), var, next) + integrateImpl(e.right(), var, next);
        case Op::Sub:
            return integrateImpl(e.left(), var, next) - integrateImpl(e.right(), var, next);
        case Op::Neg:
            return -integrateImpl(e.left(), var, next);
        case Op::Mul:
            if (e.left().isFreeOf(var)) {
                return e.left() * integrateImpl(e.right(), var, next);
            }
            if (e.right().isFreeOf(var)) {
                return integrateImpl(e.left(), var, next) * e.right();
            }
            break;
        case Op::Div:
            if (e.right().isFreeOf(var)) {
                return integrateImpl(e.left(), var, next) / e.right();
            }
            if (auto result = integrateQuotient(e, var, next)) {
                return *result;
            }
            break;
        default:
            if (auto result = integrateTable(e, var, next)) {
                return *result;
            }
            if (auto result = integrateQuadraticPower(e, var)) {
                return *result;
            }
    }
    if (auto result = integrateBySubstitution(e, var, next)) {
        return *result;
    }
    if (auto result = integrateSpecialProduct(e, var, next)) {
        return *result;
    }
    try {
        if (auto result = integrateByParts(e, var, next)) {
            return *result;
        }
    } catch (const IntegrationError&) {
    }
    Expression expanded = expand(e);
    if (expanded != e) {
        return integrateImpl(expanded, var, next);
    }
    throw IntegrationError("cannot integrate " + e.toString() + " with respect to " + var);
}

double adaptiveSimpson(const std::function<double(double)>& f, double a, double b, double fa, double fm, double fb,
                       double whole, double tolerance, int depth) {
    double m = (a + b) / 2;
    double lm = (a + m) / 2;
    double rm = (m + b) / 2;
    double flm = f(lm);
    double frm = f(rm);
    double left = (m - a) / 6 * (fa + 4 * flm + fm);
    double right = (b - m) / 6 * (fm + 4 * frm + fb);
    double delta = left + right - whole;
    if (depth <= 0 || std::fabs(delta) <= 15 * tolerance) {
        return left + right + delta / 15;
    }
    return adaptiveSimpson(f, a, m, fa, flm, fm, left, tolerance / 2, depth - 1) +
           adaptiveSimpson(f, m, b, fm, frm, fb, right, tolerance / 2, depth - 1);
}

}  // namespace

// ---------------------------------------------------------------------------
// Expression

Expression::Expression(double value) {
    auto node = std::make_shared<Node>();
    node->op = Op::Constant;
    node->value = value;
    node_ = std::move(node);
}

Expression::Expression(std::shared_ptr<const Node> node) : node_(std::move(node)) {}

Expression Expression::variable(const std::string& name) {
    auto node = std::make_shared<Node>();
    node->op = Op::Variable;
    node->name = name;
    return Expression(std::shared_ptr<const Node>(std::move(node)));
}

Expression Expression::make(Op op, Expression a, Expression b) {
    auto node = std::make_shared<Node>();
    node->op = op;
    node->a = a.node_;
    if (!isUnary(op)) {
        node->b = b.node_;
    }
    return Expression(std::shared_ptr<const Node>(std::move(node)));
}

double Expression::evaluate(const Variables& vars) const { return evaluateNode(*node_, vars); }

double Expression::evaluate(const std::string& var, double value) const { return evaluate(Variables{{var, value}}); }

std::string Expression::toString() const {
    if (node_->op == Op::Variable) {
        return node_->name;
    }
    return print(*this).text;
}

bool Expression::isFreeOf(const std::string& var) const {
    switch (node_->op) {
        case Op::Constant:
            return true;
        case Op::Variable:
            return node_->name != var;
        default:
            return left().isFreeOf(var) && (isUnary(node_->op) || right().isFreeOf(var));
    }
}

bool Expression::isConstant() const { return node_->op == Op::Constant; }

double Expression::value() const { return node_->value; }

Op Expression::op() const { return node_->op; }

Expression Expression::left() const {
    if (!node_->a) {
        throw std::logic_error("expression has no operands");
    }
    return Expression(node_->a);
}

Expression Expression::right() const {
    if (!node_->b) {
        throw std::logic_error("expression has no second operand");
    }
    return Expression(node_->b);
}

bool Expression::operator==(const Expression& other) const {
    if (node_ == other.node_) {
        return true;
    }
    const Node& x = *node_;
    const Node& y = *other.node_;
    if (x.op != y.op) {
        return false;
    }
    switch (x.op) {
        case Op::Constant:
            return x.value == y.value;
        case Op::Variable:
            return x.name == y.name;
        default:
            return left() == other.left() && (isUnary(x.op) || right() == other.right());
    }
}

// ---------------------------------------------------------------------------
// Simplifying constructors

Expression operator+(const Expression& a, const Expression& b) {
    if (a.isConstant() && b.isConstant()) {
        return Expression(a.value() + b.value());
    }
    if (isValue(a, 0)) {
        return b;
    }
    if (isValue(b, 0)) {
        return a;
    }
    if (isNegativeConstant(b)) {
        return a - Expression(-b.value());
    }
    if (b.op() == Op::Neg) {
        return a - b.left();
    }
    if (hasNegativeCoefficient(b)) {
        return a - Expression(-b.left().value()) * b.right();
    }
    if (a.isConstant()) {
        return b + a;
    }
    if (a.op() == Op::Neg) {
        return b - a.left();
    }
    if (b.isConstant() && a.op() == Op::Add && a.right().isConstant()) {
        return a.left() + Expression(a.right().value() + b.value());
    }
    if (b.isConstant() && a.op() == Op::Sub && a.right().isConstant()) {
        return a.left() + Expression(b.value() - a.right().value());
    }
    // Keep sums left-leaning: a + (b + c) -> (a + b) + c.
    if (b.op() == Op::Add) {
        return (a + b.left()) + b.right();
    }
    if (b.op() == Op::Sub) {
        return (a + b.left()) - b.right();
    }
    auto [ca, ra] = splitCoefficient(a);
    auto [cb, rb] = splitCoefficient(b);
    if (ra == rb) {
        return Expression(ca + cb) * ra;
    }
    return Expression::make(Op::Add, a, b);
}

Expression operator-(const Expression& a, const Expression& b) {
    if (a.isConstant() && b.isConstant()) {
        return Expression(a.value() - b.value());
    }
    if (isValue(b, 0)) {
        return a;
    }
    if (isValue(a, 0)) {
        return -b;
    }
    if (a == b) {
        return Expression(0);
    }
    if (isNegativeConstant(b)) {
        return a + Expression(-b.value());
    }
    if (b.op() == Op::Neg) {
        return a + b.left();
    }
    if (hasNegativeCoefficient(b)) {
        return a + Expression(-b.left().value()) * b.right();
    }
    if (b.isConstant() && a.op() == Op::Add && a.right().isConstant()) {
        return a.left() + Expression(a.right().value() - b.value());
    }
    if (b.isConstant() && a.op() == Op::Sub && a.right().isConstant()) {
        return a.left() - Expression(a.right().value() + b.value());
    }
    if (b.op() == Op::Add) {
        return (a - b.left()) - b.right();
    }
    if (b.op() == Op::Sub) {
        return (a - b.left()) + b.right();
    }
    auto [ca, ra] = splitCoefficient(a);
    auto [cb, rb] = splitCoefficient(b);
    if (ra == rb) {
        return Expression(ca - cb) * ra;
    }
    return Expression::make(Op::Sub, a, b);
}

Expression operator-(const Expression& a) {
    if (a.isConstant()) {
        return Expression(-a.value());
    }
    if (a.op() == Op::Neg) {
        return a.left();
    }
    if (a.op() == Op::Mul && a.left().isConstant()) {
        return Expression(-a.left().value()) * a.right();
    }
    if (a.op() == Op::Sub) {
        return a.right() - a.left();
    }
    return Expression::make(Op::Neg, a);
}

Expression operator*(const Expression& a, const Expression& b) {
    if (a.isConstant() && b.isConstant()) {
        return Expression(a.value() * b.value());
    }
    if (isValue(a, 0) || isValue(b, 0)) {
        return Expression(0);
    }
    if (isValue(a, 1)) {
        return b;
    }
    if (isValue(b, 1)) {
        return a;
    }
    if (isValue(a, -1)) {
        return -b;
    }
    if (isValue(b, -1)) {
        return -a;
    }
    // Numeric coefficients go first.
    if (b.isConstant()) {
        return b * a;
    }
    if (a.op() == Op::Neg) {
        return -(a.left() * b);
    }
    if (b.op() == Op::Neg) {
        return -(a * b.left());
    }
    if (a.isConstant() && b.op() == Op::Mul && b.left().isConstant()) {
        return Expression(a.value() * b.left().value()) * b.right();
    }
    if (!a.isConstant() && a.op() == Op::Mul && a.left().isConstant()) {
        return a.left() * (a.right() * b);
    }
    if (!a.isConstant() && b.op() == Op::Mul && b.left().isConstant()) {
        return b.left() * (a * b.right());
    }
    // Keep a single fraction bar: (p/q)*r -> (p*r)/q.
    if (a.op() == Op::Div) {
        return (a.left() * b) / a.right();
    }
    if (b.op() == Op::Div) {
        return (a * b.left()) / b.right();
    }
    auto [baseA, powerA] = splitPower(a);
    auto [baseB, powerB] = splitPower(b);
    if (baseA == baseB) {
        return pow(baseA, powerA + powerB);
    }
    if (isNegativePower(b)) {
        return a / pow(b.left(), -b.right().value());
    }
    if (isNegativePower(a)) {
        return b / pow(a.left(), -a.right().value());
    }
    return Expression::make(Op::Mul, a, b);
}

Expression operator/(const Expression& a, const Expression& b) {
    if (a.isConstant() && b.isConstant() && b.value() != 0) {
        return Expression(a.value() / b.value());
    }
    if (isValue(b, 1)) {
        return a;
    }
    if (isValue(b, -1)) {
        return -a;
    }
    if (isValue(a, 0) && !isValue(b, 0)) {
        return Expression(0);
    }
    if (isNegativeConstant(b)) {
        return -(a / Expression(-b.value()));
    }
    // a/0.5 -> 2*a
    if (b.isConstant() && !isInteger(b.value()) && isInteger(1 / b.value())) {
        return Expression(1 / b.value()) * a;
    }
    if (a.op() == Op::Neg) {
        return -(a.left() / b);
    }
    if (b.op() == Op::Neg) {
        return -(a / b.left());
    }
    if (a.op() == Op::Div) {
        return a.left() / (a.right() * b);
    }
    if (b.isConstant() && b.value() != 0 && a.op() == Op::Mul && a.left().isConstant()) {
        double c = a.left().value();
        double d = b.value();
        if (!isInteger(c) || !isInteger(d) || std::fabs(c) >= 1e15 || std::fabs(d) >= 1e15) {
            return Expression(c / d) * a.right();
        }
        double g = std::gcd(static_cast<long long>(c), static_cast<long long>(d));
        if (d < 0) {
            g = -g;
        }
        if (g != 1) {
            return (Expression(c / g) * a.right()) / Expression(d / g);
        }
    }
    auto [baseA, powerA] = splitPower(a);
    auto [baseB, powerB] = splitPower(b);
    if (baseA == baseB && !a.isConstant()) {
        return pow(baseA, powerA - powerB);
    }
    return Expression::make(Op::Div, a, b);
}

Expression pow(const Expression& base, const Expression& exponent) {
    if (base.isConstant() && exponent.isConstant()) {
        return Expression(std::pow(base.value(), exponent.value()));
    }
    if (isValue(exponent, 0)) {
        return Expression(1);
    }
    if (isValue(exponent, 1)) {
        return base;
    }
    if (isValue(base, 1)) {
        return Expression(1);
    }
    if (isValue(base, 0) && exponent.isConstant() && exponent.value() > 0) {
        return Expression(0);
    }
    // (u^a)^n = u^(a*n) holds for integer n.
    if (exponent.isConstant() && isInteger(exponent.value())) {
        if (base.op() == Op::Pow && base.right().isConstant()) {
            return pow(base.left(), Expression(base.right().value() * exponent.value()));
        }
        if (base.op() == Op::Sqrt) {
            return pow(base.left(), Expression(exponent.value() / 2));
        }
        if (base.op() == Op::Div) {
            return pow(base.left(), exponent) / pow(base.right(), exponent);
        }
        if (base.op() == Op::Exp) {
            return exp(exponent * base.left());
        }
    }
    return Expression::make(Op::Pow, base, exponent);
}

namespace {

Expression function(Op op, double (*f)(double), const Expression& a) {
    if (a.isConstant()) {
        return Expression(f(a.value()));
    }
    return Expression::make(op, a);
}

// u for a = -u or a = -c*u.
std::optional<Expression> negatedArgument(const Expression& a) {
    if (a.op() == Op::Neg) {
        return a.left();
    }
    if (hasNegativeCoefficient(a)) {
        return Expression(-a.left().value()) * a.right();
    }
    return std::nullopt;
}

// f(-u) = -f(u)
Expression oddFunction(Op op, double (*f)(double), Expression (*self)(const Expression&), const Expression& a) {
    if (auto u = negatedArgument(a)) {
        return -self(*u);
    }
    return function(op, f, a);
}

// f(-u) = f(u)
Expression evenFunction(Op op, double (*f)(double), const Expression& a) {
    if (auto u = negatedArgument(a)) {
        return function(op, f, *u);
    }
    return function(op, f, a);
}

}  // namespace

Expression sin(const Expression& a) { return a.op() == Op::Asin ? a.left() : oddFunction(Op::Sin, std::sin, sin, a); }

Expression cos(const Expression& a) { return a.op() == Op::Acos ? a.left() : evenFunction(Op::Cos, std::cos, a); }

Expression tan(const Expression& a) { return a.op() == Op::Atan ? a.left() : oddFunction(Op::Tan, std::tan, tan, a); }

Expression exp(const Expression& a) {
    if (a.op() == Op::Log) {
        return a.left();
    }
    return function(Op::Exp, std::exp, a);
}

Expression log(const Expression& a) {
    if (a.op() == Op::Exp) {
        return a.left();
    }
    return function(Op::Log, std::log, a);
}

Expression sqrt(const Expression& a) { return function(Op::Sqrt, std::sqrt, a); }

Expression abs(const Expression& a) {
    if (a.op() == Op::Abs) {
        return a;
    }
    return function(Op::Abs, std::fabs, a);
}

Expression asin(const Expression& a) { return oddFunction(Op::Asin, std::asin, asin, a); }

Expression acos(const Expression& a) { return function(Op::Acos, std::acos, a); }

Expression atan(const Expression& a) { return oddFunction(Op::Atan, std::atan, atan, a); }

Expression sinh(const Expression& a) {
    return a.op() == Op::Asinh ? a.left() : oddFunction(Op::Sinh, std::sinh, sinh, a);
}

Expression cosh(const Expression& a) { return a.op() == Op::Acosh ? a.left() : evenFunction(Op::Cosh, std::cosh, a); }

Expression tanh(const Expression& a) {
    return a.op() == Op::Atanh ? a.left() : oddFunction(Op::Tanh, std::tanh, tanh, a);
}

Expression asinh(const Expression& a) { return oddFunction(Op::Asinh, std::asinh, asinh, a); }

Expression acosh(const Expression& a) { return function(Op::Acosh, std::acosh, a); }

Expression atanh(const Expression& a) { return oddFunction(Op::Atanh, std::atanh, atanh, a); }

// ---------------------------------------------------------------------------
// Public algorithms

Expression parse(const std::string& text) { return Parser(text).parse(); }

Expression substitute(const Expression& e, const std::string& var, const Expression& value) {
    switch (e.op()) {
        case Op::Constant:
            return e;
        case Op::Variable:
            return e.toString() == var ? value : e;
        default:
            return build(e.op(), substitute(e.left(), var, value),
                         isUnary(e.op()) ? Expression() : substitute(e.right(), var, value));
    }
}

Expression expand(const Expression& e) {
    switch (e.op()) {
        case Op::Add:
        case Op::Sub:
        case Op::Neg: {
            std::vector<Expression> terms;
            for (const Expression& term : termsOf(e)) {
                collectTerms(expand(term), false, terms);
            }
            return sumOf(terms);
        }
        case Op::Mul: {
            Expression a = expand(e.left());
            Expression b = expand(e.right());
            auto ta = termsOf(a);
            auto tb = termsOf(b);
            if (ta.size() == 1 && tb.size() == 1) {
                return a * b;
            }
            return sumOf(multiplyTerms(ta, tb));
        }
        case Op::Div: {
            Expression b = expand(e.right());
            std::vector<Expression> terms;
            for (const Expression& term : termsOf(expand(e.left()))) {
                collectTerms(term / b, false, terms);
            }
            return sumOf(terms);
        }
        case Op::Pow: {
            Expression base = expand(e.left());
            Expression exponent = expand(e.right());
            auto terms = termsOf(base);
            if (terms.size() > 1 && exponent.isConstant() && isInteger(exponent.value()) && exponent.value() > 1 &&
                exponent.value() <= kMaxExpandPower) {
                std::vector<Expression> result = terms;
                for (int i = 1; i < static_cast<int>(exponent.value()); ++i) {
                    result = multiplyTerms(result, terms);
                    Expression merged = sumOf(result);
                    result = termsOf(merged);
                }
                return sumOf(result);
            }
            return pow(base, exponent);
        }
        default:
            return e;
    }
}

Expression differentiate(const Expression& e, const std::string& var) {
    if (e.isFreeOf(var)) {
        return Expression(0);
    }
    if (e.op() == Op::Variable) {
        return Expression(1);
    }
    const Expression a = e.left();
    const Expression da = differentiate(a, var);
    switch (e.op()) {
        case Op::Neg:
            return -da;
        case Op::Sin:
            return cos(a) * da;
        case Op::Cos:
            return -sin(a) * da;
        case Op::Tan:
            return da / pow(cos(a), 2);
        case Op::Exp:
            return e * da;
        case Op::Log:
            return da / a;
        case Op::Sqrt:
            return da / (2 * e);
        case Op::Abs:
            return da * a / e;
        case Op::Asin:
            return da / sqrt(1 - pow(a, 2));
        case Op::Acos:
            return -(da / sqrt(1 - pow(a, 2)));
        case Op::Atan:
            return da / (1 + pow(a, 2));
        case Op::Sinh:
            return cosh(a) * da;
        case Op::Cosh:
            return sinh(a) * da;
        case Op::Tanh:
            return da / pow(cosh(a), 2);
        case Op::Asinh:
            return da / sqrt(pow(a, 2) + 1);
        case Op::Acosh:
            return da / sqrt(pow(a, 2) - 1);
        case Op::Atanh:
            return da / (1 - pow(a, 2));
        default:
            break;
    }
    const Expression b = e.right();
    const Expression db = differentiate(b, var);
    switch (e.op()) {
        case Op::Add:
            return da + db;
        case Op::Sub:
            return da - db;
        case Op::Mul:
            return da * b + a * db;
        case Op::Div:
            if (b.isFreeOf(var)) {
                return da / b;
            }
            if (a.isFreeOf(var)) {
                return -(a * db) / pow(b, 2);
            }
            return (da * b - a * db) / pow(b, 2);
        case Op::Pow:
            if (b.isFreeOf(var)) {
                return b * pow(a, b - 1) * da;
            }
            if (a.isFreeOf(var)) {
                return e * log(a) * db;
            }
            return e * (db * log(a) + b * da / a);
        default:
            return Expression(0);
    }
}

Expression differentiate(const Expression& e, const std::string& var, int order) {
    Expression result = e;
    for (int i = 0; i < order; ++i) {
        result = differentiate(result, var);
    }
    return result;
}

Expression integrate(const Expression& e, const std::string& var) { return integrateImpl(e, var, 0); }

double integrate(const Expression& e, const std::string& var, double a, double b, const Variables& vars) {
    try {
        Expression antiderivative = integrate(e, var);
        Variables at = vars;
        at[var] = b;
        double upper = antiderivative.evaluate(at);
        at[var] = a;
        double result = upper - antiderivative.evaluate(at);
        if (std::isfinite(result)) {
            return result;
        }
    } catch (const IntegrationError&) {
    }
    return numericIntegral(e, var, a, b, 1e-10, vars);
}

double numericIntegral(const Expression& e, const std::string& var, double a, double b, double tolerance,
                       const Variables& vars) {
    Variables at = vars;
    double& x = at[var];
    std::function<double(double)> f = [&](double value) {
        x = value;
        return e.evaluate(at);
    };
    // A few fixed panels first so periodic integrands cannot fool the error
    // estimate on the very first step.
    constexpr int kPanels = 8;
    constexpr int kMaxDepth = 40;
    double h = (b - a) / kPanels;
    double result = 0;
    for (int i = 0; i < kPanels; ++i) {
        double lo = a + i * h;
        double hi = i + 1 == kPanels ? b : lo + h;
        double flo = f(lo);
        double fhi = f(hi);
        double fm = f((lo + hi) / 2);
        double whole = (hi - lo) / 6 * (flo + 4 * fm + fhi);
        result += adaptiveSimpson(f, lo, hi, flo, fm, fhi, whole, tolerance / kPanels, kMaxDepth);
    }
    return result;
}

double numericDerivative(const Expression& e, const std::string& var, double at, const Variables& vars) {
    Variables point = vars;
    double& x = point[var];
    auto f = [&](double value) {
        x = value;
        return e.evaluate(point);
    };
    // Five-point stencil: error O(h^4).
    double h = 1e-3 * std::max(1.0, std::fabs(at));
    return (-f(at + 2 * h) + 8 * f(at + h) - 8 * f(at - h) + f(at - 2 * h)) / (12 * h);
}

}  // namespace calculus
