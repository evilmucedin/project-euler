#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits> // Required for numeric_limits and streamsize

using namespace std;

void createAndWriteFile(const string& filename) {
    ofstream outFile(filename); // Open file for writing

    if (!outFile.is_open()) {
        cerr << "Error opening file for writing: " << filename << endl;
        return;
    }

    outFile << "Student: Alice, ID: 101" << endl;
    outFile << "Student: Bob, ID: 102" << endl;
    outFile << "Employee: Charlie, ID: 501" << endl;
    outFile << "Employee: David, ID: 502" << endl;
    outFile << "Student: Eve, ID: 103" << endl;

    outFile.close();
    cout << "File '" << filename << "' created with sample data." << endl;
}

void searchFile(const string& filename, const string& searchQuery) {
    ifstream inFile(filename); // Open file for reading

    if (!inFile.is_open()) {
        cerr << "Error opening file for reading: " << filename << endl;
        return;
    }

    string line;
    int lineNumber = 0;
    vector<int> foundLines;

    // Read file line by line
    while (getline(inFile, line)) {
        lineNumber++;
        // Check if the search query is a substring of the current line
        if (line.find(searchQuery) != string::npos) {
            foundLines.push_back(lineNumber);
            cout << "Found on line " << lineNumber << ": " << line << endl;
        }
    }

    inFile.close();

    if (foundLines.empty()) {
        cout << "Search query '" << searchQuery << "' not found in the file." << endl;
    } else {
        cout << "Search complete. Occurrences found on " << foundLines.size() << " line(s)." << endl;
    }
}

int main() {
    string filename = "data.txt";
    string searchQuery;

    // 1. Create the file with initial data
    createAndWriteFile(filename);
    cout << "----------------------------------------" << endl;

    // 2. Get the search request from the user
    cout << "Enter a string to search for (e.g., 'Student' or '102'): ";
    // Use getline to safely read a line of text containing spaces
    getline(cin, searchQuery);

    // Clear any potential error flags and ignore the rest of the line from the buffer
    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    cout << "----------------------------------------" << endl;
    // 3. Search the file based on the request
    searchFile(filename, searchQuery);

    return 0;
}

