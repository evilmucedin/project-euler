#include "lib/header.h"

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
using namespace std;

void printTransactions(double m, int k, int d, vector <string> name, vector <int> owned, vector < vector <double> > prices) {
    //Enter your code here
}

int main() {
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
