#include "ConnectionHandler.h"
#include <iostream>
#include <vector>
#include <cstring>

ConnectionHandler::ConnectionHandler(int socket, ClientDatabase& db, Logger& log, const sockaddr_in& addr)
    : clientSocket(socket), clientDB(db), logger(log), clientAddr(addr) {}

// Ожидание данных с таймаутом
bool ConnectionHandler::waitForData(int timeoutSeconds) {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(clientSocket, &readfds);
    
    struct timeval timeout;
    timeout.tv_sec = timeoutSeconds;
    timeout.tv_usec = 0;
    
    int activity = select(clientSocket + 1, &readfds, NULL, NULL, &timeout);
    
    if (activity < 0) {
        logger.logError("Ошибка select при ожидании данных: " + std::to_string(errno), false);
        return false;
    }
    
    if (activity == 0) {
        // Таймаут
        logger.logError("Таймаут ожидания данных (" + std::to_string(timeoutSeconds) + " сек)", false);
        return false;
    }
    
    return FD_ISSET(clientSocket, &readfds);
}

// Получение данных с таймаутом
ssize_t ConnectionHandler::recvWithTimeout(void* buffer, size_t size, int timeoutSeconds) {
    if (!waitForData(timeoutSeconds)) {
        return -1;
    }
    
    ssize_t received = recv(clientSocket, buffer, size, 0);
    if (received <= 0) {
        logger.logError("Ошибка recv или соединение закрыто", false);
    }
    
    return received;
}

// Получение точного количества байт с таймаутом
bool ConnectionHandler::receiveWithTimeout(void* buffer, size_t size, int timeoutSeconds) {
    char* ptr = static_cast<char*>(buffer);
    size_t totalReceived = 0;
    time_t startTime = time(nullptr);
    
    while (totalReceived < size) {
        // Проверяем общий таймаут
        if (difftime(time(nullptr), startTime) > timeoutSeconds) {
            logger.logError("Общий таймаут получения данных (" + std::to_string(timeoutSeconds) + " сек)", false);
            return false;
        }
        
        // Ждем данные с оставшимся временем
        int remainingTime = timeoutSeconds - static_cast<int>(difftime(time(nullptr), startTime));
        if (remainingTime <= 0) {
            logger.logError("Таймаут получения данных", false);
            return false;
        }
        
        if (!waitForData(remainingTime)) {
            return false;
        }
        
        ssize_t received = recv(clientSocket, ptr + totalReceived, size - totalReceived, 0);
        if (received <= 0) {
            logger.logError("Ошибка получения данных", false);
            return false;
        }
        totalReceived += received;
    }
    return true;
}

// Модифицируем существующие методы для использования таймаута
bool ConnectionHandler::receiveExactly(void* buffer, size_t size) {
    return receiveWithTimeout(buffer, size, TIMEOUT_SECONDS);
}

bool ConnectionHandler::sendExactly(const void* buffer, size_t size) {
    const char* ptr = static_cast<const char*>(buffer);
    size_t totalSent = 0;
    time_t startTime = time(nullptr);
    
    while (totalSent < size) {
        // Проверяем общий таймаут
        if (difftime(time(nullptr), startTime) > TIMEOUT_SECONDS) {
            logger.logError("Таймаут отправки данных (" + std::to_string(TIMEOUT_SECONDS) + " сек)", false);
            return false;
        }
        
        ssize_t sent = send(clientSocket, ptr + totalSent, size - totalSent, 0);
        if (sent <= 0) {
            logger.logError("Ошибка отправки данных", false);
            return false;
        }
        totalSent += sent;
    }
    return true;
}

bool ConnectionHandler::authenticateClient() {
    std::string clientIP = inet_ntoa(clientAddr.sin_addr);
    
    // Получение логина с таймаутом
    char loginBuffer[256];
    ssize_t received = recvWithTimeout(loginBuffer, sizeof(loginBuffer) - 1, TIMEOUT_SECONDS);
    if (received <= 0) {
        logger.logError("Таймаут/ошибка получения логина", false);
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
    if (!sendExactly(salt.c_str(), salt.length())) {
        logger.logError("Таймаут/ошибка отправки соли", false);
        return false;
    }
    
    // Получение хеша с таймаутом
    char hashBuffer[256];
    received = recvWithTimeout(hashBuffer, sizeof(hashBuffer) - 1, TIMEOUT_SECONDS);
    if (received <= 0) {
        logger.logError("Таймаут/ошибка получения хеша", false);
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
    if (!sendExactly("OK\n", 3)) {
        logger.logError("Таймаут/ошибка отправки OK", false);
        return false;
    }
    
    logger.logInfo("Успешная аутентификация: " + login);
    return true;
}

bool ConnectionHandler::processData() {
    try {
        // Получаем количество векторов с таймаутом
        uint32_t numVectors;
        if (!receiveExactly(&numVectors, sizeof(numVectors))) {
            logger.logError("Таймаут получения количества векторов", false);
            return false;
        }
        
        for (uint32_t i = 0; i < numVectors; ++i) {
            // Получаем размер вектора с таймаутом
            uint32_t vectorSize;
            if (!receiveExactly(&vectorSize, sizeof(vectorSize))) {
                logger.logError("Таймаут получения размера вектора", false);
                return false;
            }
            
            // Получаем значения вектора с таймаутом
            std::vector<uint32_t> vector(vectorSize);
            for (uint32_t j = 0; j < vectorSize; ++j) {
                if (!receiveExactly(&vector[j], sizeof(vector[j]))) {
                    logger.logError("Таймаут получения значения вектора", false);
                    return false;
                }
            }
            
            // Вычисляем и отправляем результат
            uint32_t average = DataProcessor::calculateAverage(vector);
            if (!sendExactly(&average, sizeof(average))) {
                logger.logError("Таймаут отправки результата", false);
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
        // Устанавливаем таймаут на сокет
        struct timeval timeout;
        timeout.tv_sec = TIMEOUT_SECONDS;
        timeout.tv_usec = 0;
        
        if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
            logger.logError("Не удалось установить SO_RCVTIMEO: " + std::to_string(errno), false);
        }
        
        if (setsockopt(clientSocket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
            logger.logError("Не удалось установить SO_SNDTIMEO: " + std::to_string(errno), false);
        }
        
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