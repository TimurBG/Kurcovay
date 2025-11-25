// Authenticator.h
#pragma once

#include <string>

// Определяем для использования MD5 (как требуется в ТЗ)
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <cryptopp/md5.h>
#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>

class Authenticator {
private:
    static constexpr int SALT_SIZE = 8; // 64 бита = 8 байт

public:
    static std::string generateSalt();
    static std::string hashPassword(const std::string& salt, const std::string& password);
    static bool verifyPassword(const std::string& passwordHash, 
                              const std::string& salt, const std::string& storedPassword);
    static std::string toHexString(const std::string& binary);
};
