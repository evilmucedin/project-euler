#pragma once

#include <stack>

using namespace std;

template<typename T>
struct Max {
    bool operator()(const T& a, const T& b) const {
        return a >= b;
    }
};

template<typename T>
struct Min {
    bool operator()(const T& a, const T& b) const {
        return a <= b;
    }
};

template<typename T, typename Aggr = Min<T>>
class MonotonicStack {
   public:
    stack<T> st_;
    stack<T> mn_;
    Aggr aggr_;

    MonotonicStack(Aggr aggr = Aggr()) : aggr_(aggr) {}

    void push(T x) {
        if (st_.empty() || aggr_(x, mn_.top())) {
            mn_.push(x);
        }
        st_.push(x);
    }

    void pop() {
        assert(!empty());
        if (st_.top() == mn_.top()) {
            mn_.pop();
        }
        st_.pop();
    }

    T top() const { return st_.top(); }

    T getMin() const { return mn_.top(); }

    bool empty() const { return st_.empty(); }

    bool size() const { return st_.size(); }
};
