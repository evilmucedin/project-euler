#include <cstdio>

#define _USE_MATH_DEFINES

#include <cmath>

#include <vector>
#include <algorithm>

using namespace std;

typedef vector<double> TDoubleVector;
typedef vector<size_t> TSizeTVector;

static const size_t N = 10000;

double fn(int k)
{
    return exp(static_cast<double>(k)/N) - 1.0;
}

template<typename T>
T Sqr(T x)
{
    return x*x;
}

template<typename T>
T Abs(T x)
{
    if (x >= 0)
    {
        return x;
    }
    else
    {
        return -x;
    }
}

struct TSum2
{
    double m_value;
    short m_index1;
    short m_index2;

    TSum2()
    {
    }

    TSum2(double value, short index1, short index2)
        : m_value(value)
        , m_index1(index1)
        , m_index2(index2)
    {
    }

    bool operator<(const TSum2& sum2) const
    {
        return m_value < sum2.m_value;
    }
};

typedef vector<TSum2> TSums;

int main()
{
    TDoubleVector nums;

    int k = 0;
    while (fn(k) < M_PI)
    {
        nums.push_back(fn(k));
        ++k;
    }

    TSums sums;
    sums.reserve(Sqr(nums.size())/2);

    for (size_t index1 = 0; index1 < nums.size(); ++index1)
    {
        if (0 == (index1 % 100))
        {
            fprintf(stderr, "...%d\n", static_cast<int>(index1));
        }
        for (size_t index2 = index1 + 1; index2 < nums.size(); ++index2)
        {
            sums.push_back( TSum2(nums[index1] + nums[index2], index1, index2) );
        }
    }
    std::sort(sums.begin(), sums.end());
    fprintf(stderr, "Sorted...\n");

    double best = 1000.0;
    int index1 = 0;
    int index2 = static_cast<int>(sums.size()) - 1;
    for (size_t index1 = 0; index1 < sums.size(); ++index1)
    {
        for (int j = index2 - 5; j <= index2 + 5; ++j)
        {
            if (j >= 0 && j < sums.size())
            {
                double sum = sums[index1].m_value + sums[j].m_value;
                double diff = Abs(sum - M_PI);
                if (diff < best)
                {
                    best = diff;
                    long long int res = 0;
                    res += Sqr(static_cast<long long int>(sums[index1].m_index1));
                    res += Sqr(static_cast<long long int>(sums[index1].m_index2));
                    res += Sqr(static_cast<long long int>(sums[j].m_index1));
                    res += Sqr(static_cast<long long int>(sums[j].m_index2));
                    if (best < 1e-6)
                    {
                        printf("%lf %d %d %d %d %lld\n", best, sums[index1].m_index1, sums[index1].m_index2, sums[j].m_index1, sums[j].m_index2, res);
                    }
                }
            }
            while ((index2 >= 0) && (sums[index1].m_value + sums[index2].m_value > M_PI))
            {
                --index2;
            }
        }
    }

    return 0;
}