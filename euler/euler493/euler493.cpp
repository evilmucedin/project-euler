#include <iostream>

#include <vector>
#include <unordered_map>

using namespace std;

typedef vector<int> TIntVector;
typedef unordered_map<TIntVector, double> TColors2Prob;

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
    TColors2Prob colors2prob;

    TIntVector colors0;
    for (size_t i = 0; i < 7; ++i)
    {
        colors0.push_back(10);
    }

    colors2prob[colors0] = 1.0;

    for (size_t i = 0; i < 20; ++i)
    {
        TColors2Prob next;
        for (TColors2Prob::const_iterator toProb = colors2prob.begin(); toProb != colors2prob.end(); ++toProb)
        {
            const TIntVector& colors = toProb->first;
            for (size_t color = 0; color < 7; ++color)
            {
                if (0 != colors[color])
                {
                    TIntVector nextColors = colors;
                    --nextColors[color];
                    next[nextColors] += static_cast<double>(colors[color])/(70.0 - i)*toProb->second;
                }
            }
        }
        colors2prob = next;
    }

    double result = 0.0;
    for (TColors2Prob::const_iterator toProb = colors2prob.begin(); toProb != colors2prob.end(); ++toProb)
    {
        size_t count = 0;
        for (size_t color = 0; color < 7; ++color)
        {
            if (10 != toProb->first[color])
            {
                ++count;
            }
        }
        result += static_cast<double>(count)*toProb->second;
    }

    cout.precision(10);
    cout << result << endl;

    return 0;
}
