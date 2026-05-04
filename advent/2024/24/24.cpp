#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

#include <iostream>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <bitset>
#include <string>

using namespace std;

// Function to split the input string by a delimiter and return a vector of tokens
vector<string> split(const string& str, char delimiter) {
    vector<string> tokens;
    stringstream ss(str);
    string token;
    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Function to evaluate the value of a wire given the gates and initial values
void evaluateWire(const string& gate, unordered_map<string, int>& wireValues) {
    vector<string> parts = split(gate, ' ');
    string wire1 = parts[0];
    string op = parts[1];
    string wire2 = parts[2];
    string resultWire = parts[4];
    
    int value1 = wireValues[wire1];
    int value2 = wireValues[wire2];
    int result = 0;
    
    if (op == "AND") {
        result = value1 & value2;
    } else if (op == "OR") {
        result = value1 | value2;
    } else if (op == "XOR") {
        result = value1 ^ value2;
    }
    
    wireValues[resultWire] = result;
}

int main() {
    // Initial wire values
    vector<string> initialValues = {
        "x00: 1", "x01: 0", "x02: 1", "x03: 1", "x04: 0",
        "y00: 1", "y01: 1", "y02: 1", "y03: 1", "y04: 1"
    };
    
    // Gates
    vector<string> gates = {
        "ntg XOR fgs -> mjb", "y02 OR x01 -> tnw", "kwq OR kpj -> z05", "x00 OR x03 -> fst",
        "tgd XOR rvg -> z01", "vdt OR tnw -> bfw", "bfw AND frj -> z10", "ffh OR nrd -> bqk",
        "y00 AND y03 -> djm", "y03 OR y00 -> psh", "bqk OR frj -> z08", "tnw OR fst -> frj",
        "gnj AND tgd -> z11", "bfw XOR mjb -> z00", "x03 OR x00 -> vdt", "gnj AND wpb -> z02",
        "x04 AND y00 -> kjc", "djm OR pbm -> qhw", "nrd AND vdt -> hwm", "kjc AND fst -> rvg",
        "y04 OR y02 -> fgs", "y01 AND x02 -> pbm", "ntg OR kjc -> kwq", "psh XOR fgs -> tgd",
        "qhw XOR tgd -> z09", "pbm OR djm -> kpj", "x03 XOR y03 -> ffh", "x00 XOR y04 -> ntg",
        "bfw OR bqk -> z06", "nrd XOR fgs -> wpb", "frj XOR qhw -> z04", "bqk OR frj -> z07",
        "y03 OR x01 -> nrd", "hwm AND bqk -> z03", "tgd XOR rvg -> z12", "tnw OR pbm -> gnj"
    };
    
    // Initialize wire values
    unordered_map<string, int> wireValues;
    for (const string& initialValue : initialValues) {
        vector<string> parts = split(initialValue, ':');
        string wire = parts[0];
        int value = stoi(parts[1]);
        wireValues[wire] = value;
    }
    
    // Evaluate gates
    for (const string& gate : gates) {
        evaluateWire(gate, wireValues);
    }
    
    // Collect the values of wires starting with 'z'
    string binaryResult;
    for (int i = 0; ; ++i) {
        string wire = "z" + to_string(i);
        if (wireValues.find(wire) != wireValues.end()) {
            binaryResult = to_string(wireValues[wire]) + binaryResult;
        } else {
            break;
        }
    }
    
    // Convert binary result to decimal
    if (!binaryResult.empty()) {
        int decimalResult = stoi(binaryResult, nullptr, 2);
        cout << "Decimal number produced by wires starting with 'z': " << decimalResult << endl;
    } else {
        cout << "No wires starting with 'z' found." << endl;
    }
    
    return 0;
}


DEFINE_int32(test, 1, "test number");

void first() {
    const auto input = readInputLines();
    int result = 0;
    cout << result << endl;
}

void second() {
    const auto input = readInputLines();
    int result = 0;
    cout << result << endl;
}

/*
int main(int argc, char* argv[]) {
    standardInit(argc, argv);

    if (FLAGS_test == 1) {
        first();
    } else if (FLAGS_test == 2) {
        second();
    }
    return 0;
}

  */
