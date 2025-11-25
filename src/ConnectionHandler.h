#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdexcept>
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

    bool authenticateClient();
    bool processData();
    bool receiveExactly(void* buffer, size_t size);
    bool sendExactly(const void* buffer, size_t size);

public:
    ConnectionHandler(int socket, ClientDatabase& db, Logger& log, const sockaddr_in& addr);
    void handleConnection();
};
