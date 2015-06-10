#include <iostream>

#include <vector>

using namespace std;

typedef vector<int> TIntVector;

int main()
{
    static const size_t MAXN = 100000;

    TIntVector f(MAXN);
    f[1] = 1;
    f[2] = 1;
    f[3] = 3;

    for (size_t i = 4; i < MAXN; ++i)
    {
        if (i % 2)
        {
            size_t i4 = i/4;
            if (1 == (i % 4))
            {
                f[i] = 2*f[2*i4 + 1] - f[2*i4];
            }
            else
            {
                f[i] = 3*f[2*i4 + 1] - 2*f[2*i4];
            }
        }
        else
        {
            f[i] = f[i/2];
        }
    }

    return 0;
}