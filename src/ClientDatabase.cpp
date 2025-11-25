// ClientDatabase.cpp
#include "ClientDatabase.h"
#include <iostream>
#include <sstream>

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

bool ClientDatabase::saveToFile() {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& [login, password] : users) {
        file << login << ":" << password << "\n";
    }
    
    file.close();
    return true;
}

bool ClientDatabase::userExists(const std::string& login) const {
    return users.find(login) != users.end();
}

std::string ClientDatabase::getPassword(const std::string& login) const {
    auto it = users.find(login);
    return (it != users.end()) ? it->second : "";
}

bool ClientDatabase::addUser(const std::string& login, const std::string& password) {
    users[login] = password;
    return saveToFile();
}
