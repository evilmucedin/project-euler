#include <cmath>

#include <iostream>
#include <algorithm>

#include <vector>

using namespace std;

typedef vector<int> TIntVector;

void GetDivisors(int x, TIntVector* result)
{
    result->clear();
    int top = sqrt(static_cast<double>(x));
    for (int i = 1; i <= top; ++i)
    {
        if (0 == (x % i))
        {
            result->push_back(i);
            result->push_back(x / i);
        }
    }
    std::sort(result->begin(), result->end());
}

int main()
{
    static const int N = 2*10000000;
    
    TIntVector maxI(N + 1, 1);
    TIntVector divs1;
    TIntVector divs2;
    for (long long int i = 3; i <= N; ++i)
    {
        if (0 == (i % 100000))
        {
            std::cerr << i << std::endl;
        }
        
        GetDivisors(i - 1, &divs1);
        GetDivisors(i + 1, &divs2);
        for (int d1 : divs1)
        {
            for (int d2 : divs2)
            {
                long long int d = static_cast<long long int>(d1)*static_cast<long long int>(d2);
                if (d > i + 1)
                {
                    if (d <= N)
                    {
                        if (i > maxI[d])
                        {
                            maxI[d] = i;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }

    long long int sum = 0;
    for (int i = 3; i <= N; ++i)
    {
        sum += maxI[i];
    }

    std::cout << sum << std::endl;
    
    return 0;
}