#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <sstream>

#include <lib/header.h>
#include <lib/string.h>

// Function to parse the ordering rules
std::unordered_map<int, std::unordered_set<int>> parse_rules(const std::vector<std::string>& rules) {
    std::unordered_map<int, std::unordered_set<int>> order_map;
    for (const std::string& rule : rules) {
        int x = std::stoi(rule.substr(0, rule.find('|')));
        int y = std::stoi(rule.substr(rule.find('|') + 1));
        order_map[x].insert(y);
    }
    return order_map;
}

// Function to check if an update is in correct order
bool is_correct_order(const std::vector<int>& update, const std::unordered_map<int, std::unordered_set<int>>& order_map) {
    std::unordered_set<int> seen;
    for (int page : update) {
        for (int required : order_map.at(page)) {
            if (seen.find(required) == seen.end()) {
                return false;
            }
        }
        seen.insert(page);
    }
    return true;
}

// Function to get the middle page number
int get_middle_page(const std::vector<int>& update) {
    return update[update.size() / 2];
}

// Main function
int main() {
    // Page ordering rules
    std::vector<std::string> rules = {
        "47|53", "97|13", "97|61", "97|47", "75|29", "61|13", "75|53", "29|13",
        "97|29", "53|29", "61|53", "97|53", "61|29", "47|13", "75|47", "97|75",
        "47|61", "75|61", "47|29", "75|13", "53|13"
    };

    // Updates
    std::vector<std::vector<int>> updates = {
        {75, 47, 61, 53, 29},
        {97, 61, 53, 29, 13},
        {75, 29, 13},
        {75, 97, 47, 61, 53},
        {61, 13, 29},
        {97, 13, 75, 29, 47}
    };

    // Parse rules
    auto order_map = parse_rules(rules);
    // std::cerr << order_map << std::endl;

    // Calculate the sum of middle page numbers for correctly-ordered updates
    long long int total_sum = 0;
    for (const auto& update : updates) {
        if (is_correct_order(update, order_map)) {
            total_sum += get_middle_page(update);
        }
    }

    // Output the result
    std::cout << "Sum of the middle page numbers: " << total_sum << std::endl;

    return 0;
}

