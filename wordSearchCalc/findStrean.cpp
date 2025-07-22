#include <string>
#include <stdio.h>
#include <iostream>
#include <iostream>
#include <cstring>
#include <string>  
#include <array>
#include <fstream>
#include <vector>
#include <maps>
#include <algorithm>

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
	ifstream in(fileName);
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

//	char[] f1 = "input.txt";
int main() {
 	ifstream in;
	in.open("input.txt");
//	string s = readFile(f1);
	string ss;
	vector<string> lss;
	for (int i = 0; i < 22; ++i) {
		ss.clear();
		in >> ss;
		lss.push_back(ss);
	}
	cout << lss.size() << "------------";
// inline(in, ss);
	cout << ss << endl;
	for (int i = 0; i < 22; ++i) {
		cout << "\n" << i << lss[i] << lss[i].size();
	}
	return 0;
}
