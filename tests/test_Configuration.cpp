#include <UnitTest++/UnitTest++.h>
#include "Configuration.h"
#include <vector>
#include <string>
#include <cstring>

SUITE(ConfigurationTests) {
    TEST(HelpShortOption) {
        Configuration config;
        const char* argv[] = {"program", "-h"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        
        bool result = config.parseCommandLine(argc, const_cast<char**>(argv));
        CHECK(!result); // help должен возвращать false
    }
    
    TEST(HelpLongOption) {
        Configuration config;
        const char* argv[] = {"program", "--help"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        
        bool result = config.parseCommandLine(argc, const_cast<char**>(argv));
        CHECK(!result);
    }
    
    TEST(ValidConfiguration) {
        Configuration config;
        const char* argv[] = {"program", "-c", "test.conf", "-l", "test.log", "-p", "44444"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        
        bool result = config.parseCommandLine(argc, const_cast<char**>(argv));
        CHECK(result);
        CHECK_EQUAL("test.conf", config.getClientDBFile());
        CHECK_EQUAL("test.log", config.getLogFile());
        CHECK_EQUAL(44444, config.getPort());
    }
    
    TEST(DefaultPort) {
        Configuration config;
        const char* argv[] = {"program", "-c", "test.conf", "-l", "test.log"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        
        bool result = config.parseCommandLine(argc, const_cast<char**>(argv));
        CHECK(result);
        CHECK_EQUAL(33333, config.getPort()); // порт по умолчанию
    }
    
    TEST(PortTooLow) {
        Configuration config;
        const char* argv[] = {"program", "-p", "80"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        
        bool result = config.parseCommandLine(argc, const_cast<char**>(argv));
        CHECK(!result); // порт < 1024 должен вызывать ошибку
    }
    
    TEST(PortTooHigh) {
        Configuration config;
        const char* argv[] = {"program", "-p", "65536"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        
        bool result = config.parseCommandLine(argc, const_cast<char**>(argv));
        CHECK(!result); // порт > 65535 должен вызывать ошибку
    }
    
    TEST(PortBoundaryValues) {
        Configuration config;
        
        // Граничное значение: 1024
        {
            const char* argv[] = {"program", "-p", "1024"};
            int argc = sizeof(argv) / sizeof(argv[0]);
            bool result = config.parseCommandLine(argc, const_cast<char**>(argv));
            CHECK(result);
            CHECK_EQUAL(1024, config.getPort());
        }
        
        // Граничное значение: 65535
        {
            const char* argv[] = {"program", "-p", "65535"};
            int argc = sizeof(argv) / sizeof(argv[0]);
            bool result = config.parseCommandLine(argc, const_cast<char**>(argv));
            CHECK(result);
            CHECK_EQUAL(65535, config.getPort());
        }
    }
    
    TEST(MissingRequiredValue) {
        Configuration config;
        const char* argv[] = {"program", "-p"}; // нет значения для -p
        int argc = sizeof(argv) / sizeof(argv[0]);
        
        bool result = config.parseCommandLine(argc, const_cast<char**>(argv));
        CHECK(!result);
    }
    
    TEST(UnknownOption) {
        Configuration config;
        const char* argv[] = {"program", "-x", "value"};
        int argc = sizeof(argv) / sizeof(argv[0]);
        
        bool result = config.parseCommandLine(argc, const_cast<char**>(argv));
        CHECK(!result);
    }
    
}