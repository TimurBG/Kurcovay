// ClientDatabase.h
#pragma once

#include <string>
#include <unordered_map>
#include <fstream>

class ClientDatabase {
private:
    std::unordered_map<std::string, std::string> users; // login -> password
    std::string filename;

public:
    bool loadFromFile(const std::string& filename);
    bool saveToFile();
    bool userExists(const std::string& login) const;
    std::string getPassword(const std::string& login) const;
    bool addUser(const std::string& login, const std::string& password);
};
