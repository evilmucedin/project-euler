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

int main()
{
    TIntIntVectorMap pyth;

    size_t count = 0;

    static const long long int MAXP = 10000000;
    // static const long long int MAXP = 1000;

    static const long long int MAXM = 4000;
    for (long long int m = 1; m <= MAXM; ++m)
    {
        cerr << m << " " << count << endl;

        for (long long int n = m + 1; n <= MAXM; ++n)
        {
            if (1 == GCD(m, n))
            {
                long long int a = n*n - m*m;
                long long int b = 2*n*m;

                long long int maxk = MAXP/a;
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

    for (TIntIntVectorMap::iterator toA = pyth.begin(); toA != pyth.end(); ++toA)
    {
        sort(toA->second.begin(), toA->second.end());
        toA->second.erase( unique(toA->second.begin(), toA->second.end()), toA->second.end() );
    }

    TWas was;

    for (TIntIntVectorMap::const_iterator toA1 = pyth.begin(); toA1 != pyth.end(); ++toA1)
    {
        int a1 = toA1->first;
        for (TIntVector::const_iterator toR = toA1->second.begin(); toR != toA1->second.end(); ++toR)
        {
            int r = *toR;
            TIntIntVectorMap::const_iterator toR2 = pyth.find(r);
            for (TIntVector::const_iterator toA2 = toR2->second.begin(); toA2 != toR2->second.end(); ++toA2)
            {
                int a2 = *toA2;
                int a = a1 + a2;
                double beta = 2*atan( static_cast<double>(r)/a1 );
                double gamma = 2*atan( static_cast<double>(r)/a2 );
                double alpha = 2*M_PI - beta - gamma;
                double sn = static_cast<double>(a)/sin(alpha);
                double b = sin(beta)*sn;
                double c = sin(gamma)*sn;
                long long int ib = static_cast<long long int>(b + 0.1);
                long long int ic = static_cast<long long int>(c + 0.1);
                static const double EPS = 1e-8;
                if ( fabs(b - ib) < EPS && fabs(c - ic) < EPS && (a + ib + ic <= MAXP) && (ib >= 1) && (ic >= 1) && (a + ib > ic) && (a + ic > ib) && (ib + ic > a) )
                {
                    long long int sum = a + ib + ic;
                    long long int slc = a*ib*(a + ib - ic);
                    if (0 == (slc % sum))
                    {
                        long long int lc = sqrt(slc/sum) + 0.4999;
                        if (lc*lc*sum == slc)
                        {
                            long long int slb = a*ic*(a + ic - ib);
                            if (0 == (slb % sum))
                            {
                                long long int lb = sqrt(slb/sum) + 0.4999;
                                if (lb*lb*sum == slb)
                                {
                                    long long int sla = ib*ic*(ib + ic - a);
                                    if (0 == (sla % sum))
                                    {
                                        long long int la = sqrt(sla/sum) + 0.4999;
                                        if (la*la*sum == sla)
                                        {
                                            long long int key = a + ib + ic + a*ib*ic;
                                            if (was.find(key) == was.end())
                                            {
                                                was.insert(key);
                                                // cerr << a << " " << ib << " " << ic << " " << la << " " << lb << " " << lc << endl;
                                                // cerr << static_cast<double>(la*la)/b/c + static_cast<double>(lb*lb)/a/c + static_cast<double>(lc*lc)/a/b << endl;
                                                count2 += a + ib + ic + la + lb + lc;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    // cout << a << " " << ib << " " << ic << endl;
                }
            }
        }
    }

    cout << count2 << endl;

    return 0;
}
