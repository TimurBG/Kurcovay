// Configuration.h
#pragma once

#include <string>

class Configuration {
private:
    std::string clientDBFile = "./etc/vcalc.conf";
    std::string logFile = "./var/log/vcalc.log";
    int port = 33333;

public:
    bool parseCommandLine(int argc, char* argv[]);
    
    // Геттеры
    const std::string& getClientDBFile() const { return clientDBFile; }
    const std::string& getLogFile() const { return logFile; }
    int getPort() const { return port; }
    
    static void printHelp();
};
