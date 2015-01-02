#include <iostream>

#include <vector>
#include <unordered_map>

using namespace std;

typedef vector<int> TIntVector;
typedef unordered_map<TIntVector, unsigned long long> TState2Count;

namespace std
{

template<>
class hash<TIntVector>
{
public:
    size_t operator()(const TIntVector& vct) const
    {
        size_t result = 0;
        for (size_t i = 0; i < vct.size(); ++i)
        {
            result = 101*result + vct[i];
        }
        return result;
    }
};

}

int main()
{
    TState2Count state2count;

    TIntVector state0;
    state0.push_back(0);
    for (size_t i = 0; i < 10; ++i)
    {
        state0.push_back(2);
    }
    state2count[state0] = 1;

    for (size_t i = 0; i < 20; ++i)
    {
        TState2Count next;
        for (TState2Count::const_iterator toState = state2count.begin(); toState != state2count.end(); ++toState)
        {
            const TIntVector& state = toState->first;
            const size_t startDigit = (0 == i) ? 1 : 0;
            for (size_t digit = startDigit; digit < 10; ++digit)
            {
                if (state[digit + 1])
                {
                    TIntVector nextState = state;
                    nextState[0] = (state[0]*10 + digit) % 11;
                    --nextState[digit + 1];
                    next[nextState] += toState->second;
                }
            }
        }
        cout << next.size() << endl;
        swap(state2count, next);
    }

    unsigned long long result = 0;
    unsigned long long sum = 0;
    for (TState2Count::const_iterator toState = state2count.begin(); toState != state2count.end(); ++toState)
    {
        if (0 == toState->first[0])
        {
            result += toState->second;
        }
        sum += toState->second;
    }

    cout << state2count.size() << endl;
    cout << result << endl;
    cout << sum << endl;

    return 0;
}
