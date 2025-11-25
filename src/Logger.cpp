// Logger.cpp
#include "Logger.h"
#include <iostream>

bool Logger::initialize(const std::string& fname) {
    filename = fname;
    logFile.open(filename, std::ios::app);
    return logFile.is_open();
}

void Logger::close() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

std::string Logger::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void Logger::logError(const std::string& message, bool critical) {
    std::lock_guard<std::mutex> lock(logMutex);
    
    std::string level = critical ? "CRITICAL" : "ERROR";
    std::string logEntry = getCurrentTime() + " [" + level + "] " + message;
    
    if (logFile.is_open()) {
        logFile << logEntry << std::endl;
        logFile.flush();
    }
    
    // Также выводим в stderr для немедленного отображения
    std::cerr << logEntry << std::endl;
}

void Logger::logInfo(const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);
    
    std::string logEntry = getCurrentTime() + " [INFO] " + message;
    
    if (logFile.is_open()) {
        logFile << logEntry << std::endl;
    }
    
    std::cout << logEntry << std::endl;
}
