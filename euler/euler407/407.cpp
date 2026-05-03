#include <cstdio>
#include <cmath>

#include <vector>
#include <algorithm>

using namespace std;

typedef unsigned long long int ui64;

typedef vector<int> TIntVector;

static const size_t MAXN = 10000000;

void GetDivisors(int n, TIntVector* result)
{
    result->clear();
    int top = static_cast<int>(sqrtf(n) + 1e-9f);
    for (int div = 1; div <= top; ++div)
    {
        if (0 == (n % div))
        {
            result->push_back(div);
            result->push_back(n / div);
        }
    }
    sort(result->begin(), result->end());
}

int main()
{
    TIntVector divAPrev;
    TIntVector m(MAXN + 1, 1);
    for (ui64 a = 2; a <= MAXN; ++a)
    {
        if (!(a % 100000))
        {
            fprintf(stderr, "%llu\n", a);
            if (a == 100000)
            {
                for (size_t i = 0; i < 20; ++i)
                {
                    fprintf(stderr, "%d %d\n", static_cast<int>(i), m[i]);
                }
            }
        }

        TIntVector divA;
        GetDivisors(a, &divA);
        for (size_t i = 0; i < divA.size(); ++i)
        {
            for (size_t j = 0; j < divAPrev.size(); ++j)
            {
                const ui64 n = static_cast<ui64>(divA[i])*static_cast<ui64>(divAPrev[j]);
                if (n > MAXN)
                {
                    break;
                }
                if (n > a)
                {
                    if (a > m[n])
                    {
                        m[n] = a;
                    }
                }
            }
        }
        divAPrev.swap(divA);
    }
    ui64 result = 0;
    for (size_t i = 2; i <= m.size(); ++i)
    {
        result += m[i];
    }
    printf("%llu\n", result);
    return 0;
}
