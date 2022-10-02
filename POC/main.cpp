#include "poc.hpp"
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>

static const std::unordered_map<std::string, std::function<void(void)>> POCS
    = { { "animated_sprite", poc_animated_sprite },
          { "ecs_example", poc_ecs_example },
          { "server_tcp_example", poc_tcp_server_example },
          { "server_udp_example", poc_udp_server_example },
          { "server_example", poc_server_example },
          { "server_write_tcp", poc_server_write_tcp_example },
          { "joystick_example", poc_joystick_example } };

int main(int ac, char** av)
{
    if (ac == 1) {
        std::cout << "Aviable options: " << std::endl;
        for (auto& it : POCS) {
            std::cout << "\t- " << it.first << std::endl;
        }
        std::cout << "====" << std::endl;
        std::cout << "Server examples never returns" << std::endl;
    } else if (ac == 2) {
        if (POCS.find(av[1]) == POCS.end()) {
            std::cout << "Invalid option" << std::endl;
            main(1, NULL);
            return 1;
        }
        POCS.at(av[1])();
    } else {
        std::cout << "Please specify one option" << std::endl;
    }
    return 0;
}
