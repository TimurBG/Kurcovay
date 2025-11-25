#include "Server.h"
#include <iostream>
#include <csignal>
#include <memory>

std::unique_ptr<Server> server;

void signalHandler(int signal) {
    if (server) {
        std::cout << "\nПолучен сигнал " << signal << ", останавливаем сервер..." << std::endl;
        server->stop();
    }
}

int main(int argc, char* argv[]) {
    // Устанавливаем обработчики сигналов
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    try {
        server = std::make_unique<Server>();
        
        if (!server->start(argc, argv)) {
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Критическая ошибка: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
