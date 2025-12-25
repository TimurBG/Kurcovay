/**
 * @file Configuration.h
 * @brief Заголовочный файл класса Configuration для управления настройками сервера
 * 
 * Содержит объявление класса Configuration, который обрабатывает параметры
 * командной строки и хранит конфигурацию сервера.
 */

#pragma once

#include <string>

/**
 * @class Configuration
 * @brief Класс для управления конфигурацией сервера
 * 
 * Предоставляет методы для парсинга параметров командной строки
 * и хранения настроек сервера.
 */
class Configuration {
private:
    std::string clientDBFile = "./etc/vcalc.conf"; ///< Файл базы клиентов
    std::string logFile = "./var/log/vcalc.log";   ///< Файл журнала
    int port = 33333;                              ///< Порт сервера

public:
    /**
     * @brief Парсит параметры командной строки
     * @param argc Количество аргументов
     * @param argv Массив аргументов
     * @return true если парсинг успешен, false в противном случае
     */
    bool parseCommandLine(int argc, char* argv[]);
    
    /**
     * @brief Получает путь к файлу базы клиентов
     * @return Путь к файлу базы клиентов
     */
    const std::string& getClientDBFile() const { return clientDBFile; }
    
    /**
     * @brief Получает путь к файлу журнала
     * @return Путь к файлу журнала
     */
    const std::string& getLogFile() const { return logFile; }
    
    /**
     * @brief Получает номер порта сервера
     * @return Номер порта
     */
    int getPort() const { return port; }
    
    /**
     * @brief Выводит справку по использованию программы
     */
    static void printHelp();
};