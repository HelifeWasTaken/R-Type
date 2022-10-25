#include "PileAA/InputHandler.hpp"
#include "poc.hpp"
#include <iostream>

void poc_joystick_example(void)
{
    paa::ControllerJoystick joystick(0);

    while (true) {
        // std::cout << joystick << std::endl;
        // std::cout << "Joystick " << joystick.getId() << " is connected: " <<
        // joystick.isConnected() << std::endl;
    }
}
