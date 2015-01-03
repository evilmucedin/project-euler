#include <cmath>

#include <iostream>

#include <vector>
#include <algorithm>
#include <map>

using namespace std;

typedef vector<int> TIntVector;
typedef map<int, int> TCounters;
typedef unsigned long long TULL;
typedef vector<bool> TBoolVector;
typedef vector<double> TDoubleVector;

struct TFactor
{
    double m_number;
    int m_count;

    TFactor()
    {
    }

    TFactor(double number, int count)
        : m_number(number)
        , m_count(count)
    {
    }
};

typedef vector<TFactor> TFactors;

void KnapSnack2(const TFactors& factors, const TFactors& mask1, double* bestSum, TULL* result)
{
}

void KnapSnack(const TFactors& factors, size_t index, double sum, double target, TFactors& mask, double* bestSum, TULL* bestMask)
{
    if (sum < target)
    {
        if (index == factors.size())
        {
            if (sum > 0.96*target)
            {
                KnapSnack2(factors, mask, bestSum, bestMask);
            }
        }
        else
        {
            for (int i = 0; i < factors[index].m_count; ++i)
            {
                mask[index].m_count = i;
                KnapSnack(factors, index + 1, sum + factors[index].m_number*i, target, mask, bestSum, bestMask);
            }
        }
    }
}

void Iterate(const TFactors& factors, size_t index, const TDoubleVector& tail, double a, double b, double c, double limit, double* bestDiff, double* bestA, double* bestB, double* bestC)
{
    if (factors.size() == index)
    {
        const double mx = max(a, max(b, c));
        const double mn = min(a, min(b, c));
        if (mx - mn < *bestDiff)
        {
            TULL a2 = static_cast<TULL>(exp(a) + 0.5);
            TULL b2 = static_cast<TULL>(exp(b) + 0.5);
            TULL c2 = static_cast<TULL>(exp(c) + 0.5);
            cerr << mx - mn << "\t" << a << "\t" << b << "\t" << c << "\t" << a2 + b2 + c2 << endl;
            *bestDiff = mx - mn;
            *bestA = a;
            *bestB = b;
            *bestC = c;
        }
    }
    else
    {
        if (a > limit || b > limit || c > limit)
        {
            return;
        }
        double diff = max(a, max(b, c)) - min(a, min(b, c));
        if (diff > tail[index] + *bestDiff)
        {
            return;
        }
        const TFactor& factor = factors[index];
        if (index != 0)
        {
            for (size_t i = 0; i <= factor.m_count; ++i)
            {
                for (size_t j = 0; j + i <= factor.m_count; ++j)
                {
                    if (index < 2)
                    {
                        cerr << index << "\t" << i << "\t" << j << "\t" << factor.m_count << "\t" << diff << "\t" << tail[index] + *bestDiff << endl;
                    }
                    Iterate(factors, index + 1, tail, a + i*factor.m_number, b + j*factor.m_number, c + (factor.m_count - i - j)*factor.m_number, limit, bestDiff, bestA, bestB, bestC);
                }
            }
        }
        else
        {
        }
            for (size_t i = 0; i <= factor.m_count; ++i)
            {
                for (size_t j = i; 2*(j + i) <= factor.m_count; ++j)
                {
                    if (index < 1)
                    {
                        cerr << index << "\t" << i << "\t" << j << "\t" << factor.m_count << endl;
                    }
                    Iterate(factors, index + 1, tail, a + i*factor.m_number, b + j*factor.m_number, c + (factor.m_count - i - j)*factor.m_number, limit, bestDiff, bestA, bestB, bestC);
                }
            }
    }
}

int main()
{
    TIntVector divs;

    static const size_t N = 43;

    for (size_t i = 2; i <= N; ++i)
    {
        size_t ii = i;
        for (size_t jj = 2; jj <= N; ++jj)
        {
            while ( 0 == (ii % jj) )
            {
                divs.push_back(jj);
                ii /= jj;
            }
        }
    }

    cout << divs.size() << endl;

    TCounters counters;
    for (size_t i = 0; i < divs.size(); ++i)
    {
        ++counters[divs[i]];
    }

    TFactors factors;
    double sum = 0.0;
    TULL var = 1;
    for (TCounters::const_iterator toCounter = counters.begin(); toCounter != counters.end(); ++toCounter)
    {
        factors.push_back( TFactor(log(static_cast<double>(toCounter->first)), toCounter->second) );
        sum += factors.back().m_number*factors.back().m_count;
        var *= factors.back().m_count*(factors.back().m_count + 1)/2;
    }
    TDoubleVector tail(factors.size() + 2);
    for (ssize_t i = factors.size() - 1; i >= 0; --i)
    {
        tail[i] = tail[i + 1] + factors[i].m_count*factors[i].m_number;
    }
    cout << "var: " << var << endl;

    double bestDiff = 10000.0;
    double bestA, bestB, bestC;
    Iterate(factors, 0, tail, 0.0, 0.0, 0.0, sum*0.33334, &bestDiff, &bestA, &bestB, &bestC);

    TULL a = static_cast<TULL>(exp(bestA) + 0.5);
    TULL b = static_cast<TULL>(exp(bestB) + 0.5);
    TULL c = static_cast<TULL>(exp(bestC) + 0.5);
    cout << bestDiff << "\t" << a + b + c << endl;

    /*
    cout << factors.size() << endl;

    cout << sum << endl;

    double best = 1000;
    TFactors mask = factors;
    TULL bestResult;
    KnapSnack(factors, 0, 0.0, sum/3.0, mask, &best, &bestResult);
    cout << bestResult << endl;
    */

    return 0;
}
