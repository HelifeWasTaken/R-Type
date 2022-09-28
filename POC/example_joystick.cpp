#include "poc.hpp"
#include "PileAA/InputHandler.hpp"
#include <iostream>

void poc_joystick_example(void)
{
    paa::Joystick joystick(0);

    while (true) {
        std::cout << joystick << std::endl;
    }
}
