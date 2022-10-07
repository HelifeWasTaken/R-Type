#include "PileAA/InputHandler.hpp"
#include "PileAA/VectorExtension.hpp"

namespace paa {

ControllerJoystick::ControllerJoystick(const unsigned int& id)
    : _id(id)
{
}

unsigned int ControllerJoystick::id() const { return _id; }

bool ControllerJoystick::isConnected() const
{
    return InputHandler::get().isJoystickConnected(_id);
}

bool ControllerJoystick::isDisconnected() const
{
    return InputHandler::get().isJoystickDisconnected(_id);
}

bool ControllerJoystick::isActive() const
{
    return InputHandler::get().isJoystickActive(_id);
}

bool ControllerJoystick::isButtonPressed(const unsigned int& button) const
{
    return InputHandler::get().isJoystickButtonPressed(_id, button);
}

bool ControllerJoystick::isButtonDown(const unsigned int& button) const
{
    return InputHandler::get().isJoystickButtonDown(_id, button);
}

bool ControllerJoystick::isButtonReleased(const unsigned int& button) const
{
    return InputHandler::get().isJoystickButtonReleased(_id, button);
}

float ControllerJoystick::getAxis(const Joystick::Axis& axis) const
{
    return InputHandler::get().getJoystickAxisPosition(_id, axis);
}

Vector2f ControllerJoystick::getAxisXY() const
{
    return Vector2f(getAxis(Joystick::Axis::X), getAxis(Joystick::Axis::Y));
}

Vector2f ControllerJoystick::getAxisZR() const
{
    return Vector2f(getAxis(Joystick::Axis::Z), getAxis(Joystick::Axis::R));
}

Vector2f ControllerJoystick::getAxisUV() const
{
    return Vector2f(getAxis(Joystick::Axis::U), getAxis(Joystick::Axis::V));
}

Vector2f ControllerJoystick::getAxisPOV() const
{
    return Vector2f(
        getAxis(Joystick::Axis::PovX), getAxis(Joystick::Axis::PovY));
}

ControllerKeyboard& ControllerKeyboard::setKey(
    const unsigned int& button, const Keyboard::Key& key)
{
    _keys_to_button[button] = key;
    return *this;
}

ControllerKeyboard& ControllerKeyboard::setAxis(const Joystick::Axis& axis,
    const Keyboard::Key& positiveAxis, const Keyboard::Key& negativeAxis)
{
    _keys_to_axis[axis * 2] = positiveAxis;
    _keys_to_axis[axis * 2 + 1] = negativeAxis;
    return *this;
}

unsigned int ControllerKeyboard::id() const { return -1; }

bool ControllerKeyboard::isConnected() const { return true; }

bool ControllerKeyboard::isDisconnected() const { return false; }

bool ControllerKeyboard::isActive() const { return true; }

bool ControllerKeyboard::isButtonPressed(const unsigned int& button) const
{
    return InputHandler::get().isKeyPressed(_keys_to_button.at(button));
}

bool ControllerKeyboard::isButtonDown(const unsigned int& button) const
{
    return InputHandler::get().isKeyDown(_keys_to_button.at(button));
}

bool ControllerKeyboard::isButtonReleased(const unsigned int& button) const
{
    return InputHandler::get().isKeyReleased(_keys_to_button.at(button));
}

float ControllerKeyboard::getAxis(const Joystick::Axis& axis) const
{
    return isButtonPressed(_keys_to_axis.at(axis * 2))
        - isButtonPressed(_keys_to_axis.at(axis * 2 + 1));
}

Vector2f ControllerKeyboard::getAxisXY() const
{
    return Vector2f(getAxis(Joystick::Axis::X), getAxis(Joystick::Axis::Y));
}

Vector2f ControllerKeyboard::getAxisZR() const
{
    return Vector2f(getAxis(Joystick::Axis::Z), getAxis(Joystick::Axis::R));
}
Vector2f ControllerKeyboard::getAxisUV() const
{
    return Vector2f(getAxis(Joystick::Axis::U), getAxis(Joystick::Axis::V));
}
Vector2f ControllerKeyboard::getAxisPOV() const
{
    return Vector2f(
        getAxis(Joystick::Axis::PovX), getAxis(Joystick::Axis::PovY));
}
}

std::ostream& operator<<(std::ostream& os, const paa::IController& controller)
{
    os << "IController(" << controller.id() << "):" << std::endl;
    os << "\t- isConnected: " << controller.isConnected() << std::endl;
    os << "\t- isDisconnected: " << controller.isDisconnected() << std::endl;
    os << "\t- isActive: " << controller.isActive() << std::endl;
    for (unsigned int b = 0; b < paa::Joystick::ButtonCount; ++b)
        os << "\t- Pressed(" << b << "): " << controller.isButtonPressed(b)
           << std::endl;
    for (unsigned int b = 0; b < paa::Joystick::ButtonCount; ++b)
        os << "\t- Down(" << b << "): " << controller.isButtonDown(b)
           << std::endl;
    for (unsigned int b = 0; b < paa::Joystick::ButtonCount; ++b)
        os << "\t- Released(" << b << "): " << controller.isButtonReleased(b)
           << std::endl;
    os << "\t- AxisXY: " << controller.getAxisXY() << std::endl;
    os << "\t- AxisZR: " << controller.getAxisZR() << std::endl;
    os << "\t- AxisUV: " << controller.getAxisUV() << std::endl;
    os << "\t- AxisPOV: " << controller.getAxisPOV();
    return os << "IController display in work";
}
