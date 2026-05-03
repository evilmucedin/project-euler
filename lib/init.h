#pragma once


class LoggingInitializer {
 public:
  static LoggingInitializer& get() {
    static LoggingInitializer instance;
    return instance;
  }

 private:
  LoggingInitializer() {
    std::cout << "Logging system initialized." << std::endl;
  }
  // Disallow copy and assign
  LoggingInitializer(const LoggingInitializer&) = delete;
  LoggingInitializer& operator=(const LoggingInitializer&) = delete;
};

void standardInit(int argc, char* argv[]);

