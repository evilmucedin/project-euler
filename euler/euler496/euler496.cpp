#include <cmath>

#include <iostream>
#include <algorithm>
#include <vector>
#include <set>

using namespace std;

typedef vector<long long int> TIntVector;
typedef set<int> TIntSet;

void GetDivs(long long int a, TIntVector* res)
{
    res->clear();
    int top = sqrt(sqrt(2*a));
    for (int i = 2; i <= top; ++i)
    {
        if ( ((a % i) == 0) && (i <= a) )
        {
            res->push_back(i);
        }
    }
}

using namespace std;

long long int GCD(long long int a, long long int b)
{
    while (0 != b)
    {
        int temp = a;
        a = b;
        b = temp % b;
    }
    return a;
}

int main()
{
    long long int sum = 0;
    long long int count = 0;
    TIntVector divs;
    TIntSet good;
    // static const long long int N = 15;
    static const long long int N = 50;
    // static const long long int N = 1000000000;
    for (long long int a = 1; a <= N; ++a)
    {
        if ( 0 == (a % 100000) )
        {
            cerr << a << " " << sum << " " << count << endl;
        }
        int top;
        if (1 == (a % 2))
            top = a/2 + 1;
        else
            top = a/2 + 1;
        // GetDivs(a, &divs);
        /*
        int top2 = sqrt(2*a);
        for (long long int bb = 2; bb < top2; ++bb)
        {
            if ( 0 == (a % bb) )
            {
                ++count;
            }
        }
        */
        int acount = 0;
        for (int b = top; b < a; ++b)
        {
            if ( ((a*a) % b) == 0 )
            {
                int rem = a/GCD(a, b);
                int rem2 = b/rem;
                if (rem*rem > 2*a)
                {
                    cout << "\t" << a << " " << (a*a)/b << " " << b << " " << GCD(a, b) << " " << rem << " " << rem2 << endl;
                }
                ++acount;
            }
        }
        cout << acount << " ";
        /*
        for (size_t i = 0; i < divs.size(); ++i)
        {
            cout << "!!!!" << divs[i] << endl;
        }
        */
        // cout << a << " " << acount << " " << divs.size() << endl;
        /*

        good.clear();
        for (int i = 0; i < divs.size(); ++i)
        {
            int b1 = divs[i];
            int b2 = a/2/b1 + 1;
            int b = b1*b2;
            if ( (a*a) % b == 0 )
            {
                if (b >= top && b < a)
                {
                    if (good.find(b) == good.end())
                    {
                        good.insert(b);
                        // cout << a << " " << b << " " << (a*a)/b - b << endl;
                        sum += a;
                        ++count;
                        --acount;
                    }
                }
            }
        }
        if (0 != acount)
        {
            cerr << a << endl;
        }
        */
    }
    cout << sum << endl;
    return 0;
}
