/**
 * @file ClientDatabase.cpp
 * @brief Реализация класса ClientDatabase
 */

#include "ClientDatabase.h"
#include <iostream>
#include <sstream>

/**
 * @brief Загружает базу пользователей из файла
 * @param fname Имя файла базы данных
 * @return true если загрузка успешна, false в противном случае
 * 
 * Формат файла: каждая строка содержит логин и пароль, разделенные двоеточием.
 * Пример: user1:password1
 */
bool ClientDatabase::loadFromFile(const std::string& fname) {
    filename = fname;
    users.clear();
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл базы: " << filename << std::endl;
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string login = line.substr(0, pos);
            std::string password = line.substr(pos + 1);
            users[login] = password;
        }
    }
    
    file.close();
    return true;
}

/**
 * @brief Проверяет существование пользователя в базе
 * @param login Логин пользователя
 * @return true если пользователь существует, false в противном случае
 */
bool ClientDatabase::userExists(const std::string& login) const {
    return users.find(login) != users.end();
}

/**
 * @brief Получает пароль пользователя
 * @param login Логин пользователя
 * @return Пароль пользователя или пустая строка если пользователь не найден
 */
std::string ClientDatabase::getPassword(const std::string& login) const {
    auto it = users.find(login);
    return (it != users.end()) ? it->second : "";
}