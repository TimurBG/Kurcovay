#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdexcept>
#include <sys/select.h>
#include <sys/time.h>
#include <cerrno>
#include "ClientDatabase.h"
#include "Authenticator.h"
#include "DataProcessor.h"
#include "Logger.h"

class ConnectionHandler {
private:
    int clientSocket;
    ClientDatabase& clientDB;
    Logger& logger;
    sockaddr_in clientAddr;
    static constexpr int TIMEOUT_SECONDS = 5;

    bool authenticateClient();
    bool processData();
    bool receiveExactly(void* buffer, size_t size);
    bool sendExactly(const void* buffer, size_t size);
    
    // Новые методы для работы с таймаутом
    bool waitForData(int timeoutSeconds = TIMEOUT_SECONDS);
    bool receiveWithTimeout(void* buffer, size_t size, int timeoutSeconds = TIMEOUT_SECONDS);
    ssize_t recvWithTimeout(void* buffer, size_t size, int timeoutSeconds = TIMEOUT_SECONDS);

public:
    ConnectionHandler(int socket, ClientDatabase& db, Logger& log, const sockaddr_in& addr);
    void handleConnection();
};