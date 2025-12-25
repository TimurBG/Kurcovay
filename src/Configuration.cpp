/**
 * @file Configuration.cpp
 * @brief Реализация класса Configuration
 */

#include "Configuration.h"
#include <boost/program_options.hpp>
#include <iostream>
#include <cstdlib>

namespace po = boost::program_options;

/**
 * @brief Парсит параметры командной строки
 * @param argc Количество аргументов
 * @param argv Массив аргументов
 * @return true если парсинг успешен, false в противном случае
 * 
 * Поддерживаемые параметры:
 * -h, --help          Показать справку
 * -c, --config FILE   Файл базы клиентов
 * -l, --log FILE      Файл журнала
 * -p, --port PORT     Порт сервера
 */
bool Configuration::parseCommandLine(int argc, char* argv[]) {
    po::options_description desc("Доступные опции");
    desc.add_options()
        ("help,h", "Показать справку")
        ("config,c", po::value<std::string>()->default_value("./etc/vcalc.conf"), 
         "Файл базы клиентов")
        ("log,l", po::value<std::string>()->default_value("./var/log/vcalc.log"), 
         "Файл журнала")
        ("port,p", po::value<int>()->default_value(33333), 
         "Порт сервера");

    po::variables_map vm;
    
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
        
        if (vm.count("help")) {
            printHelp();
            return false;
        }
        
        if (argc == 1) {
            printHelp();
        }

        clientDBFile = vm["config"].as<std::string>();
        logFile = vm["log"].as<std::string>();
        port = vm["port"].as<int>();
        
        if (!(1024 <= port && port <= 65535)) {
            std::cerr << "Ошибка: недопустимый номер порта: " << port << std::endl;
            std::cerr << "Порт должен быть в диапазоне 1024-65535" << std::endl;
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка парсинга параметров: " << e.what() << std::endl;
        printHelp();
        return false;
    }
}

/**
 * @brief Выводит справку по использованию программы
 */
void Configuration::printHelp() {
    std::cout << "Использование: vcalc_server [OPTIONS]\n\n"
              << "Опции:\n"
              << "  -h, --help          Показать эту справку\n"
              << "  -c, --config FILE   Файл базы клиентов (по умолчанию: ./etc/vcalc.conf)\n"
              << "  -l, --log FILE      Файл журнала (по умолчанию: ./var/log/vcalc.log)\n"
              << "  -p, --port PORT     Порт сервера (по умолчанию: 33333)\n";
}