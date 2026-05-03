#include <cstdio>
#include <vector>
#include <algorithm>

using namespace std;

typedef unsigned long long int ui64;
typedef char i8;

struct TPoint {
	i8 X;
	i8 Y;

	TPoint() {

	}

	TPoint(i8 x, i8 y)
		: X(x)
		, Y(y)
	{
	}

	bool operator==(const TPoint& rhs) const {
		return X == rhs.X && Y == rhs.Y;
	}

	bool operator<(const TPoint& rhs) const {
		if (X != rhs.X)
			return X < rhs.X;
		return Y < rhs.Y;
	}
};

typedef vector<TPoint> TPoints;
typedef vector<TPoints> TGenerated;

void Generate(short x, short y, size_t max, TPoints& points, TGenerated& result) {
	if (points.size() < max) {
		static const int DIRS[] = {1, 0, -1, 0, 0, 1, 0, -1};
		for (size_t i = 0; i < 4; ++i) {
			const short newX = x + DIRS[2*i];
			const short newY = y + DIRS[2*i + 1];
			TPoint nw(newX, newY);
			if (find(points.begin(), points.end(), nw) == points.end()) {
				points.push_back(nw);
				Generate(newX, newY, max, points, result);
				points.pop_back();
			}
		}
	} else {
		result.push_back(points);
	}
}

static bool IsNeightbour(const TPoint& a, const TPoint& b) {
	return (a.X + 1 == b.X && a.Y == b.Y) ||
 		   (a.X == b.X + 1 && a.Y == b.Y) ||
 		   (a.X == b.X && a.Y + 1 == b.Y) ||
 		   (a.X == b.X && a.Y == b.Y + 1);
}

int main() {

	static const int N = 15;

	TGenerated generatedConfigs;

	{
		TGenerated generated;
		{
			printf("Path generation...\n");
			TPoints dummy;
			Generate(N + 1, N + 1, N, dummy, generated);
		}

		printf("Config generation... %d\n", generated.size());
		generatedConfigs.resize(generated.size());
		for (size_t i = 0; i < generated.size(); ++i) {
			for (size_t j = 0; j < generated[i].size(); ++j)
				for (size_t k = j + 1; k < generated[i].size(); ++k)
					if (IsNeightbour(generated[i][j], generated[i][k]))
						generatedConfigs[i].push_back(TPoint(j, k));
		}

		printf("Uniq sort... %d\n", generatedConfigs.size());
		sort(generatedConfigs.begin(), generatedConfigs.end());
		printf("Uniq unique... %d\n", generatedConfigs.size());
		generatedConfigs.erase(unique(generatedConfigs.begin(), generatedConfigs.end()), generatedConfigs.end());
		printf("Uniq end... %d\n", generatedConfigs.size());
	}

	printf("Counting...\n");

	ui64 sum = 0;
	for (int i = 0; i < 1 << N; ++i) {
		int max = 0;
		for (size_t j = 0; j < generatedConfigs.size(); ++j) {
			int count = 0;
			for (size_t k = 0; k < generatedConfigs[j].size(); ++k)
				if ( (i & (1ULL << generatedConfigs[j][k].X)) && (i & (1ULL << generatedConfigs[j][k].Y)) )
					++count;
			if (count > max)
				max = count;
		}
		if (0 == i % 100)
			printf("%d %d\n", i, max);
		sum += max;
	}

	printf("%lld %.20lf\n", sum, double(sum)/(1 << N));

	return 0;
}
