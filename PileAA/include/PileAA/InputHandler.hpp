#pragma once

#include "Types.hpp"
#include "external/obscur/InputHandler.hpp"
#include "meta.hpp"
#include <memory>

namespace paa {

using InputHandler = obscur::InputHandler;
using ControllerType = obscur::ControllerType;
using IController = obscur::IController;
using AController = obscur::AController;
using SimulatedController = obscur::SimulatedController;
using ManualController = obscur::ManualController;
using ControllerKeyboard = obscur::Keyboard;
using ControllerJoystick = obscur::Joystick;

using Controller = std::shared_ptr<IController>;

}
