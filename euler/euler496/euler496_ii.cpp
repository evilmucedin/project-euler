#include <iostream>

using namespace std;

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
    // static const size_t N = 15;
    // static const long long int N = 100;
    static const long long int N = 1000000000;
    long long int result = 0;
    for (long long int n = 1; n <= N; ++n)
    {
        if (0 == (n % 100000) || (n < 100))
        {
            cout << n << " " << result << endl;
        }
        for (long long int m = n; m <= N/n; ++m)
        {
            if (m*n <= N)
            {
                long long int a = m*n;
                long long int b = n*n;
                long long int c = m*m - n*n;
                if (a + b > c && a + c > b && b + c > a)
                {
                    if (1 == GCD(m, n))
                    {
                        long long int cnt = N/a;
                        result += a*cnt*(cnt + 1)/2;
                        // cerr << a << " " << b << " " << c << endl;
                    }
                }
            }
        }
    }
    cout << result << endl;
    return 0;
}
