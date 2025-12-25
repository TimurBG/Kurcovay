/**
 * @file Server.h
 * @brief Заголовочный файл класса Server для управления сервером
 * 
 * Содержит объявление класса Server, который является основным классом
 * приложения и управляет всеми компонентами сервера.
 */

#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <atomic>
#include <thread>
#include <vector>
#include "Configuration.h"
#include "ClientDatabase.h"
#include "Logger.h"
#include "ConnectionHandler.h"

/**
 * @class Server
 * @brief Основной класс сервера приложения
 * 
 * Управляет инициализацией, запуском и остановкой сервера,
 * обработкой входящих соединений и координацией работы всех компонентов.
 */
class Server {
private:
    Configuration config;          ///< Конфигурация сервера
    ClientDatabase clientDB;       ///< База данных пользователей
    Logger logger;                 ///< Логгер для записи событий
    std::atomic<bool> running;     ///< Флаг работы сервера
    int serverSocket;              ///< Основной сокет сервера
    
    /**
     * @brief Инициализирует сокет сервера
     * @return true если инициализация успешна, false в противном случае
     */
    bool initializeSocket();
    
    /**
     * @brief Принимает входящие соединения
     * 
     * Запускает бесконечный цикл приема соединений, каждое соединение
     * обрабатывается в отдельном потоке.
     */
    void acceptConnections();

public:
    /**
     * @brief Конструктор класса Server
     */
    Server();
    
    /**
     * @brief Деструктор класса Server
     */
    ~Server();
    
    /**
     * @brief Запускает сервер
     * @param argc Количество аргументов командной строки
     * @param argv Массив аргументов командной строки
     * @return true если запуск успешен, false в противном случае
     */
    bool start(int argc, char* argv[]);
    
    /**
     * @brief Останавливает сервер
     */
    void stop();
};