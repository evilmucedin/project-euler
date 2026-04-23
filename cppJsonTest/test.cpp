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
// 2. Function to parse JSON and update Config (simplified)
// ---------------------------------------------------------
bool loadConfig(const fs::path& path, AppConfig& config) {
    std::cerr << "Path: " << path << std::endl;
    // For now, use default config values
    // TODO: Implement JSON parsing when nlohmann/json is available
    config.app_name = "Dynamic Config App";
    config.max_players = 4;
    config.gravity = 9.8;
    config.debug_mode = true;
    config.difficulty = "normal";
    config.server_port = 8080;
    return true;
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

