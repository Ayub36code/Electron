#include "Logger.h"
#include <iostream>

Logger::Logger() 
    : currentLevel(LogLevel::INFO),
      consoleOutput(true),
      fileOutput(false) {}

Logger::~Logger() {
    if(logFile.is_open()) {
        logFile.close();
    }
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::log(const std::string& message, LogLevel level) {
    if(level < currentLevel) return;
    
    std::lock_guard<std::mutex> lock(logMutex);
    
    std::string timestamp = getTimestamp();
    std::string levelStr = levelToString(level);
    std::string fullMessage = timestamp + " [" + levelStr + "] " + message;
    
    if(consoleOutput) {
        switch(level) {
            case LogLevel::INFO:
                std::cout << fullMessage << std::endl;
                break;
            case LogLevel::WARNING:
                std::cout << "\033[33m" << fullMessage << "\033[0m" << std::endl;
                break;
            case LogLevel::ERROR:
                std::cerr << "\033[31m" << fullMessage << "\033[0m" << std::endl;
                break;
            case LogLevel::DEBUG:
                std::cout << "\033[36m" << fullMessage << "\033[0m" << std::endl;
                break;
        }
    }
    
    if(fileOutput && logFile.is_open()) {
        logFile << fullMessage << std::endl;
        logFile.flush();
    }
}

void Logger::setLogFile(const std::string& filename) {
    if(logFile.is_open()) {
        logFile.close();
    }
    logFile.open(filename, std::ios::app);
    if(logFile.is_open()) {
        fileOutput = true;
        info("Logging to file: " + filename);
    } else {
        error("Failed to open log file: " + filename);
    }
}

std::string Logger::getTimestamp() {
    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

std::string Logger::levelToString(LogLevel level) {
    switch(level) {
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARN";
        case LogLevel::ERROR:   return "ERROR";
        case LogLevel::DEBUG:   return "DEBUG";
        default: return "UNKNOWN";
    }
}