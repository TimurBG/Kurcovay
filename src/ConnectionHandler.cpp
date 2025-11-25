#include "ConnectionHandler.h"
#include <iostream>
#include <vector>
#include <cstring>

ConnectionHandler::ConnectionHandler(int socket, ClientDatabase& db, Logger& log, const sockaddr_in& addr)
    : clientSocket(socket), clientDB(db), logger(log), clientAddr(addr) {}

bool ConnectionHandler::authenticateClient() {
    std::string clientIP = inet_ntoa(clientAddr.sin_addr);
    
    // Получение логина
    char loginBuffer[256];
    ssize_t received = recv(clientSocket, loginBuffer, sizeof(loginBuffer) - 1, 0);
    if (received <= 0) {
        logger.logError("Ошибка получения логина", false);
        return false;
    }
    loginBuffer[received] = '\0';
    std::string login = loginBuffer;
    
    // Удаление символов новой строки
    size_t pos = login.find('\n');
    if (pos != std::string::npos) login = login.substr(0, pos);
    pos = login.find('\r');
    if (pos != std::string::npos) login = login.substr(0, pos);
    
    if (!clientDB.userExists(login)) {
        logger.logError("Пользователь не найден: " + login, false);
        send(clientSocket, "ERR\n", 4, 0);
        return false;
    }
    
    // Генерация и отправка соли
    std::string salt = Authenticator::generateSalt();
    if (send(clientSocket, salt.c_str(), salt.length(), 0) <= 0) {
        logger.logError("Ошибка отправки соли", false);
        return false;
    }
    
    // Получение хеша
    char hashBuffer[256];
    received = recv(clientSocket, hashBuffer, sizeof(hashBuffer) - 1, 0);
    if (received <= 0) {
        logger.logError("Ошибка получения хеша", false);
        return false;
    }
    hashBuffer[received] = '\0';
    std::string receivedHash = hashBuffer;
    
    // Удаление символов новой строки
    pos = receivedHash.find('\n');
    if (pos != std::string::npos) receivedHash = receivedHash.substr(0, pos);
    pos = receivedHash.find('\r');
    if (pos != std::string::npos) receivedHash = receivedHash.substr(0, pos);
    
    // Проверка пароля
    std::string storedPassword = clientDB.getPassword(login);
    if (!Authenticator::verifyPassword(receivedHash, salt, storedPassword)) {
        logger.logError("Ошибка аутентификации для " + login, false);
        send(clientSocket, "ERR\n", 4, 0);
        return false;
    }
    
    // Успешная аутентификация
    if (send(clientSocket, "OK\n", 3, 0) <= 0) {
        logger.logError("Ошибка отправки OK", false);
        return false;
    }
    
    logger.logInfo("Успешная аутентификация: " + login);
    return true;
}

bool ConnectionHandler::receiveExactly(void* buffer, size_t size) {
    char* ptr = static_cast<char*>(buffer);
    size_t totalReceived = 0;
    
    while (totalReceived < size) {
        ssize_t received = recv(clientSocket, ptr + totalReceived, size - totalReceived, 0);
        if (received <= 0) {
            return false;
        }
        totalReceived += received;
    }
    return true;
}

bool ConnectionHandler::sendExactly(const void* buffer, size_t size) {
    const char* ptr = static_cast<const char*>(buffer);
    size_t totalSent = 0;
    
    while (totalSent < size) {
        ssize_t sent = send(clientSocket, ptr + totalSent, size - totalSent, 0);
        if (sent <= 0) {
            return false;
        }
        totalSent += sent;
    }
    return true;
}

bool ConnectionHandler::processData() {
    try {
        // Получаем количество векторов
        uint32_t numVectors;
        if (!receiveExactly(&numVectors, sizeof(numVectors))) {
            logger.logError("Ошибка получения количества векторов", false);
            return false;
        }
        
        for (uint32_t i = 0; i < numVectors; ++i) {
            // Получаем размер вектора
            uint32_t vectorSize;
            if (!receiveExactly(&vectorSize, sizeof(vectorSize))) {
                logger.logError("Ошибка получения размера вектора", false);
                return false;
            }
            
            // Получаем значения вектора
            std::vector<uint32_t> vector(vectorSize);
            for (uint32_t j = 0; j < vectorSize; ++j) {
                if (!receiveExactly(&vector[j], sizeof(vector[j]))) {
                    logger.logError("Ошибка получения значения вектора", false);
                    return false;
                }
            }
            
            // Вычисляем и отправляем результат
            uint32_t average = DataProcessor::calculateAverage(vector);
            if (!sendExactly(&average, sizeof(average))) {
                logger.logError("Ошибка отправки результата", false);
                return false;
            }
        }
        
        logger.logInfo("Успешно обработано " + std::to_string(numVectors) + " векторов");
        return true;
        
    } catch (const std::exception& e) {
        logger.logError(std::string("Ошибка в processData: ") + e.what(), false);
        return false;
    }
}

void ConnectionHandler::handleConnection() {
    std::string clientIP = inet_ntoa(clientAddr.sin_addr);
    logger.logInfo("Новое подключение от " + clientIP);
    
    try {
        if (!authenticateClient()) {
            close(clientSocket);
            return;
        }
        
        if (!processData()) {
            logger.logError("Ошибка обработки данных от " + clientIP, false);
        }
        
    } catch (const std::exception& e) {
        logger.logError("Исключение при обработке соединения: " + std::string(e.what()), false);
    }
    
    close(clientSocket);
    logger.logInfo("Соединение с " + clientIP + " закрыто");
}
