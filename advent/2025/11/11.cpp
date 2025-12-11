#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>

#include "lib/header.h"
#include "lib/string.h"

#include "advent/lib/aoc.h"

using namespace std;

void mm(map<string, int>& m, const string& s, int a, const map<string, vector<string>>& d) {
	auto to = m.find(s);
	if (to == m.end() || to->second > a) {
		m[s] = a;
		const auto dd = d.find(s);
		if (dd != d.end()) {
			for (const auto& ss: dd->second)
				mm(m, ss, a + 1, d);
		}
	}
}

int main() {
    auto lines = readInputLines();

    map<string, vector<string>> d;
    for (const string& s: lines) {
    	string a;
        const int n = s.size();
	int i = 0;
        while (s[i] != ':') {
		a += s[i];
		++i;
	}
	if (d.find(a) != d.end())
		throw 1;
	++i;
	++i;
	while (i < n) {
		string b;
		while (i < n && s[i] != ' ') {
			b += s[i];
			++i;
		}
		++i;
		cerr << a << " " << b << endl;
		d[a].push_back(b);
	}
    }

    map<string, int> m;

    mm(m, "you", 0, d);

    int sum = m["out"] + 2;

    cout << sum << " " << m.size() - 1 << endl;

    return 0;
}
