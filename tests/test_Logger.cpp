#include <UnitTest++/UnitTest++.h>
#include "Logger.h"
#include <fstream>
#include <thread>
#include <vector>
#include <cstdio>
#include <sstream>
#include <iostream>

SUITE(LoggerTests) {
    TEST(InitializeSuccess) {
        Logger logger;
        bool result = logger.initialize("test_log_init.log");
        CHECK(result);
        logger.close();
        std::remove("test_log_init.log");
    }
    
    TEST(InitializeFailure) {
        Logger logger;
        // Попытка открыть файл в несуществующем каталоге
        bool result = logger.initialize("/nonexistent/path/test.log");
        CHECK(!result);
    }
    
    TEST(LogInfoWritesToFile) {
        const std::string filename = "test_info.log";
        Logger logger;
        logger.initialize(filename);
        
        std::string message = "Test info message";
        logger.logInfo(message);
        logger.close();
        
        // Проверяем, что сообщение записано в файл
        std::ifstream file(filename);
        std::string line;
        bool found = false;
        
        while (std::getline(file, line)) {
            if (line.find(message) != std::string::npos && 
                line.find("[INFO]") != std::string::npos) {
                found = true;
                break;
            }
        }
        
        CHECK(found);
        
        file.close();
        std::remove(filename.c_str());
    }
    
    TEST(LogErrorWritesToFile) {
        const std::string filename = "test_error.log";
        Logger logger;
        logger.initialize(filename);
        
        std::string message = "Test error message";
        logger.logError(message, false);
        logger.close();
        
        std::ifstream file(filename);
        std::string line;
        bool found = false;
        
        while (std::getline(file, line)) {
            if (line.find(message) != std::string::npos && 
                line.find("[ERROR]") != std::string::npos) {
                found = true;
                break;
            }
        }
        
        CHECK(found);
        
        file.close();
        std::remove(filename.c_str());
    }
    
    TEST(LogCriticalWritesToFile) {
        const std::string filename = "test_critical.log";
        Logger logger;
        logger.initialize(filename);
        
        std::string message = "Test critical message";
        logger.logError(message, true);
        logger.close();
        
        std::ifstream file(filename);
        std::string line;
        bool found = false;
        
        while (std::getline(file, line)) {
            if (line.find(message) != std::string::npos && 
                line.find("[CRITICAL]") != std::string::npos) {
                found = true;
                break;
            }
        }
        
        CHECK(found);
        
        file.close();
        std::remove(filename.c_str());
    }
    
    
}