#include <unordered_map>
#include <string>
#include <functional>
#include <iostream>
#include "poc.hpp"

static const std::unordered_map<std::string, std::function<void(void)>> POCS = {
    {"animated_sprite", poc_animated_sprite},
    {"ecs_example", poc_ecs_example},
    {"server_example", poc_server_example},
    {"server_write_udp", poc_server_write_tcp_example}
};

int main(int ac, char **av)
{
    if (ac == 1) {
        std::cout << "Aviable options: " << std::endl;
        for (auto& it : POCS) {
            std::cout << "\t- " << it.first << std::endl;
        }
        std::cout << "====" << std::endl;
        std::cout << "Server examples never returns" << std::endl;
    } else if (ac == 2) {
        POCS.at(av[1])();
    } else {
        std::cout << "Please specify one option" << std::endl;
    }
}
