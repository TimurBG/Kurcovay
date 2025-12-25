/**
 * @file ClientDatabase.h
 * @brief Заголовочный файл класса ClientDatabase для работы с базой пользователей
 * 
 * Содержит объявление класса ClientDatabase, который управляет загрузкой
 * и доступом к базе данных пользователей.
 */

#pragma once

#include <string>
#include <unordered_map>
#include <fstream>

/**
 * @class ClientDatabase
 * @brief Класс для работы с базой данных пользователей
 * 
 * Предоставляет методы для загрузки базы пользователей из файла,
 * проверки существования пользователя и получения пароля.
 */
class ClientDatabase {
private:
    std::unordered_map<std::string, std::string> users; ///< Хэш-таблица: логин -> пароль
    std::string filename; ///< Имя файла базы данных

public:
    /**
     * @brief Загружает базу пользователей из файла
     * @param filename Имя файла базы данных
     * @return true если загрузка успешна, false в противном случае
     */
    bool loadFromFile(const std::string& filename);
    
    /**
     * @brief Проверяет существование пользователя в базе
     * @param login Логин пользователя
     * @return true если пользователь существует, false в противном случае
     */
    bool userExists(const std::string& login) const;
    
    /**
     * @brief Получает пароль пользователя
     * @param login Логин пользователя
     * @return Пароль пользователя или пустая строка если пользователь не найден
     */
    std::string getPassword(const std::string& login) const;
};