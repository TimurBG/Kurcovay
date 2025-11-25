// Logger.h
#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <chrono>
#include <iomanip>

class Logger {
private:
    std::ofstream logFile;
    std::mutex logMutex;
    std::string filename;

    std::string getCurrentTime();

public:
    bool initialize(const std::string& filename);
    void close();
    
    void logError(const std::string& message, bool critical = false);
    void logInfo(const std::string& message);
};
