#include <map>
#include <set>
#include <list>
#include <cmath>
#include <ctime>
#include <vector>
#include <cstdlib>
#include <numeric>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <fstream>

using namespace std;

void printTransactions(double m, int k, int d, vector<string> name, vector<int> owned, vector<vector<double> > prices) {
    vector<int> plan(k);

    if (d == 0) {
        for (int i = 0; i < k; ++k) {
            plan[i] = -owned[i];
        }
    } else {
        double capital = m;
        double pricesSum = 0;
        for (int i = 0; i < k; ++i) {
            pricesSum += prices[i].back();
            capital += prices[i].back() * owned[i];
        }
        double remains = capital;
        for (int i = 0; i < k; ++i) {
            if (prices[i].back()) {
                double targetCapital = capital * prices[i].back() / pricesSum;
                int target = targetCapital / prices[i].back();
                int delta = target - owned[i];
                plan[i] = delta;
                remains -= target * prices[i].back();
            }
        }

        if (remains > 0) {
            double maxDrop = -1e9;
            int maxDropIndex = -1;
            for (int i = 0; i < k; ++i) {
                double sum = 0;
                for (size_t j = 0; j + 1 < prices[i].size(); ++j) {
                    sum += prices[i][j];
                }
                sum /= prices[i].size() - 1;
                double drop = sum - prices[i].back();
                if (drop > maxDrop) {
                    maxDrop = drop;
                    maxDropIndex = i;
                }
            }

            plan[maxDropIndex] += static_cast<int>(remains / prices[maxDropIndex].back());
        }
    }

    int count = 0;
    for (auto x : plan) {
        if (x) {
            ++count;
        }
    }

    cout << count << endl;
    for (int i = 0; i < k; ++i) {
        auto delta = plan[i];
        if (delta > 0) {
            cout << name[i] << " BUY " << delta << endl;
        } else if (delta < 0) {
            cout << name[i] << " SELL " << -delta << endl;
        }
    }
}

int main() {
#ifdef __APPLE__
    ifstream fIn("small_data_set.txt");
    cin.rdbuf(fIn.rdbuf());
#endif

    double _m;
    cin >> _m;

    int _k;
    cin >> _k;

    int _d;
    cin >> _d;

    vector<string> _name;
    vector<int> _owned;
    vector<vector<double> > _prices;

    string _name_item;
    int _owned_item;
    double _prices_item_item;

    for (int _i = 0; _i < _k; _i++) {
        cin >> _name_item;
        _name.push_back(_name_item);

        cin >> _owned_item;
        _owned.push_back(_owned_item);

        vector<double> _prices_item;
        for (int _j = 0; _j < 5; _j++) {
            cin >> _prices_item_item;
            _prices_item.push_back(_prices_item_item);
        }
        _prices.push_back(_prices_item);
    }

    printTransactions(_m, _k, _d, _name, _owned, _prices);

    return 0;
}
