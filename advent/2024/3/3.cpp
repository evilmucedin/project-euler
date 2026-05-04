#include <iostream>
#include <regex>
#include <string>
#include <fstream>

int main() {
    std::ifstream fIn("input.txt");


    std::string input = "xmul(2,4)%&mul[3,7]!@^do_not_mul(5,5)+mul(32,64]then(mul(11,8)mul(8,5))";
    std::string s;
    input = "";
    while (fIn) {
        fIn >> s;
        input += s;
        input += "\n";
    }
    std::regex mul_regex(R"(mul\((\d+),(\d+)\))");
    std::smatch matches;

    long long int total = 0;
    std::string::const_iterator searchStart(input.cbegin());

    while (std::regex_search(searchStart, input.cend(), matches, mul_regex)) {
        std::cerr << matches[0].str() << std::endl;
        int x = std::stoi(matches[1].str());
        int y = std::stoi(matches[2].str());
        total += x * y;
        searchStart = matches.suffix().first;
    }

    std::cout << "Total sum of all multiplications: " << total << std::endl;
    return 0;
}

