#include <stdlib.h>
#include <iostream>
#include <cstring>

using namespace std;

int n, m;
int mas[100][100];
bool used[100][100];
int was[100];
int path[100];
int outpath[100];
int maxpath;
int maxlength;
int outpathlength;

void solve();

int main()
{
	while (true)
	{
		cin >> n;
		if (n == -1)
			return 0;
		cin >> m;
		memset(mas,0 ,sizeof(mas));
		for(int i=0; i<m; i++)
		{
			int a, b, c;
			cin >> a >> b >> c;
			if ((mas[a-1][b-1] == 0) || (mas[a-1][b-1] > c))
			{
				mas[a-1][b-1] = c;
				mas[b-1][a-1] = c;
			}
		}
		memset(was, 0, sizeof(was));
		solve();

	}
	return 0;
}

int go(int node, int start, int step, int curlength)
{
	int retval=23323;
	was[node] = 1;
	int next = -1;
	path[step] = node;
	if ((mas[node][start] != 0) && (!used[node][start]))
	{
		retval = mas[node][start];
		next = start;
		if ((curlength + mas[node][start]) < maxlength)
		{
			maxlength = curlength + mas[node][start];
			memcpy(outpath, path, sizeof(path));
			outpathlength = step;
		}
	}
	for (int i=0; i<n; i++)
	{
		if ((mas[node][i] == 0) || (was[i] != 0) || (used[node][i]) ||
			((mas[node][i] + curlength) >= maxlength))
			continue;

		used[node][i] = true;
		used[i][node] = true;
		int a = go(i, start, step+1, curlength + mas[node][i]);
		used[node][i] = false;
		used[i][node] = false;
		if ((a != 0) && ((a + mas[node][i]) < retval))
		{
			retval = a + mas[node][i];
			next = i;
		}
	}
	was[node] = 0;

	return retval;
}

void solve()
{
	memset(outpath, 0, sizeof(outpath));
	maxlength = 1000000000;
	outpathlength = 0;
	for (int i=0; i<n; i++)
	{
		maxpath = 0;
		memset(was, 0, sizeof(was));
		memset(path,0, sizeof(path));
		go(i, i, 0, 0);
	}
	if (maxlength < 1000000000)
		for (int i=0; i<= outpathlength; i++)
			cout << outpath[i]+1 << ' ';
	else
		cout << "No solution.";
	cout << '\n';
}
