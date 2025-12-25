/**
 * @file Logger.cpp
 * @brief Реализация класса Logger
 */

#include "Logger.h"
#include <iostream>

/**
 * @brief Инициализирует логгер
 * @param fname Имя файла для записи журнала
 * @return true если инициализация успешна, false в противном случае
 */
bool Logger::initialize(const std::string& fname) {
    filename = fname;
    logFile.open(filename, std::ios::app);
    return logFile.is_open();
}

/**
 * @brief Закрывает логгер
 */
void Logger::close() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

/**
 * @brief Получает текущее время в формате строки
 * @return Строка с текущим временем в формате "YYYY-MM-DD HH:MM:SS"
 */
std::string Logger::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

/**
 * @brief Записывает сообщение об ошибке
 * @param message Текст сообщения
 * @param critical Флаг критичности ошибки (true для критических ошибок)
 * 
 * Записывает сообщение в файл журнала и выводит на stderr.
 * Критические ошибки помечаются меткой "CRITICAL", обычные - "ERROR".
 */
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

/**
 * @brief Записывает информационное сообщение
 * @param message Текст сообщения
 * 
 * Записывает сообщение в файл журнала и выводит на stdout.
 */
void Logger::logInfo(const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);
    
    std::string logEntry = getCurrentTime() + " [INFO] " + message;
    
    if (logFile.is_open()) {
        logFile << logEntry << std::endl;
    }
    
    std::cout << logEntry << std::endl;
}