#include <cmath>

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <set>

using namespace std;

typedef vector<int> TIntVector;
typedef map<int, TIntVector> TIntIntVectorMap;
typedef set<long long int> TWas;

template<typename T>
T GCD(T a, T b)
{
    while (0 != b)
    {
        T temp = a;
        a = b;
        b = temp % b;
    }
    return a;
}

template<typename T>
T Sqr(T x)
{
    return x*x;
}

int main()
{
    TIntIntVectorMap pyth;

    size_t count = 0;

    static const long long int MAXP = 10000000;
    // static const long long int MAXP = 1000;

    static const long long int MAXM = 4000;
    for (long long int m = 1; m <= MAXM; ++m)
    {
        if ((m % 30) == 0)
        {
            cerr << m << " " << count << endl;
        }

        for (long long int n = m + 1; n <= MAXM; ++n)
        {
            if (1 == GCD(m, n))
            {
                long long int a = n*n - m*m;
                long long int b = 2*n*m;

                long long int maxk = MAXP/(a + b)/2;
                for (int k = 1; k <= maxk; ++k)
                {
                    pyth[k*a].push_back(k*b);
                    pyth[k*b].push_back(k*a);
                    ++count;
                }
            }
        }
    }

    cerr << count << endl;

    long long int count2 = 0;
    long long int sum = 0;

    for (TIntIntVectorMap::iterator toA = pyth.begin(); toA != pyth.end(); ++toA)
    {
        sort(toA->second.begin(), toA->second.end());
        toA->second.erase( unique(toA->second.begin(), toA->second.end()), toA->second.end() );
    }

    cerr << "unique" << endl;

    typedef set<long long int> TWas;
    TWas was;

    for (TIntIntVectorMap::const_iterator toR = pyth.begin(); toR != pyth.end(); ++toR)
    {
        long long int r = toR->first;

        if (0 == (r % 10000))
        {
            cerr << r << " " << count2 << endl;
        }

        const TIntVector& catets = toR->second;
        for (size_t i = 0; i < catets.size(); ++i)
        {
            long long int bs = catets[i];
            for (size_t j = i; j < catets.size(); ++j)
            {
                long long int cs = catets[j];
                long long int n = bs*cs - r*r;
                if (n > 0)
                {
                    long long int d = r*r*(bs + cs);
                    if ( 0 == (d % n) )
                    {
                        long long int as = d/n;
                        long long int a = bs + cs;
                        long long int b = as + cs;
                        long long int c = as + bs;
                        long long int p = a + b + c;
                        if (p <= MAXP && (a + b > c) && (a + c > b) && (b + c > a) && (a > 0) && (b > 0) && (c > 0))
                        {
                            long long int key = bs + cs + as + as*bs*cs;
                            if (was.find(key) == was.end())
                            {
                                was.insert(key);
                                long long int la = sqrt(Sqr(bs) + Sqr(r)) + 0.499;
                                long long int lb = sqrt(Sqr(cs) + Sqr(r)) + 0.499;
                                long long int lc = sqrt(Sqr(as) + Sqr(r)) + 0.499;
                                long long int s = p + la + lb + lc;
                                sum += s;
                                // cerr << catets[i] + as << " " << catets[j] + as << " " << catets[i] + catets[j] << " " << r << " " << catets[i] << " " << catets[j] << " " << as << endl;
                                ++count2;
                            }
                        }
                    }
                }
            }
        }
    }

    cout << count2 << endl;
    cout << sum << endl;

    return 0;
}
