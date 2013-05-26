#include <cstdio>
#include <cmath>

#include <vector>

using namespace std;

typedef vector<int> TIntVector;
typedef vector<TIntVector> TIntVectorVector;
typedef vector<bool> TBoolVector;

static const size_t N = 100000;
static const size_t MG = 75;

int main()
{
	TIntVector grundy(N + 1);
	grundy[0] = 0;
	int maxGrundy = 0;
	for (size_t i = 1; i <= N; ++i)
	{
		size_t top = static_cast<size_t>(sqrtf(static_cast<float>(i)));
		TBoolVector used(MG);
		for (size_t j = 1; j <= top; ++j)
		{
			used[grundy[i - j*j]] = true;
		}
		size_t j = 0;
		while (used[j])
		{
			++j;
		}
		grundy[i] = j;
		maxGrundy = max(maxGrundy, static_cast<int>(j));
	}

	printf("MG: %d\n", maxGrundy);

	TIntVector dummy(N + 2);
	TIntVectorVector counts(MG, dummy);
	
	for (size_t i = 0; i < MG; ++i)
	{
		counts[i][0] = 0;
		for (size_t j = 0; j <= N; ++j)
		{
			if (grundy[j] == i)
			{
				counts[i][j + 1] = counts[i][j] + 1;
			}
			else
			{
				counts[i][j + 1] = counts[i][j];			
			}
		}
	}

	long long int result = 0;
	for (size_t i = 0; i <= N; ++i)
	{
		if ((i % 50) == 0)
		{
			printf("%d\n", i);
		}
		for (size_t j = i; j <= N; ++j)
		{
			int xor = grundy[i] ^ grundy[j];
			if (xor < MG)
			{
				result += counts[xor][N + 1] - counts[xor][j];
			}
		}
	}
	printf("%lld\n", result);

	return 0;
}