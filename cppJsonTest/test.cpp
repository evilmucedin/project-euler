#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <thread>
#include <filesystem>
#include <exception>
#include <iomanip>

// Include nlohmann/json (Ensure json.hpp is in your include path)
// #include "json.hpp"

namespace fs = std::filesystem;
// using json = nlohmann::json;

// ---------------------------------------------------------
// Helper: Extract string value from JSON
// ---------------------------------------------------------
std::string extractJsonString(const std::string& json_str, const std::string& key) {
    std::string search = "\"" + key + "\":";
    size_t pos = json_str.find(search);
    if (pos == std::string::npos) return "";

    pos = json_str.find("\"", pos + search.length());
    if (pos == std::string::npos) return "";
    pos++; // skip opening quote

    size_t end_pos = json_str.find("\"", pos);
    if (end_pos == std::string::npos) return "";

    return json_str.substr(pos, end_pos - pos);
}

// Helper: Extract number value from JSON
template<typename T>
T extractJsonNumber(const std::string& json_str, const std::string& key) {
    std::string search = "\"" + key + "\":";
    size_t pos = json_str.find(search);
    if (pos == std::string::npos) return T();

    pos += search.length();
    // Skip whitespace
    while (pos < json_str.length() && isspace(json_str[pos])) pos++;

    size_t end_pos = pos;
    while (end_pos < json_str.length() && (isdigit(json_str[end_pos]) || json_str[end_pos] == '.' || json_str[end_pos] == '-')) {
        end_pos++;
    }

    std::string num_str = json_str.substr(pos, end_pos - pos);
    if (num_str.empty()) return T();

    try {
        if constexpr (std::is_same_v<T, int>) {
            return std::stoi(num_str);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::stod(num_str);
        }
    } catch (...) {}
    return T();
}

// Helper: Extract bool value from JSON
bool extractJsonBool(const std::string& json_str, const std::string& key) {
    std::string search = "\"" + key + "\":";
    size_t pos = json_str.find(search);
    if (pos == std::string::npos) return false;

    pos += search.length();
    // Skip whitespace
    while (pos < json_str.length() && isspace(json_str[pos])) pos++;

    if (json_str.substr(pos, 4) == "true") return true;
    if (json_str.substr(pos, 5) == "false") return false;
    return false;
}

// ---------------------------------------------------------
// 1. Data Structure to hold our configuration
// ---------------------------------------------------------
struct AppConfig {
    std::string app_name;
    std::string difficulty;
    int max_players;
    double gravity;
    bool debug_mode;
    int server_port;

    // Default constructor with fallback values
    AppConfig() : app_name("Unknown"), difficulty("easy"),
                  max_players(1), gravity(1.0), debug_mode(false),
                  server_port(0) {}
};

// ---------------------------------------------------------
// 2. Function to parse JSON and update Config
// ---------------------------------------------------------
bool loadConfig(const fs::path& path, AppConfig& config) {
    try {
        std::ifstream fileStream(path);
        if (!fileStream.is_open()) {
            std::cerr << "Error: Could not open config file at " << path << std::endl;
            return false;
        }

        std::string json_content((std::istreambuf_iterator<char>(fileStream)),
                                 std::istreambuf_iterator<char>());

        // Parse JSON values using simple string extraction
        config.app_name = extractJsonString(json_content, "name");
        config.max_players = extractJsonNumber<int>(json_content, "max_players");
        config.gravity = extractJsonNumber<double>(json_content, "gravity_multiplier");
        config.debug_mode = extractJsonBool(json_content, "is_debug_mode");
        config.difficulty = extractJsonString(json_content, "difficulty_level");
        config.server_port = extractJsonNumber<int>(json_content, "server_port");

        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

// ---------------------------------------------------------
// 3. Function to simulate application behavior
// ---------------------------------------------------------
void runBehavior(const AppConfig& config) {
    std::cout << "------------------------------------------------" << std::endl;
    std::cout << ">>> BEHAVIOR SIMULATION <<<" << std::endl;
    std::cout << "Current App: " << config.app_name << std::endl;

    // Behavior depends on config values
    double calculation = config.max_players * config.gravity;

    if (config.debug_mode) {
        std::cout << "[DEBUG] Calculation raw value: " << std::fixed << std::setprecision(2) << calculation << std::endl;
        std::cout << "[DEBUG] Server Port is set to: " << config.server_port << std::endl;
    }
    else {
        // In production mode, we might hide sensitive info
        std::cout << "Status: Running safely." << std::endl;
    }

    std::cout << "Active Players Limit: " << config.max_players << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
}

// ---------------------------------------------------------
// 4. Main Entry Point
// ---------------------------------------------------------
int main() {
    // Try to find config.json in multiple locations
    fs::path CONFIG_PATH = "config.json";

    // If not in current directory, try relative to source
    if (!fs::exists(CONFIG_PATH)) {
        fs::path alt_path = "cppJsonTest/config.json";
        if (fs::exists(alt_path)) {
            CONFIG_PATH = alt_path;
        }
    }

    // Check if file exists initially
    if (!fs::exists(CONFIG_PATH)) {
        std::cerr << "Configuration file 'config.json' not found in current or cppJsonTest directories!" << std::endl;
        return 1;
    }

    AppConfig config;
    auto last_modified_time = fs::last_write_time(CONFIG_PATH);

    // Initial Load
    if (!loadConfig(CONFIG_PATH, config)) {
        std::cerr << "Failed to load initial config." << std::endl;
        return 1;
    }

    std::cout << "=== App Started. Watching '" << CONFIG_PATH.string() << "' ===" << std::endl;
    std::cout << "Edit the JSON file and save to see behavior change live." << std::endl;
    std::cout << "Press Ctrl+C to exit." << std::endl;
    std::cout << std::endl;

    // Show initial state
    runBehavior(config);

    // Infinite Loop to monitor for changes
    while (true) {
        try {
            // Get current modification time
            auto current_modified_time = fs::last_write_time(CONFIG_PATH);

            // Check if file was modified
            if (current_modified_time != last_modified_time) {
                std::cout << "\n[NOTIFICATION] Config file updated detected! Reloading..." << std::endl;

                // Update the stored timestamp
                last_modified_time = current_modified_time;

                // Reload config
                if (loadConfig(CONFIG_PATH, config)) {
                    // Re-run behavior to demonstrate change
                    runBehavior(config);
                } else {
                    std::cerr << "[ERROR] Failed to reload config. Check syntax." << std::endl;
                }
            }

        } catch (const std::system_error& e) {
            // Ignore errors if file is temporarily locked or deleted during edit
            std::cerr << "[Warning] Could not read file status: " << e.what() << std::endl;
        }

        // Sleep to avoid high CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}

