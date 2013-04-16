#include <cstdio>

#include <vector>
#include <queue>
#include <bitset>

using namespace std;

typedef pair<int, int> TPoint;
typedef pair<TPoint, TPoint> TPosition;

struct TState
{
	int m_distance;
	TPosition m_parent;

	TState()
	{
		m_distance = -1;
	}
};

typedef vector<TState> TStateVector;
typedef vector<TStateVector> TStateVectorVector;
typedef vector<TStateVectorVector> TStateVectorVectorVector;
typedef vector<TStateVectorVectorVector> TStateVectorVectorVectorVector;

TState& GetState(TStateVectorVectorVectorVector& vct, const TPosition& pos)
{
	return vct[pos.first.first][pos.first.second][pos.second.first][pos.second.second];
}

int CalcShortestPath(int n, int m, bool visualize = false)
{
	TStateVector dummy1(m, TState());
    TStateVectorVector dummy2(n, dummy1);
    TStateVectorVectorVector dummy3(m, dummy2);
    TStateVectorVectorVectorVector distances(n, dummy3);

    typedef queue<TPosition> TPositions;

    TPositions positions;
    
    TPosition start = make_pair( make_pair(0, 0), make_pair(n - 1, m - 1) );
    positions.push(start);
	distances[start.first.first][start.first.second][start.second.first][start.second.second].m_distance = 0;

    bool finish = false;
    TPosition resultPosition;
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
				const TPosition nextPosition = make_pair(nextFill, next);
				TState& nextState = GetState(distances, nextPosition); 
				int& distance = nextState.m_distance;
                if (-1 == distance)
                {
					distance = 1 + GetState(distances, current).m_distance;
					nextState.m_parent = current;
					if (nextFill.first == n - 1 && nextFill.second == m - 1 && !finish)
                    {
                        finish = true;
						resultPosition = nextPosition;
                    }
					positions.push(nextPosition);
                }
            }
        }
    }

	if (visualize)
	{
		TPosition currentPosition = resultPosition;
		do
		{
			for (size_t i = 0; i < n; ++i)
			{
				for (size_t j = 0; j < m; ++j)
				{
					TPoint p = make_pair(i, j);
					if (p == currentPosition.first)
					{
						printf("*");
					}
					else if (p == currentPosition.second)
					{
						printf("0");
					}
					else
					{
						printf(".");
					}
				}
				printf("\n");
			}
			printf("\n");
			currentPosition = GetState(distances, currentPosition).m_parent;
		}
		while (currentPosition != start);
	}

	return GetState(distances, resultPosition).m_distance;
}

int main()
{
	CalcShortestPath(5, 5, true);
	return 0;

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
