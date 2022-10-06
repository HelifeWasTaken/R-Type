#include "poc.hpp"
#include "PileAA/InputHandler.hpp"
#include <iostream>

void poc_joystick_example(void)
{
    paa::ControllerJoystick joystick(0);

    while (true) {
        std::cout << joystick << std::endl;
    }
}
