#include <cmath>

#include <iostream>

using namespace std;

int main()
{
    // static const size_t N = 15;
    // static const size_t N = 1000;
    static const size_t N = 1000;
    // static const size_t N = 10000000;
    size_t count = 0;
    for (long long int a = 1; a <= N; ++a)
    {
        /*
        if (0 == (a % 100) || (a < 10000))
        {
            cerr << a << endl;
        }
        */
        for (long long int b = a; b <= N - a; ++b)
        {
            int topC = min(N - a - b, a + b - 1);
            int dc = 1;
            int c0 = -1;
            for (long long int c = b; c <= topC; c += dc)
            {
                long long int sum = a + b + c;
                long long int slc = a*b*(a + b - c);
                if (0 == (slc % sum))
                {
                    // cerr << a << " " << b << " " << c << endl;
                    if (c0 == -1)
                    {
                        c0 = c;
                    }
                    else if (1 == dc)
                    {
                        dc = c - c0;
                    }
                    long long int lc = sqrt(slc/sum) + 0.4999;
                    if (lc*lc*sum == slc)
                    {
                        long long int slb = a*c*(a + c - b);
                        if (0 == (slb % sum))
                        {
                            long long int lb = sqrt(slb/sum) + 0.4999;
                            if (lb*lb*sum == slb)
                            {
                                long long int sla = b*c*(b + c - a);
                                if (0 == (sla % sum))
                                {
                                    long long int la = sqrt(sla/sum) + 0.4999;
                                    if (la*la*sum == sla)
                                    {
                                        cerr << a << " " << b << " " << c << " " << la << " " << lb << " " << lc << endl;
                                        // cerr << static_cast<double>(la*la)/b/c + static_cast<double>(lb*lb)/a/c + static_cast<double>(lc*lc)/a/b << endl;
                                        count += a + b + c + la + lb + lc;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    cout << count << endl;

    return 0;
}
