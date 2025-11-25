// Server.h
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

class Server {
private:
    Configuration config;
    ClientDatabase clientDB;
    Logger logger;
    std::atomic<bool> running;
    int serverSocket;
    
    bool initializeSocket();
    void acceptConnections();

public:
    Server();
    ~Server();
    
    bool start(int argc, char* argv[]);
    void stop();
};
