#pragma once

#include <stack>

using namespace std;

class MinStack {
   public:
    stack<char> st, mn;

    void push(char x) {
        if (st.empty() || x <= mn.top()) mn.push(x);
        st.push(x);
    }

    void pop() {
        if (st.top() == mn.top()) mn.pop();
        st.pop();
    }

    char top() { return st.top(); }

    char getMin() { return mn.top(); }

    bool empty() const { return st.empty(); }
};
