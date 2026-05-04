#include <cstdio>

#include <vector>

using namespace std;

static const int P = 76543217;
// static const int N = 10;
// static const int M = 5;
static const int N = 10000;
static const int M = 5000;

typedef vector<int> TIntVector;

int main()
{
	TIntVector inv(P, -1);
	{
		TIntVector powers(P);
		int now = 1;
		for (int power = 0; power < P; ++power)
		{
			powers[power] = now;
			now = (now + now + now) % P;			
		}
		printf("powers done: %d %d\n", powers[P - 2], powers[P - 1]);
		for (int i = 0; i < P; ++i)
		{
			if (inv[powers[i]] != -1)
				printf("!!%d %d %d\n", i, powers[i], inv[powers[i]]);
			inv[powers[i]] = powers[(P - i - 1) % P];
		}
		printf("inv done\n");
		for (long long int i = 0; i < P; ++i)
		{
			long long int ii = inv[i];
			if ( (i*ii) % P != 1 )
				printf("!!!%lld %lld\n", i, ii);
		}

	}

	long long int result = 1;
	for (long long int i = 1; i <= N*N - M*M; ++i)
	{
		result = (result*i) % P;
	}
	printf("fact done\n");

	for (int x = 0; x < N; ++x)
	{
		printf("%d\n", x);
		for (int y = 0; y < N; ++y)
		{
			if (x < N - M || y < N - M)
			{
				int dx;
				if (y >= N - M)
					dx = N - M - x;
				else
					dx = N - x;
				int dy;
				if (x >= N - M)
					dy = N - M - y;
				else
					dy = N - y;
				int hook = dx + dy - 1;
				result = ((long long int)result*(long long int)inv[hook % P]) % P;
			}
		}
	}

	printf("%lld\n", result);

	return 0;
}