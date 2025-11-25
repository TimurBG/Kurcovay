// Configuration.cpp
#include "Configuration.h"
#include <boost/program_options.hpp>
#include <iostream>
#include <cstdlib>

namespace po = boost::program_options;

bool Configuration::parseCommandLine(int argc, char* argv[]) {
    po::options_description desc("Доступные опции");
    desc.add_options()
        ("help,h", "Показать справку")
        ("config,c", po::value<std::string>()->default_value("/etc/vcalc.conf"), 
         "Файл базы клиентов")
        ("log,l", po::value<std::string>()->default_value("/var/log/vcalc.log"), 
         "Файл журнала")
        ("port,p", po::value<int>()->default_value(33333), 
         "Порт сервера");

    po::variables_map vm;
    
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
        
        if (vm.count("help") || argc == 1) {
            printHelp();
            return false;
        }
        
        clientDBFile = vm["config"].as<std::string>();
        logFile = vm["log"].as<std::string>();
        port = vm["port"].as<int>();
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка парсинга параметров: " << e.what() << std::endl;
        printHelp();
        return false;
    }
}

void Configuration::printHelp() {
    std::cout << "Использование: vcalc_server [OPTIONS]\n\n"
              << "Опции:\n"
              << "  -h, --help          Показать эту справку\n"
              << "  -c, --config FILE   Файл базы клиентов (по умолчанию: /etc/vcalc.conf)\n"
              << "  -l, --log FILE      Файл журнала (по умолчанию: /var/log/vcalc.log)\n"
              << "  -p, --port PORT     Порт сервера (по умолчанию: 33333)\n";
}
