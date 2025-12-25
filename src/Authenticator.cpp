/**
 * @file Authenticator.cpp
 * @brief Реализация класса Authenticator
 */

#include "Authenticator.h"
#include <sstream>
#include <iomanip>

/**
 * @brief Генерирует случайную соль для хеширования пароля
 * @return Строка с шестнадцатеричным представлением соли
 * 
 * Использует криптографически безопасный генератор случайных чисел
 * для создания 8-байтовой (64-битной) соли.
 */
std::string Authenticator::generateSalt() {
    CryptoPP::byte salt[SALT_SIZE];
    CryptoPP::AutoSeededRandomPool prng;
    prng.GenerateBlock(salt, SALT_SIZE);
    
    return toHexString(std::string(reinterpret_cast<char*>(salt), SALT_SIZE));
}

/**
 * @brief Хеширует пароль с использованием соли
 * @param salt Соль для хеширования
 * @param password Пароль пользователя
 * @return Хеш пароля в шестнадцатеричном формате
 * 
 * Использует алгоритм MD5 для создания хеша из конкатенации соли и пароля.
 * Результат возвращается в виде шестнадцатеричной строки.
 */
std::string Authenticator::hashPassword(const std::string& salt, const std::string& password) {
    // Используем Weak::MD5 как требует библиотека Crypto++
    CryptoPP::Weak::MD5 hash;
    std::string data = salt + password;
    std::string digest;
    
    CryptoPP::StringSource ss(data, true,
        new CryptoPP::HashFilter(hash,
            new CryptoPP::HexEncoder(
                new CryptoPP::StringSink(digest))));
    
    return digest;
}

/**
 * @brief Проверяет корректность пароля
 * @param passwordHash Хеш пароля для проверки
 * @param salt Соль, использованная при хешировании
 * @param storedPassword Оригинальный пароль из базы данных
 * @return true если пароль верный, false в противном случае
 * 
 * Вычисляет хеш от хранимого пароля с переданной солью и сравнивает
 * с предоставленным хешем. Сравнение выполняется без учета регистра.
 */
bool Authenticator::verifyPassword(const std::string& passwordHash, 
                                  const std::string& salt, const std::string& storedPassword) {
    std::string computedHash = hashPassword(salt, storedPassword);
    
    // Сравниваем без учета регистра (шестнадцатеричные цифры)
    if (computedHash.length() != passwordHash.length()) {
        return false;
    }
    
    for (size_t i = 0; i < computedHash.length(); ++i) {
        if (std::toupper(computedHash[i]) != std::toupper(passwordHash[i])) {
            return false;
        }
    }
    
    return true;
}

/**
 * @brief Преобразует бинарные данные в шестнадцатеричную строку
 * @param binary Бинарные данные
 * @return Шестнадцатеричное представление данных
 * 
 * Каждый байт преобразуется в два шестнадцатеричных символа.
 * Результат дополняется слева нулями до 16 символов.
 */
std::string Authenticator::toHexString(const std::string& binary) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    
    for (unsigned char c : binary) {
        ss << std::setw(2) << static_cast<int>(c);
    }
    
    std::string result = ss.str();
    
    // Дополняем слева нулями до 16 символов (как требует ТЗ)
    while (result.length() < 16) {
        result = "0" + result;
    }
    
    return result;
}