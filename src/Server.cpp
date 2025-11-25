// Server.cpp
#include "Server.h"
#include <iostream>
#include <signal.h>
#include <memory>

Server::Server() : running(false), serverSocket(-1) {}

Server::~Server() {
    stop();
}

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
