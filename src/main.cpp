/**
 * @file main.cpp
 * @brief Основной файл приложения сервера
 * 
 * Содержит точку входа приложения и обработчики сигналов.
 * Управляет созданием и запуском основного объекта сервера.
 */

#include "Server.h"
#include <iostream>
#include <csignal>
#include <memory>

std::unique_ptr<Server> server; ///< Указатель на основной объект сервера

/**
 * @brief Обработчик сигналов операционной системы
 * @param signal Номер полученного сигнала
 * 
 * Обрабатывает сигналы SIGINT (Ctrl+C) и SIGTERM для
 * корректного завершения работы сервера.
 */
void signalHandler(int signal) {
    if (server) {
        std::cout << "\nПолучен сигнал " << signal << ", останавливаем сервер..." << std::endl;
        server->stop();
    }
}

/**
 * @brief Точка входа приложения
 * @param argc Количество аргументов командной строки
 * @param argv Массив аргументов командной строки
 * @return Код завершения программы (0 - успех, 1 - ошибка)
 * 
 * Основная функция приложения:
 * 1. Устанавливает обработчики сигналов
 * 2. Создает объект сервера
 * 3. Запускает сервер
 * 4. Обрабатывает исключения
 */
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