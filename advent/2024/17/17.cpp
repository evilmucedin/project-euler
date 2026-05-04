#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

#include <iostream>
#include <vector>
#include <string>
#include <cmath>

// Function to convert combo operand to its actual value
int getComboValue(int comboOperand, int A, int B, int C) {
    if (comboOperand <= 3) return comboOperand;
    else if (comboOperand == 4) return A;
    else if (comboOperand == 5) return B;
    else if (comboOperand == 6) return C;
    else throw std::invalid_argument("Invalid combo operand");
}

// Function to execute the program and return the output
std::string executeProgram(const std::vector<int>& program, int A, int B, int C) {
    int ip = 0; // Instruction pointer
    std::string output;

    while (ip < program.size()) {
        int opcode = program[ip];
        int operand = program[ip + 1];
        switch (opcode) {
            case 0: // adv
                A = A / static_cast<int>(std::pow(2, getComboValue(operand, A, B, C)));
                break;
            case 1: // bxl
                B = B ^ operand;
                break;
            case 2: // bst
                B = getComboValue(operand, A, B, C) % 8;
                break;
            case 3: // jnz
                if (A != 0) {
                    ip = operand;
                    continue;
                }
                break;
            case 4: // bxc
                B = B ^ C;
                break;
            case 5: // out
                output += std::to_string(getComboValue(operand, A, B, C) % 8) + ",";
                break;
            case 6: // bdv
                B = A / static_cast<int>(std::pow(2, getComboValue(operand, A, B, C)));
                break;
            case 7: // cdv
                C = A / static_cast<int>(std::pow(2, getComboValue(operand, A, B, C)));
                break;
            default:
                throw std::invalid_argument("Invalid opcode");
        }
        ip += 2;
    }

    if (!output.empty() && output.back() == ',') {
        output.pop_back();
    }

    return output;
}

int main() {
    // Initial register values
    int A = 47792830;
    int B = 0;
    int C = 0;

    // Program to execute
    std::vector<int> program = {2,4,1,5,7,5,1,6,4,3,5,5,0,3,3,0};

    auto lines = readInputLines();
    A = parseIntegers(lines[0])[0];
    B = parseIntegers(lines[1])[0];
    C = parseIntegers(lines[2])[0];
    program = parseIntegers(lines[4]);


    // Execute the program and collect output
    std::string result = executeProgram(program, A, B, C);

    // Output the result
    std::cout << "Output: " << result << std::endl;

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
