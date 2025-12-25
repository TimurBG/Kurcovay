/**
 * @file ConnectionHandler.h
 * @brief Заголовочный файл класса ConnectionHandler для обработки клиентских соединений
 * 
 * Содержит объявление класса ConnectionHandler, который обрабатывает
 * отдельные клиентские соединения, включая аутентификацию и обработку данных.
 */

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

/**
 * @class ConnectionHandler
 * @brief Класс для обработки клиентских соединений
 * 
 * Обрабатывает отдельное клиентское соединение, включая аутентификацию,
 * прием и обработку данных, отправку результатов.
 */
class ConnectionHandler {
private:
    int clientSocket;                  ///< Сокет клиентского соединения
    ClientDatabase& clientDB;          ///< Ссылка на базу данных пользователей
    Logger& logger;                    ///< Ссылка на логгер
    sockaddr_in clientAddr;            ///< Адрес клиента
    static constexpr int TIMEOUT_SECONDS = 5; ///< Таймаут операций в секундах

    /**
     * @brief Выполняет аутентификацию клиента
     * @return true если аутентификация успешна, false в противном случае
     */
    bool authenticateClient();
    
    /**
     * @brief Обрабатывает данные от клиента
     * @return true если обработка успешна, false в противном случае
     */
    bool processData();
    
    /**
     * @brief Принимает точное количество байт
     * @param buffer Буфер для приема данных
     * @param size Количество байт для приема
     * @return true если прием успешен, false в противном случае
     */
    bool receiveExactly(void* buffer, size_t size);
    
    /**
     * @brief Отправляет точное количество байт
     * @param buffer Буфер с данными для отправки
     * @param size Количество байт для отправки
     * @return true если отправка успешна, false в противном случае
     */
    bool sendExactly(const void* buffer, size_t size);
    
    /**
     * @brief Ожидает доступности данных для чтения
     * @param timeoutSeconds Таймаут в секундах
     * @return true если данные доступны, false при таймауте или ошибке
     */
    bool waitForData(int timeoutSeconds = TIMEOUT_SECONDS);
    
    /**
     * @brief Принимает данные с таймаутом
     * @param buffer Буфер для приема данных
     * @param size Количество байт для приема
     * @param timeoutSeconds Таймаут в секундах
     * @return true если прием успешен, false в противном случае
     */
    bool receiveWithTimeout(void* buffer, size_t size, int timeoutSeconds = TIMEOUT_SECONDS);
    
    /**
     * @brief Принимает данные с таймаутом (низкоуровневый метод)
     * @param buffer Буфер для приема данных
     * @param size Максимальный размер буфера
     * @param timeoutSeconds Таймаут в секундах
     * @return Количество принятых байт или -1 при ошибке
     */
    ssize_t recvWithTimeout(void* buffer, size_t size, int timeoutSeconds = TIMEOUT_SECONDS);

public:
    /**
     * @brief Конструктор класса ConnectionHandler
     * @param socket Сокет клиентского соединения
     * @param db Ссылка на базу данных пользователей
     * @param log Ссылка на логгер
     * @param addr Адрес клиента
     */
    ConnectionHandler(int socket, ClientDatabase& db, Logger& log, const sockaddr_in& addr);
    
    /**
     * @brief Обрабатывает клиентское соединение
     * 
     * Выполняет аутентификацию и обработку данных, затем закрывает соединение.
     */
    void handleConnection();
};