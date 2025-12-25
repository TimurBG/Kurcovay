/**
 * @file Authenticator.h
 * @brief Заголовочный файл класса Authenticator для аутентификации пользователей
 * 
 * Содержит объявление класса Authenticator, который отвечает за генерацию соли,
 * хеширование паролей и проверку аутентификационных данных с использованием MD5.
 */

#pragma once

#include <string>

// Определяем для использования MD5 (как требуется в ТЗ)
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <cryptopp/md5.h>
#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>

/**
 * @class Authenticator
 * @brief Класс для работы с аутентификацией пользователей
 * 
 * Предоставляет статические методы для генерации соли, хеширования паролей
 * и проверки аутентификационных данных с использованием алгоритма MD5.
 */
class Authenticator {
private:
    static constexpr int SALT_SIZE = 8; ///< Размер соли в байтах (64 бита)

public:
    /**
     * @brief Генерирует случайную соль для хеширования пароля
     * @return Строка с шестнадцатеричным представлением соли
     */
    static std::string generateSalt();
    
    /**
     * @brief Хеширует пароль с использованием соли
     * @param salt Соль для хеширования
     * @param password Пароль пользователя
     * @return Хеш пароля в шестнадцатеричном формате
     */
    static std::string hashPassword(const std::string& salt, const std::string& password);
    
    /**
     * @brief Проверяет корректность пароля
     * @param passwordHash Хеш пароля для проверки
     * @param salt Соль, использованная при хешировании
     * @param storedPassword Оригинальный пароль из базы данных
     * @return true если пароль верный, false в противном случае
     */
    static bool verifyPassword(const std::string& passwordHash, 
                              const std::string& salt, const std::string& storedPassword);
    
    /**
     * @brief Преобразует бинарные данные в шестнадцатеричную строку
     * @param binary Бинарные данные
     * @return Шестнадцатеричное представление данных
     */
    static std::string toHexString(const std::string& binary);
};