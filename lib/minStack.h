#pragma once

#include <stack>

using namespace std;

template<typename T>
class MinStack {
   public:
    stack<T> st_;
    stack<T> mn_;

    void push(T x) {
        if (st_.empty() || x <= mn_.top()) {
            mn_.push(x);
        }
        st_.push(x);
    }

    void pop() {
        if (st_.top() == mn_.top()) {
            mn_.pop();
        }
        st_.pop();
    }

    T top() const { return st_.top(); }

    T getMin() const { return mn_.top(); }

    bool empty() const { return st_.empty(); }
};
