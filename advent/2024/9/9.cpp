#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

#include <iostream>
#include <vector>
#include <string>

// Function to parse the disk map into files and free spaces
std::vector<std::pair<char, int>> parse_disk_map(const std::string& disk_map) {
    std::vector<std::pair<char, int>> parsed_map;
    for (size_t i = 0; i < disk_map.length(); ++i) {
        char c = disk_map[i];
        int len = c - '0';
        parsed_map.push_back({c, len});
    }
    return parsed_map;
}

// Function to compact the files on the disk
std::vector<char> compact_disk(const std::string& disk_map) {
    std::vector<std::pair<char, int>> parsed_map = parse_disk_map(disk_map);
    std::vector<char> compacted_disk;

    // Collect all the file blocks
    for (const auto& p : parsed_map) {
        if (p.first != '0') {
            for (int i = 0; i < p.second; ++i) {
                compacted_disk.push_back(p.first);
            }
        }
    }

    // Fill the rest with free space
    int total_blocks = disk_map.length();
    while (compacted_disk.size() < total_blocks) {
        compacted_disk.push_back('.');
    }

    return compacted_disk;
}

// Function to calculate the filesystem checksum
long long calculate_checksum(const std::vector<char>& disk) {
    cerr << disk << std::endl;
    long long checksum = 0;
    for (size_t i = 0; i < disk.size(); ++i) {
        if (disk[i] != '.') {
            checksum += i * (disk[i] - '0');
        }
    }
    return checksum;
}

// Main function
int main() {
    std::string disk_map = "2333133121414131402";

    // Compact the disk
    std::vector<char> compacted_disk = compact_disk(disk_map);

    // Calculate the checksum
    long long checksum = calculate_checksum(compacted_disk);

    // Output the result
    std::cout << "Resulting filesystem checksum: " << checksum << std::endl;

    return 0;
}


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

