#pragma once

#include "./external/Galbar/InputHandler.hpp"
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
    ControllerJoystick(const unsigned int& id);
    unsigned int id() const override;
    bool isConnected() const override;
    bool isDisconnected() const override;
    bool isActive() const override;
    bool isButtonPressed(const unsigned int& button) const override;
    bool isButtonDown(const unsigned int& button) const override;
    bool isButtonReleased(const unsigned int& button) const override;
    float getAxis(const Joystick::Axis& axis) const override;
    Vector2f getAxisXY() const override;
    Vector2f getAxisZR() const override;
    Vector2f getAxisUV() const override;
    Vector2f getAxisPOV() const override;
};

class ControllerKeyboard : public IController {
private:
    std::array<Keyboard::Key, Joystick::ButtonCount>   _keys_to_button;
    std::array<Keyboard::Key, Joystick::AxisCount * 2> _keys_to_axis;

public:
    ControllerKeyboard& setKey(const unsigned int& button, const Keyboard::Key& key);
    ControllerKeyboard& setAxis(const Joystick::Axis& axis, const Keyboard::Key& positiveAxis, const Keyboard::Key& negativeAxis);

    unsigned int id() const override;
    bool isConnected() const override;
    bool isDisconnected() const override;
    bool isActive() const override;

    bool isButtonPressed(const unsigned int& button) const override;
    bool isButtonDown(const unsigned int& button) const override;
    bool isButtonReleased(const unsigned int& button) const override;

    float getAxis(const Joystick::Axis& axis) const override;

    Vector2f getAxisXY() const override;
    Vector2f getAxisZR() const override;
    Vector2f getAxisUV() const override;
    Vector2f getAxisPOV() const override;
};

using Controller = std::shared_ptr<IController>;

}

std::ostream& operator<<(std::ostream& os, const paa::IController& controller);
