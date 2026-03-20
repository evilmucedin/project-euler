#include <iostream>
#include <vector>

using namespace std;

static const int INF = 1000000000;

int main()
{
	while (true)
	{
		int n;
		cin >> n;
		if (n == -1)
			return 0;
		int m;
		cin >> m;

		vector<vector<int>> g(n, vector<int>(n, INF));
		for (int i = 0; i < n; i++)
			g[i][i] = 0;

		for (int e = 0; e < m; e++)
		{
			int a, b, c;
			cin >> a >> b >> c;
			--a;
			--b;
			if (c < g[a][b])
				g[a][b] = g[b][a] = c;
		}

		vector<vector<int>> dist(n, vector<int>(n));
		vector<vector<int>> nxt(n, vector<int>(n, -1));
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				dist[i][j] = g[i][j];
				if (i != j && g[i][j] < INF)
					nxt[i][j] = j;
			}
		}

		int best = INF;
		int bi = -1, bj = -1, bk = -1;
		vector<vector<int>> best_nxt;

		for (int k = 0; k < n; k++)
		{
			for (int i = 0; i < k; i++)
			{
				for (int j = i + 1; j < k; j++)
				{
					if (g[i][k] == INF || g[j][k] == INF || dist[i][j] == INF)
						continue;
					int total = dist[i][j] + g[i][k] + g[j][k];
					if (total < best)
					{
						best = total;
						bi = i;
						bj = j;
						bk = k;
						// Paths must use only vertices < k; final Floyd nxt may route through k+.
						best_nxt = nxt;
					}
				}
			}
			for (int i = 0; i < n; i++)
			{
				for (int j = 0; j < n; j++)
				{
					if (dist[i][k] + dist[k][j] < dist[i][j])
					{
						dist[i][j] = dist[i][k] + dist[k][j];
						nxt[i][j] = nxt[i][k];
					}
				}
			}
		}

		if (best >= INF)
		{
			cout << "No solution.\n";
		}
		else
		{
			vector<int> out;
			int u = bi;
			while (u != bj)
			{
				out.push_back(u);
				u = best_nxt[u][bj];
			}
			out.push_back(bj);
			out.push_back(bk);
			for (size_t t = 0; t < out.size(); t++)
			{
				if (t)
					cout << ' ';
				cout << out[t] + 1;
			}
			cout << '\n';
		}
	}

    return 0;
}
