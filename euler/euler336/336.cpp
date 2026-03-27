#include <cstdio>

#include <algorithm>
#include <vector>

using namespace std;

typedef vector<int> TIntVector;

void Out(const TIntVector& v) {
    for (TIntVector::const_iterator i = v.begin(); i != v.end(); ++i)
        printf("%c", 'A' + *i);
}

int Len(TIntVector v, bool print) {
    int result = 0;
    if (print) {
        Out(v);
        printf("\n");
    }
    for (size_t i = 0; i < v.size(); ++i) {
        size_t j = 0;
        while ( v[j] != i )
            ++j;
        if (j != i) {
            TIntVector nw;
            for (size_t k = 0; k < i; ++k)
                nw.push_back(v[k]);
            if (j + 1 != v.size()) {
                for (size_t k = j; k < v.size(); ++k)
                    nw.push_back(v[k]);
                for (size_t k = i; k < j; ++k)
                    nw.push_back(v[i + (j - k) - 1]);
                result += 2;
            } else {
                for (size_t k = i; k < v.size(); ++k)
                    nw.push_back(v[v.size() - k + i - 1]);
                ++result;
            }
            v.swap(nw);
            if (print) {
                Out(v);
                printf(" %d\n", result);
            }
        }
    }
    return result;
}

int main() {
    /*
    // int arr[] = {3, 5, 0, 4, 2, 1};
    int arr[] = {3, 0, 2, 1};
    TIntVector v(arr, arr + sizeof(arr)/sizeof(arr[0]));
    printf("%d\n", Len(v, true));
    return 0;
    */

    const static int N = 11;

    TIntVector train(N);
    for (size_t i = 0; i < N; ++i)
        train[i] = i;

    size_t count = 0;
    size_t max = 19;
    size_t i = 0;
    do {
        ++i;
        if (i % 100000 == 0) {
            Out(train);
            printf("\n");
        }
        int len = Len(train, false);
        if (len > max) {
            max = len;
            count = 0;
        }
        if (len == max)
            ++count;
        if (len == max && count == 2011) {
            Out(train);
            printf(" %d %d %d\n", len, count, max);
        }
    } while (next_permutation(train.begin(), train.end()));

    return 0;
}
