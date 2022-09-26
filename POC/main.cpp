#include <unordered_map>
#include <string>
#include <functional>
#include <iostream>
#include "poc.hpp"

static const std::unordered_map<std::string, std::function<void(void)>> POCS = {
    {"animated_sprite", poc_animated_sprite},
};

int main(int ac, char **av)
{
    if (ac == 1) {
        std::cout << "Aviable options: " << std::endl;
        for (auto& it : POCS) {
            std::cout << "- " << it.first << std::endl;
        }
        std::cout << "====" << std::endl;
    }
    for (int i = 1; av[i]; i++) {
        POCS.at(av[i])();
    }
}