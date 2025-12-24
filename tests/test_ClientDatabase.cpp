#include <UnitTest++/UnitTest++.h>
#include "ClientDatabase.h"
#include <fstream>
#include <cstdio>

SUITE(ClientDatabaseTests) {
    struct TestFixture {
        std::string testFileName = "test_db.conf";
        
        TestFixture() {
            // Создаем тестовый файл
            std::ofstream file(testFileName);
            file << "user1:password1\n";
            file << "user2:password2\n";
            file << "admin:admin123\n";
            file.close();
        }
        
        ~TestFixture() {
            // Удаляем тестовый файл
            std::remove(testFileName.c_str());
        }
    };
    
    TEST_FIXTURE(TestFixture, LoadFromFileSuccess) {
        ClientDatabase db;
        bool result = db.loadFromFile(testFileName);
        CHECK(result);
    }
    
    TEST(LoadFromFileFailure) {
        ClientDatabase db;
        bool result = db.loadFromFile("nonexistent_file.conf");
        CHECK(!result);
    }
    
    TEST_FIXTURE(TestFixture, UserExistsTrue) {
        ClientDatabase db;
        db.loadFromFile(testFileName);
        
        CHECK(db.userExists("user1"));
        CHECK(db.userExists("user2"));
        CHECK(db.userExists("admin"));
    }
    
    TEST_FIXTURE(TestFixture, UserExistsFalse) {
        ClientDatabase db;
        db.loadFromFile(testFileName);
        
        CHECK(!db.userExists("nonexistent"));
        CHECK(!db.userExists(""));
    }
    
    TEST_FIXTURE(TestFixture, GetPasswordSuccess) {
        ClientDatabase db;
        db.loadFromFile(testFileName);
        
        CHECK_EQUAL("password1", db.getPassword("user1"));
        CHECK_EQUAL("password2", db.getPassword("user2"));
        CHECK_EQUAL("admin123", db.getPassword("admin"));
    }
    
    TEST_FIXTURE(TestFixture, GetPasswordFailure) {
        ClientDatabase db;
        db.loadFromFile(testFileName);
        
        CHECK_EQUAL("", db.getPassword("nonexistent"));
        CHECK_EQUAL("", db.getPassword(""));
    }

}