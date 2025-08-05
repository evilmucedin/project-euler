#include <string>
#include <stdio.h>
#include <iostream>
#include <iostream>
#include <cstring>
#include <string>
#include <array>
#include <fstream>
#include <vector>
// #include <maps>
#include <algorithm>

#include <filesystem> // Include the filesystem header


// #include "devtools_build/data_dependency_filepath.h"

using namespace std;

/*
string readFile(char[]& fileName) {
	FILE* fIn = 0;
	fIn = fopen(fileName, "r");
	if (fIn) {
		fclose(fIn);
		return "Yes";
	} else {
		return "No";
	}
}
*/

vector<string> readFile2(const string& fileName) {
	ifstream in;
	in.open(fileName);
	if (!in.is_open()) {
		cerr << "Not founf: " << fileName << endl;
	}
	vector<string> result;
	string ins;
	while (true) {
		ins.clear();
		in >> ins;
		if (0 == ins.size()) {
			break;
		}
		result.push_back(ins);
	}
	return result;
}

bool match(const vector<string>& lss, int a, int b, const string& key, int pos) {
	if (pos >= (int)key.size()) {
		return true;
	}
	if (a < 0 || a >= (int)lss.size()) {
		return false;
	}
	if (b < 0 || b >= (int)lss[a].size()) {
		return false;
	}
	if (lss[a][b] != key[pos]) {
		return false;
	}

	for (int aa = -1; aa <= 1; ++aa) {
		for (int bb = -1; bb <= 1; ++bb) {
			if (aa != 0 || bb != 0) {
				if (match(lss, a + aa, b + bb, key, pos + 1)) {
					return true;
				}
			}
		}
	}
	return false;
}

//	char[] f1 = "input.txt";
int main() {
	// cout << devtools_build::GetDataDependencyFilepath("findStrean/data/file") << endl;


    // Get the current path
    const std::string pathL =
#ifdef _WIN64
    "\\";
#else
#ifdef _WIN32
    "\\";
#else
    "/";
#endif
#endif
    ;

    // cout << "path: (" << pathL << ")" << endl;
	const std::string currentPath = filesystem::current_path().string() + pathL + "wordSearchCalc" + pathL;

    // Print the current path
    cout << "Current working directory: " << currentPath << endl;

//	string s = readFile(f1);
	string ss;
	const vector<string> lss = readFile2(currentPath + "input.txt");

/*
	lss.clear();
	ifstream in;
	in.open(currentPath + "input.txt");
	for (int i = 0; i < 22; ++i) {
		ss.clear();
		// getdelim(in, ss);
		in >> ss;
		lss.push_back(ss);
	}
*/
/*
	cout << lss.size() << "------------" << endl;
// inline(in, ss);
	cout << ss << endl;
	for (long unsigned int i = 0; i < lss.size(); ++i) {
		cout << i << lss[i] << " size=" << lss[i].length() << endl;
	}
*/
	const auto questions = readFile2(currentPath + "q.txt");
	for (long unsigned int iq = 0; iq < questions.size(); ++iq)
	{
		const auto q = questions[iq];
		int found = 0;
		for (long unsigned int i = 0; i < lss.size(); ++i) {
			for (long unsigned int j = 0; j < lss[i].size(); ++j) {
				if (match(lss, i, j, q, 0)) {
					++found;
				}
			}
		}
		cout << q << ": " << found << endl;
	}

	return 0;
}
