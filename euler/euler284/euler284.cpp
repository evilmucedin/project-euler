#include <cstdio>

#include <vector>
#include <algorithm>

using namespace std;

typedef vector<int> TIntVector;
typedef unsigned long long ui64;

static const int BASE = 14;

static const int MAXLEN = 10000;
// static const int MAXLEN = 9;

void Print(ui64 x, int base, TIntVector* result) {
    result->clear();
    while (x) {
        result->push_back(x % base);
        x /= base;
    }
}

void Mul(const TIntVector& a, const TIntVector& b, int base, TIntVector* result) {
    TIntVector sm(a.size());
    for (size_t i = 0; i < a.size(); ++i)
        for (size_t j = 0; j < min(a.size() - i, b.size()); ++j)
            sm[i + j] += a[i]*b[j];
    int carry = 0;
    for (size_t i = 0; i < sm.size(); ++i) {
        sm[i] += carry;
        carry = sm[i] / base;
        sm[i] %= base;
    }
    result->swap(sm);
}

char Ch(int x) {
    if (x < 10)
        return x + '0';
    else
        return x - 10 + 'a';
}

char Draw(const TIntVector& p) {
    if (p.empty()) {
        printf("0");
    } else {
        for (int i = p.size() - 1; i >= 0; --i)
            printf("%c", Ch(p[i]));
    }
    printf("\n");
}

ui64 DSum(const TIntVector& v) {
    if (v.back() != 0) {
        ui64 result = 0;
        for (size_t i = 0; i < v.size(); ++i)
            result += v[i];
        return result;
    }
    return 0;
}

int main() {
    ui64 sum = 1;

    TIntVector r1(1);
    r1[0] = 7;

    TIntVector r2(1);
    r2[0] = 8;

    for (size_t ln = 1; ln <= MAXLEN; ++ln) {
        fprintf(stderr, "%d\n", ln);
        // Draw(r1);
        // Draw(r2);
        // printf("\n");

        sum += DSum(r1);
        TIntVector newR1 = r1;
        newR1.push_back(0);
        Mul(newR1, r1, BASE, &newR1);

        sum += DSum(r2);
        TIntVector newR2 = r2;
        newR2.push_back(0);
        for (size_t i = 0; i < 6; ++i)
            Mul(newR2, r2, BASE, &newR2);

        r1.swap(newR1);
        r2.swap(newR2);
    }

    TIntVector sumBase;
    Print(sum, BASE, &sumBase);
    Draw(sumBase);

    return 0;
}
