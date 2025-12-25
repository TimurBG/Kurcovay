/**
 * @file Logger.h
 * @brief Заголовочный файл класса Logger для ведения журнала событий
 * 
 * Содержит объявление класса Logger, который предоставляет методы
 * для записи информационных и ошибокных сообщений в файл и консоль.
 */

#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <chrono>
#include <iomanip>

/**
 * @class Logger
 * @brief Класс для ведения журнала событий
 * 
 * Предоставляет потокобезопасные методы для записи сообщений
 * разных уровней важности в файл и на консоль.
 */
class Logger {
private:
    std::ofstream logFile;    ///< Поток для записи в файл журнала
    std::mutex logMutex;      ///< Мьютекс для синхронизации доступа
    std::string filename;     ///< Имя файла журнала

    /**
     * @brief Получает текущее время в формате строки
     * @return Строка с текущим временем в формате "YYYY-MM-DD HH:MM:SS"
     */
    std::string getCurrentTime();

public:
    /**
     * @brief Инициализирует логгер
     * @param filename Имя файла для записи журнала
     * @return true если инициализация успешна, false в противном случае
     */
    bool initialize(const std::string& filename);
    
    /**
     * @brief Закрывает логгер
     */
    void close();
    
    /**
     * @brief Записывает сообщение об ошибке
     * @param message Текст сообщения
     * @param critical Флаг критичности ошибки (true для критических ошибок)
     */
    void logError(const std::string& message, bool critical = false);
    
    /**
     * @brief Записывает информационное сообщение
     * @param message Текст сообщения
     */
    void logInfo(const std::string& message);
};