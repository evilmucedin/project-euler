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
typedef vector<long double> TDoubleVector;

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

void Out(const TFactors& factors)
{
    bool first = true;
    for (size_t i = 0; i < factors.size(); ++i)
    {
        int num = exp(factors[i].m_number) + 0.5;
        for (size_t j = 0; j < factors[i].m_count; ++j)
        {
            if (!first)
            {
                cerr << "*";
            }
            cerr << num;
            first = false;
        }
    }
    cerr << " + ";
}

void KnapSnack2(const TFactors& factors, const TFactors& mask1, size_t index, TFactors& mask2, long double a, long double b, long double c, long double* bestDiff, long double* bestA, long double* bestB, long double* bestC)
{
    if (index == factors.size())
    {
        const long double mx = max(a, max(b, c));
        const long double mn = min(a, min(b, c));
        if (mx - mn < *bestDiff)
        {
            TULL a2 = static_cast<TULL>(exp(a) + 0.5);
            TULL b2 = static_cast<TULL>(exp(b) + 0.5);
            TULL c2 = static_cast<TULL>(exp(c) + 0.5);
            cerr << mx - mn << "\t" << a << "\t" << b << "\t" << c << "\t" << a2 + b2 + c2 << "\t" << exp(a) << "\t" << exp(b) << "\t" << exp(c) << endl;
            Out(mask1);
            Out(mask2);
            TFactors mask3 = factors;
            for (size_t i = 0; i < factors.size(); ++i)
            {
                mask3[i].m_count = factors[i].m_count - mask1[i].m_count - mask2[i].m_count;
            }
            Out(mask3);
            cerr << endl;
            *bestDiff = mx - mn;
            *bestA = a;
            *bestB = b;
            *bestC = c;
        }
    }
    else
    {
        for (size_t i = 0; i <= factors[index].m_count - mask1[index].m_count; ++i)
        {
            mask2[index].m_count = i;
            KnapSnack2(factors, mask1, index + 1, mask2, a, b + factors[index].m_number*i, c + factors[index].m_number*(factors[index].m_count - i - mask1[index].m_count), bestDiff, bestA, bestB, bestC);
        }
    }
}

void KnapSnack(const TFactors& factors, size_t index, long double sum, long double target, TFactors& mask, long double* bestDiff, long double* bestA, long double* bestB, long double* bestC)
{
    if (sum < target)
    {
        if (index == factors.size())
        {
            if (sum > 0.999999*target)
            {
                TFactors mask2 = factors;
                KnapSnack2(factors, mask, 0, mask2, sum, 0.0, 0.0, bestDiff, bestA, bestB, bestC);
            }
        }
        else
        {
            for (int i = 0; i <= factors[index].m_count; ++i)
            {
                mask[index].m_count = i;
                KnapSnack(factors, index + 1, sum + factors[index].m_number*i, target, mask, bestDiff, bestA, bestB, bestC);
            }
        }
    }
}

void Iterate(const TFactors& factors, size_t index, const TDoubleVector& tail, long double a, long double b, long double c, long double limit, long double* bestDiff, long double* bestA, long double* bestB, long double* bestC)
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
        double mx = max(a, max(b, c));
        double mn = min(a, min(b, c));
        double md = a + b + c - mx - mn;
        double diff = mx - mn + mx - md;
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
            for (size_t i = 0; i <= factor.m_count; ++i)
            {
                for (size_t j = i; j + j + i <= factor.m_count; ++j)
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
}

int main()
{
    cout.precision(30);
    cerr.precision(30);
    cerr << sizeof(double) << "\t" << sizeof(long double) << endl;

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
    long double sum = 0.0;
    TULL var = 1;
    for (TCounters::const_iterator toCounter = counters.begin(); toCounter != counters.end(); ++toCounter)
    {
        factors.push_back( TFactor(log(static_cast<long double>(toCounter->first)), toCounter->second) );
        cerr << factors.back().m_number << "\t" << factors.back().m_count << endl;
        sum += factors.back().m_number*factors.back().m_count;
        var *= factors.back().m_count*(factors.back().m_count + 1)/2;
    }
    TDoubleVector tail(factors.size() + 2);
    for (ssize_t i = factors.size() - 1; i >= 0; --i)
    {
        tail[i] = tail[i + 1] + factors[i].m_count*factors[i].m_number;
    }
    cout << "var: " << var << endl;

    long double bestDiff = 10000.0;
    long double bestA, bestB, bestC;
    if (false)
    {
        Iterate(factors, 0, tail, 0.0, 0.0, 0.0, sum*0.33334, &bestDiff, &bestA, &bestB, &bestC);
    }
    else
    {
        TFactors mask1 = factors;
        KnapSnack(factors, 0, 0.0, sum/3.0, mask1, &bestDiff, &bestA, &bestB, &bestC);
    }

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
