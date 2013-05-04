#include <cstdio>
#include <cmath>

#include <vector>

using namespace std;

typedef unsigned long long int ui64;

typedef vector<int> TIntVector;

static const size_t MAXN = 10000000 + 1;

void GetDivisors(ui64 n, TIntVector* result)
{
    result->clear();
    ui64 top = static_cast<ui64>(sqrtf(n) + 1e-9f);
    for (ui64 div = 1; div <= top; ++div)
    {
        if (0 == (n % div))
        {
            result->push_back(div);
            result->push_back(n / div);
        }
    }
}

int main()
{
    TIntVector m(MAXN);
    for (ui64 a = 0; a < MAXN; ++a)
    {
        if (!(a % 100000))
        {
            fprintf(stderr, "%llu\n", a);
        }

        TIntVector divA;
        GetDivisors(a, &divA);
        TIntVector divAMinus1;
        GetDivisors(a - 1, &divAMinus1);
        for (size_t i = 0; i < divA.size(); ++i)
        {
            for (size_t j = 0; j < divAMinus1.size(); ++j)
            {
                ui64 n = static_cast<ui64>(divA[i])*static_cast<ui64>(divAMinus1[j]);
                if (n > a && n < MAXN)
                {
                    if (a > m[n])
                    {
                        m[n] = a;
                    }
                }
            }
        }
    }
    ui64 result = 0;
    for (size_t i = 0; i < m.size(); ++i)
    {
        result += m[i];
    }
    printf("%llu\n", result);
    return 0;
}
