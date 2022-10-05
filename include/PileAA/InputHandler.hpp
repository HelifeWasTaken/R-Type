#pragma once

#include "./external/Galbar/InputHandler.hpp"
#include "VectorExtension.hpp"
#include "meta.hpp"
#include <array>
#include <memory>
#include "Types.hpp"

namespace paa {

HL_SINGLETON_IMPL(galbar::InputHandler, InputHandler);

class IController {
private:

public:
    virtual unsigned int id() const = 0;

    virtual bool isConnected() const = 0;
    virtual bool isDisconnected() const = 0;

    virtual bool isActive() const = 0;

    virtual bool isButtonPressed(const unsigned int&) const = 0;
    virtual bool isButtonDown(const unsigned int&) const = 0;
    virtual bool isButtonReleased(const unsigned int&) const = 0;

    virtual float getAxis(const Joystick::Axis&) const = 0;

    virtual Vector2f getAxisXY() const = 0;
    virtual Vector2f getAxisZR() const = 0;
    virtual Vector2f getAxisUV() const = 0;
    virtual Vector2f getAxisPOV() const = 0;
};

class ControllerJoystick : public IController {
private:
    const unsigned int _id;

public:
    ControllerJoystick(const unsigned int& id)
        : _id(id)
    {}

    unsigned int id() const override
    { return _id; }

    bool isConnected() const override
    { return InputHandler::get().isJoystickConnected(_id); }
    bool isDisconnected() const override
    { return InputHandler::get().isJoystickDisconnected(_id); }

    bool isActive() const override
    { return InputHandler::get().isJoystickActive(_id); }

    bool isButtonPressed(const unsigned int& button) const override
    { return InputHandler::get().isJoystickButtonPressed(_id, button); }
    bool isButtonDown(const unsigned int& button) const override
    { return InputHandler::get().isJoystickButtonDown(_id, button); }
    bool isButtonReleased(const unsigned int& button) const override
    { return InputHandler::get().isJoystickButtonReleased(_id, button); }

    float getAxis(const Joystick::Axis& axis) const override
    { return InputHandler::get().getJoystickAxisPosition(_id, axis); }

    Vector2f getAxisXY() const override
    { return Vector2f(getAxis(Joystick::Axis::X), getAxis(Joystick::Axis::Y)); }
    Vector2f getAxisZR() const override
    { return Vector2f(getAxis(Joystick::Axis::Z), getAxis(Joystick::Axis::R)); }
    Vector2f getAxisUV() const override
    { return Vector2f(getAxis(Joystick::Axis::U), getAxis(Joystick::Axis::V)); }
    Vector2f getAxisPOV() const override
    { return Vector2f(getAxis(Joystick::Axis::PovX), getAxis(Joystick::Axis::PovY)); }
};

class ControllerKeyboard : public IController {
private:
    std::array<Keyboard::Key, Joystick::ButtonCount>   _keys_to_button;
    std::array<Keyboard::Key, Joystick::AxisCount * 2> _keys_to_axis;

public:
    ControllerKeyboard& setKey(const unsigned int& button, const Keyboard::Key& key)
    { _keys_to_button[button] = key; return *this; }
    ControllerKeyboard& setAxis(const Joystick::Axis& axis, const Keyboard::Key& positiveAxis, const Keyboard::Key& negativeAxis)
    { _keys_to_axis[axis * 2] = positiveAxis; _keys_to_axis[axis * 2 + 1] = negativeAxis; return *this; }

    unsigned int id() const override
    { return -1; }

    bool isConnected() const override
    { return true; }
    bool isDisconnected() const override
    { return false; }
    bool isActive() const override
    { return true; }

    bool isButtonPressed(const unsigned int& button) const override
    { return InputHandler::get().isKeyPressed(_keys_to_button.at(button)); }
    bool isButtonDown(const unsigned int& button) const override
    { return InputHandler::get().isKeyDown(_keys_to_button.at(button)); }
    bool isButtonReleased(const unsigned int& button) const override
    { return InputHandler::get().isKeyReleased(_keys_to_button.at(button)); }

    float getAxis(const Joystick::Axis& axis) const override
    { return isButtonPressed(_keys_to_axis.at(axis * 2)) - isButtonPressed(_keys_to_axis.at(axis * 2 + 1)); }

    Vector2f getAxisXY() const override
    { return Vector2f(getAxis(Joystick::Axis::X), getAxis(Joystick::Axis::Y)); }
    Vector2f getAxisZR() const override
    { return Vector2f(getAxis(Joystick::Axis::Z), getAxis(Joystick::Axis::R)); }
    Vector2f getAxisUV() const override
    { return Vector2f(getAxis(Joystick::Axis::U), getAxis(Joystick::Axis::V)); }
    Vector2f getAxisPOV() const override
    { return Vector2f(getAxis(Joystick::Axis::PovX), getAxis(Joystick::Axis::PovY)); }
};

using Controller = std::shared_ptr<IController>;

}

inline std::ostream& operator<<(std::ostream& os, const paa::IController& controller)
{
    /*
    os << "IController(" << controller.id() << "):" << std::endl;
    os << "\t- isConnected: " << controller.isConnected() << std::endl;
    os << "\t- isDisconnected: " << controller.isDisconnected() << std::endl;
    os << "\t- isActive: " << controller.isActive() << std::endl;
    for (unsigned int b = 0; b < paa::Joystick::ButtonCount; ++b)
        os << "\t- Pressed(" << b << "): " << controller.isButtonPressed(b) << std::endl;
    for (unsigned int b = 0; b < paa::Joystick::ButtonCount; ++b)
        os << "\t- Down(" << b << "): " << controller.isButtonDown(b) << std::endl;
    for (unsigned int b = 0; b < paa::Joystick::ButtonCount; ++b)
        os << "\t- Released(" << b << "): " << controller.isButtonReleased(b) << std::endl;
    os << "\t- AxisXY: " << controller.getAxisXY() << std::endl;
    os << "\t- AxisZR: " << controller.getAxisZR() << std::endl;
    os << "\t- AxisUV: " << controller.getAxisUV() << std::endl;
    os << "\t- AxisPOV: " << controller.getAxisPOV();
    return os;
    */
   return os << "IController display in work";
}
