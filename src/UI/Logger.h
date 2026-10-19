#pragma once
#include <string>
#include <fstream>
#include <mutex>
#include <chrono>
#include <iomanip>

enum class LogLevel {
    INFO,
    WARNING,
    ERROR,
    DEBUG
};

class Logger {
public:
    static Logger& getInstance();
    
    void log(const std::string& message, LogLevel level = LogLevel::INFO);
    void info(const std::string& message) { log(message, LogLevel::INFO); }
    void warning(const std::string& message) { log(message, LogLevel::WARNING); }
    void error(const std::string& message) { log(message, LogLevel::ERROR); }
    void debug(const std::string& message) { log(message, LogLevel::DEBUG); }
    
    void setLogFile(const std::string& filename);
    void setLogLevel(LogLevel level) { currentLevel = level; }
    void enableConsole(bool enable) { consoleOutput = enable; }
    
private:
    Logger();
    ~Logger();

    static std::string getTimestamp();

    static std::string levelToString(LogLevel level);
    
    std::ofstream logFile;
    std::mutex logMutex;
    LogLevel currentLevel;
    bool consoleOutput;
    bool fileOutput;
};