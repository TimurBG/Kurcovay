#include <UnitTest++/UnitTest++.h>
#include "Authenticator.h"
#include <string>
#include <unordered_set>

SUITE(AuthenticatorTests) {
    TEST(GenerateSaltReturnsCorrectLength) {
        std::string salt = Authenticator::generateSalt();
        CHECK_EQUAL(16, salt.length()); // 8 байт = 16 hex символов
        for (char c : salt) {
            bool isHex = (c >= '0' && c <= '9') || 
                        (c >= 'A' && c <= 'F') || 
                        (c >= 'a' && c <= 'f');
            CHECK(isHex);
        }
    }
    
    TEST(SaltsAreUnique) {
        std::unordered_set<std::string> salts;
        for (int i = 0; i < 100; ++i) {
            std::string salt = Authenticator::generateSalt();
            CHECK(salts.find(salt) == salts.end());
            salts.insert(salt);
        }
    }
    
    TEST(HashPasswordProducesMD5Hash) {
        std::string salt = "0123456789ABCDEF";
        std::string password = "testpassword";
        std::string hash = Authenticator::hashPassword(salt, password);
        
        CHECK_EQUAL(32, hash.length()); // MD5 produces 32 hex characters
        for (char c : hash) {
            bool isHex = (c >= '0' && c <= '9') || 
                        (c >= 'A' && c <= 'F') || 
                        (c >= 'a' && c <= 'f');
            CHECK(isHex);
        }
    }
    
    TEST(VerifyPasswordCorrect) {
        std::string salt = "0123456789ABCDEF";
        std::string password = "testpassword";
        std::string hash = Authenticator::hashPassword(salt, password);
        
        bool result = Authenticator::verifyPassword(hash, salt, password);
        CHECK(result);
    }
    
    TEST(VerifyPasswordIncorrect) {
        std::string salt = "0123456789ABCDEF";
        std::string password = "testpassword";
        std::string wrongPassword = "wrongpassword";
        std::string hash = Authenticator::hashPassword(salt, password);
        
        bool result = Authenticator::verifyPassword(hash, salt, wrongPassword);
        CHECK(!result);
    }
    
    TEST(ToHexStringPadsCorrectly) {
        std::string binary = "test";
        std::string hex = Authenticator::toHexString(binary);
        
        CHECK_EQUAL(16, hex.length());
        CHECK(hex.substr(hex.length() - 8) == "74657374"); // "test" в hex
    }
}