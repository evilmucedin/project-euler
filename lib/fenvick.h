#include <algorithm>

template <typename V, typename Op>
class FenwickGeneric {
    // V must be container type with
    // ```
    //	typedef ... value_type;
    //	typedef ... size_type;
    //	reference &operator[](size_type);
    //	size_type size();
    // ```
    //  and optionally
    // ```
    //	void reserve(size_type);
    //	size_type resize(size_type);
    //	size_type resize(size_type, const value_type &);
    //	value_type &emplace_back(Args... &&);
    //	void pop_back();
    // ```
    // (vector, array, deque, ...)
    // Op must be
    // 1) binary operation -- callable as `void operator() (value_type &, value_type)`
    // 2) commutative (a op b = b op a),
    //    `auto v = a; op(v, b);` and `auto v = b; op(v, a)` result `v` must be same
    // 3) associative (((a op b) op c) == (a op (b op c)))
    //    `auto v = a; op(v, b); op(v, c);` and `auto v = c; op(v, b); op(v, a);` result `v` must be same
    // e.g.
    // prefix max (note that new value in add() must be not lesser that current):
    //  Fenwick f(vector<int>(size, numeric_limits<int>::min()), [](auto &a, auto b) { if (a < b) a = b; }, true);
    // prefix min (note that new value in add() must be not greater that current):
    //  Fenwick f(vector<int>(size, numeric_limits<int>::max()), [](auto &a, auto b) { if (a > b) a = b; }, true);
    // prefix sum:
    //  Fenwick f(vector<int>(size, 0), [](auto &a, auto b) { a += b; }, true);
    // prefix sum with arbitrary init values:
    //  Fenwick f(vector<int>{1,2,3,4,5}, [](auto &a, auto b) { a += b; });
    // prefix product:
    //  Fenwick f(vector<int>(size, 1), [](auto &a, auto b) { a *= b; }, true);
    // prefix modulo product:
    //  Fenwick f(vector<int>(size, 1), [m](auto &a, auto b) { a = a*static_cast<int64_t>(b) % m; }, true);
    V A;
    [[no_unique_address]] Op op;

   public:
    typedef typename V::value_type value_type;
    typedef typename V::size_type size_type;

   private:
    // return least significant bit of i
    static size_type LSB(size_type i) { return i & -i; }
    void init() {  // time = O(n)
        for (size_type i = 1; i < A.size(); ++i) {
            auto j = i + LSB(i);
            if (j < A.size()) op(A[j], A[i]);
        }
    }

   public:
    // set no_init to true if A consists of only neutral element
    // (0 for addition, 1 for multiplication, equal values for min/max)
    // time = no_init ? O(1) : O(n)
    FenwickGeneric(V &&A, Op op = Op{}, bool no_init = false) : A{A}, op{op} {
        if (!no_init) init();
    }
    // time = O(n)
    FenwickGeneric(const V &A, Op op = Op{}, bool no_init = false) : A{A}, op{op} {
        if (!no_init) init();
    }
    // requires `op(current_value, v), current_value == new_value`
    // for op == max v must be equal or greater than current value
    // for op == min v must be equal or less than current value
    // time = O(log n)
    void add(size_type idx, const value_type &v) {
        if (idx == 0) {
            op(A[0], v);
            return;
        }
        for (; idx < A.size(); idx += LSB(idx)) op(A[idx], v);
    }
    // return op(A[0], op(A[1], op(A[2], ... op(A[idx - 1], A[idx]))))
    // time = O(log idx)
    value_type prefix_sum(size_type idx) const {
        auto v = A[0];
        for (; idx != 0; idx -= LSB(idx)) op(v, A[idx]);
        return v;
    }
    constexpr size_type size() const { return A.size(); }
    constexpr bool empty() const { return A.empty(); }
};
