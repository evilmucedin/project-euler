#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

void first() {
    auto input = readInputLines();

    auto read = [&](int x, int y) {
   	int b = y;
	while (b >= 1 && isdigit(input[x][b - 1])) {
		--b;
	}
	int e = y;
	while (e + 1 < input[x].size() && isdigit(input[x][e + 1])) {
		++e;
	}
	int result = 0;
	for (int i = b; i <= e; ++i) {
		result = result*10 + input[x][i] - '0';
		input[x][i] = '.';
	}
	return result;
    };

    int result = 0;
    long long result2 = 0;
    vector<long long> gears;
    for (int i = 0; i < input.size(); ++i) {
    	for (int j = 0; j < input[i].size(); ++j) {
		if (input[i][j] != '.' && !isdigit(input[i][j])) {
			gears.clear();
			static constexpr int DIRS[] = {-1, 0, 1, 0, 0, 1, 0, -1, 1, 1, 1, -1, -1, 1, -1, -1};
			for (int k = 0; k < 8; ++k) {
				int ii = i + DIRS[2*k];
				int jj = j + DIRS[2*k + 1];
				if (ii >= 0 && ii < input.size()) {
					if (jj >= 0 && jj < input[ii].size()) {
						if (isdigit(input[ii][jj])) {
							int r = read(ii, jj);
							result += r;
							gears.emplace_back(r);
						}
					}
				}
			}
			if (input[i][j] == '*' && gears.size() == 2) {
				result2 += gears[1]*gears[0];
			}
		}
	}
    }
    cout << result << endl;
    cout << result2 << endl;
}

void second() {
    const auto input = readInputLines();
    int result = 0;
    cout << result << endl;
}

int main(int argc, char* argv[]) {
    standardInit(argc, argv);

    if (FLAGS_test == 1) {
        first();
    } else if (FLAGS_test == 2) {
        second();
    }
    return 0;
}
    
