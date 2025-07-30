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

#include <iostream>
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
	// cout << devtools_build::GetDataDependencyFilepath("findStrean/data/file") << endl;


    // Get the current path
    string currentPath = std::filesystem::current_path().string() + "/wordSearchCalc/";

    // Print the current path
    std::cout << "Current working directory: " << currentPath << std::endl;

//	string s = readFile(f1);
	string ss;
	vector<string> lss = readFile2(currentPath + "input.txt");
 	
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
	cout << lss.size() << "------------";
// inline(in, ss);
	cout << ss << endl;
	for (int i = 0; i < 22; ++i) {
		cout << "\n" << i << lss[i] << " size=" << lss[i].length();
	}
	return 0;
}
