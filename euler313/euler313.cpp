#include <cstdio>

#include <vector>
#include <queue>
#include <bitset>

using namespace std;

int CalcShortestPath(int n, int m)
{
    typedef vector<int> TIntVector;
    typedef vector<TIntVector> TIntVectorVector;
    typedef vector<TIntVectorVector> TIntVectorVectorVector;
    typedef vector<TIntVectorVectorVector> TIntVectorVectorVectorVector;

    TIntVector dummy1(m, -1);
    TIntVectorVector dummy2(n, dummy1);
    TIntVectorVectorVector dummy3(m, dummy2);
    TIntVectorVectorVectorVector distances(n, dummy3);

    typedef pair<int, int> TPoint;
    typedef pair<TPoint, TPoint> TPosition;
    typedef queue<TPosition> TPositions;

    TPositions positions;
    
    TPosition start = make_pair( make_pair(0, 0), make_pair(n - 1, m - 1) );
    positions.push(start);
    distances[start.first.first][start.first.second][start.second.first][start.second.second] = 0;

    bool finish = false;
    int result = -1;
    while (!finish)
    {
        TPosition current = positions.front();
        positions.pop();
        static const int DIRS[] = {-1, 0, 1, 0, 0, -1, 0, 1};
        for (size_t i = 0; i < 4; ++i)
        {
            const TPoint empty = current.second;
            const TPoint next = make_pair(empty.first + DIRS[2*i], empty.second + DIRS[2*i + 1]);
            if (next.first >= 0 && next.first < n && next.second >= 0 && next.second < m)
            {
                TPoint nextFill = current.first;
                if (nextFill == next)
                {
                    nextFill = empty;
                }
                int& distance = distances[nextFill.first][nextFill.second][next.first][next.second];
                if (-1 == distance)
                {
                    distance = 1 + distances[current.first.first][current.first.second][current.second.first][current.second.second];
                    if (nextFill.first == n -1 && nextFill.second == m - 1 && !finish)
                    {
                        finish = true;
                        result = distance;
                    }
                    positions.push(make_pair(nextFill, next));
                }
            }
        }
    }

    return result;
}

int main()
{
    for (int i = 2; i < 15; ++i)
    {
        // printf("%d %d\n", i, CalcShortestPath(i, i));
        // continue;
        int prev = 0;
        for (int j = i; j < 15; ++j)
        {
            int sp = CalcShortestPath(i, j);
            printf("%d %d %d %d %f\n", i, j, sp, sp - prev, static_cast<float>(sp)/(i + j));
            prev = sp;
        }
    }
    return 0;

    static const int MAXN = 100;
    bitset<MAXN> isprime;
    long long int result = 0;
    for (int i = 2; i < MAXN; ++i)
    {
        isprime[i] = true;
    }
    for (long long int i = 2; i < MAXN; ++i)
    {
        if (isprime[i])
        {
            if (i != 2)
            {
                result += (i*i + 11)/4;
            }
            for (int j = i + i; j < MAXN; j += i)
            {
                isprime[j] = false;
            }
        }
    }
    printf("%lld\n", result);
    return 0;
}
