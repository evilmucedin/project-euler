#include "lib/calculus/calculus.h"

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

// Placeholder variable used by integration by substitution.
const char* const kSubstitutionVar = "__u";

// Guards the mutually recursive integration rules.
constexpr int kMaxIntegrationDepth = 64;

// Largest integer power of a sum that expand() multiplies out.
constexpr double kMaxExpandPower = 32;

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
        case Op::Sin:
            return sin(a);
        case Op::Cos:
            return cos(a);
        case Op::Tan:
            return tan(a);
        case Op::Exp:
            return exp(a);
        case Op::Log:
            return log(a);
        case Op::Sqrt:
            return sqrt(a);
        case Op::Abs:
            return abs(a);
        case Op::Constant:
        case Op::Variable:
            break;
    }
    return a;
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
        case Op::Sin:
            return std::sin(arg(n.a));
        case Op::Cos:
            return std::cos(arg(n.a));
        case Op::Tan:
            return std::tan(arg(n.a));
        case Op::Exp:
            return std::exp(arg(n.a));
        case Op::Log:
            return std::log(arg(n.a));
        case Op::Sqrt:
            return std::sqrt(arg(n.a));
        case Op::Abs:
            return std::fabs(arg(n.a));
    }
    return 0;
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

const char* functionName(Op op) {
    switch (op) {
        case Op::Sin:
            return "sin";
        case Op::Cos:
            return "cos";
        case Op::Tan:
            return "tan";
        case Op::Exp:
            return "exp";
        case Op::Log:
            return "log";
        case Op::Sqrt:
            return "sqrt";
        case Op::Abs:
            return "abs";
        default:
            return "?";
    }
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
            return {std::string(functionName(e.op())) + "(" + print(e.left()).text + ")", kAtom, false};
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
        static const std::map<std::string, Op> kFunctions = {
            {"sin", Op::Sin}, {"cos", Op::Cos}, {"tan", Op::Tan},   {"exp", Op::Exp},
            {"log", Op::Log}, {"ln", Op::Log},  {"sqrt", Op::Sqrt}, {"abs", Op::Abs},
        };
        auto it = kFunctions.find(name);
        if (it == kFunctions.end()) {
            pos_ = at;
            fail("unknown function '" + name + "'");
        }
        return build(it->second, arg, Expression());
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
        case Op::Div:
            collectFactors(e.left(), var, out);
            collectFactors(pow(e.right(), -1), var, out);
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

// Multiset equality under structural comparison.
bool sameFactors(const std::vector<Expression>& a, const std::vector<Expression>& b) {
    if (a.size() != b.size()) {
        return false;
    }
    std::vector<bool> used(b.size(), false);
    for (const Expression& x : a) {
        bool found = false;
        for (size_t j = 0; j < b.size(); ++j) {
            if (!used[j] && b[j] == x) {
                used[j] = true;
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }
    return true;
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

Expression integrateImpl(const Expression& e, const std::string& var, int depth);

// Antiderivatives of f(a*x + b) for the elementary functions: F(u) / a.
std::optional<Expression> integrateTable(const Expression& e, const std::string& var) {
    if (e.op() == Op::Pow) {
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
        default:
            return std::nullopt;
    }
}

// ∫ h(g(x)) * c*g'(x) dx = c * H(g(x)), where H is an antiderivative of h.
std::optional<Expression> integrateBySubstitution(const Expression& e, const std::string& var, int depth) {
    const Expression u = Expression::variable(kSubstitutionVar);
    Factors f = factorsOf(e, var);
    for (size_t i = 0; i < f.factors.size(); ++i) {
        const Expression& factor = f.factors[i];
        // Candidate inner functions g together with the outer h(u).
        std::vector<std::pair<Expression, Expression>> candidates = {{factor, u}};
        if (isUnary(factor.op()) && factor.op() != Op::Neg) {
            candidates.push_back({factor.left(), build(factor.op(), u, Expression())});
        } else if (factor.op() == Op::Pow) {
            if (factor.right().isFreeOf(var)) {
                candidates.push_back({factor.left(), pow(u, factor.right())});
            } else if (factor.left().isFreeOf(var)) {
                candidates.push_back({factor.right(), pow(factor.left(), u)});
            }
        }
        std::vector<Expression> rest;
        for (size_t j = 0; j < f.factors.size(); ++j) {
            if (j != i) {
                rest.push_back(f.factors[j]);
            }
        }
        for (const auto& [inner, outer] : candidates) {
            Factors derivative = factorsOf(differentiate(inner, var), var);
            if (!sameFactors(rest, derivative.factors)) {
                continue;
            }
            try {
                Expression antiderivative = integrateImpl(outer, kSubstitutionVar, depth + 1);
                return f.coefficient * substitute(antiderivative, kSubstitutionVar, inner) / derivative.coefficient;
            } catch (const IntegrationError&) {
            }
        }
    }
    return std::nullopt;
}

bool isPartsTarget(const Expression& e, const std::string& var) {
    switch (e.op()) {
        case Op::Exp:
        case Op::Sin:
        case Op::Cos:
        case Op::Log:
            return linearSlope(e.left(), var).has_value();
        case Op::Pow:
            return e.left().isFreeOf(var) && linearSlope(e.right(), var).has_value();
        default:
            return false;
    }
}

// ∫ p(x) * t(x) dx for a polynomial p and t one of exp, sin, cos, c^x, log of a
// linear argument.
std::optional<Expression> integrateByParts(const Expression& e, const std::string& var, int depth) {
    Factors f = factorsOf(e, var);
    std::optional<size_t> target;
    std::vector<Expression> polynomial;
    for (size_t i = 0; i < f.factors.size(); ++i) {
        if (!target && isPartsTarget(f.factors[i], var)) {
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
    if (t.op() == Op::Log) {
        // Differentiate the logarithm, integrate the polynomial.
        Expression antiP = integrateImpl(p, var, depth + 1);
        return antiP * t - integrateImpl(antiP * differentiate(t, var), var, depth + 1);
    }
    // Differentiate the polynomial; its degree drops on every step.
    Expression antiT = integrateImpl(t, var, depth + 1);
    return p * antiT - integrateImpl(differentiate(p, var) * antiT, var, depth + 1);
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
            if (e.left().isFreeOf(var)) {
                if (auto k = linearSlope(e.right(), var)) {
                    return e.left() * log(abs(e.right())) / *k;
                }
            }
            break;
        default:
            if (auto result = integrateTable(e, var)) {
                return *result;
            }
    }
    if (auto result = integrateBySubstitution(e, var, next)) {
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

}  // namespace

Expression sin(const Expression& a) { return function(Op::Sin, std::sin, a); }

Expression cos(const Expression& a) { return function(Op::Cos, std::cos, a); }

Expression tan(const Expression& a) { return function(Op::Tan, std::tan, a); }

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
