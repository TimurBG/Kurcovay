/**
 * @file Server.cpp
 * @brief Реализация класса Server
 */

#include "Server.h"
#include <iostream>
#include <signal.h>
#include <memory>

/**
 * @brief Конструктор класса Server
 */
Server::Server() : running(false), serverSocket(-1) {}

/**
 * @brief Деструктор класса Server
 * 
 * Автоматически останавливает сервер при уничтожении объекта.
 */
Server::~Server() {
    stop();
}

/**
 * @brief Инициализирует сокет сервера
 * @return true если инициализация успешна, false в противном случае
 * 
 * Создает сокет, устанавливает опции, привязывает к порту
 * и переводит в режим прослушивания.
 */
bool Server::initializeSocket() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        logger.logError("Ошибка создания сокета", true);
        return false;
    }
    
    // Устанавливаем опцию повторного использования адреса
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        logger.logError("Ошибка установки SO_REUSEADDR", false);
    }
    
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(config.getPort());
    
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        logger.logError("Ошибка привязки сокета к порту " + std::to_string(config.getPort()), true);
        close(serverSocket);
        return false;
    }
    
    if (listen(serverSocket, 10) < 0) {
        logger.logError("Ошибка перевода сокета в режим прослушивания", true);
        close(serverSocket);
        return false;
    }
    
    return true;
}

/**
 * @brief Принимает входящие соединения
 * 
 * Запускает бесконечный цикл приема соединений, каждое соединение
 * обрабатывается в отдельном потоке с помощью ConnectionHandler.
 */
void Server::acceptConnections() {
    while (running) {
        sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        
        int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientLen);
        
        if (clientSocket < 0) {
            if (running) {
                logger.logError("Ошибка принятия соединения", false);
            }
            continue;
        }
        
        // Обрабатываем соединение в отдельном потоке
        std::thread([this, clientSocket, clientAddr]() {
            ConnectionHandler handler(clientSocket, clientDB, logger, clientAddr);
            handler.handleConnection();
        }).detach();
    }
}

/**
 * @brief Запускает сервер
 * @param argc Количество аргументов командной строки
 * @param argv Массив аргументов командной строки
 * @return true если запуск успешен, false в противном случае
 * 
 * Последовательность запуска:
 * 1. Парсинг параметров командной строки
 * 2. Инициализация логгера
 * 3. Загрузка базы данных пользователей
 * 4. Инициализация сокета
 * 5. Запуск цикла приема соединений
 */
bool Server::start(int argc, char* argv[]) {
    // Парсим командную строку
    if (!config.parseCommandLine(argc, argv)) {
        return false;
    }
    
    // Инициализируем логгер
    if (!logger.initialize(config.getLogFile())) {
        std::cerr << "Не удалось инициализировать логгер" << std::endl;
        return false;
    }
    
    logger.logInfo("Запуск сервера...");
    
    // Загружаем базу клиентов
    if (!clientDB.loadFromFile(config.getClientDBFile())) {
        logger.logError("Не удалось загрузить базу клиентов", true);
        return false;
    }
    
    // Инициализируем сокет
    if (!initializeSocket()) {
        return false;
    }
    
    running = true;
    logger.logInfo("Сервер запущен на порту " + std::to_string(config.getPort()));
    
    // Обрабатываем соединения
    acceptConnections();
    
    return true;
}

/**
 * @brief Останавливает сервер
 * 
 * Устанавливает флаг running в false, закрывает сокет сервера
 * и логгер. Поток acceptConnections автоматически завершится.
 */
void Server::stop() {
    if (running) {
        running = false;
        logger.logInfo("Остановка сервера...");
        
        if (serverSocket != -1) {
            close(serverSocket);
            serverSocket = -1;
        }
        
        logger.close();
    }
}