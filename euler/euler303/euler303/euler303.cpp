#include <cstdio>

#include <set>
#include <queue>

using namespace std;

typedef long long int i64;

int main()
{
	typedef set<int> TIntSet;
	TIntSet forCount;
	for (size_t i = 1; i <= 10000; ++i)
	{
		forCount.insert(i);
	}

	typedef queue<i64> TQueue;
	TQueue q;
	q.push(1);
	q.push(2);
	i64 sum = 11112222222222222222/9999;
	forCount.erase(9999);
	while (!forCount.empty())
	{
		i64 now = q.front();
		q.pop();
		for (TIntSet::iterator iCount = forCount.begin(); iCount != forCount.end();)
		{
			if ((now % *iCount) == 0)
			{
				sum += now / (*iCount);
				iCount = forCount.erase(iCount);
				
				{
					printf("%d %lld [", static_cast<int>(forCount.size()), now);
					size_t index = 0;
					for (TIntSet::iterator iPCount = forCount.begin(); iPCount != forCount.end(); ++iPCount)
					{
						if (index)
						{
							printf(" ");
						}
						printf("%d", *iPCount);
						if (index > 10)
						{
							break;
						}
						++index;
					}
					printf("]\n");
				}
			}
			else
			{
				++iCount;
			}
		}
		q.push(10*now);
		q.push(10*now + 1);
		q.push(10*now + 2);
	}
	printf("%lld\n", sum);
	return 0;
}