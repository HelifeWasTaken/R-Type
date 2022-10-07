#pragma once

#include "./external/Galbar/InputHandler.hpp"
#include "Types.hpp"
#include "meta.hpp"
#include <array>
#include <memory>
#include <ostream>

namespace paa {

HL_SINGLETON_IMPL(galbar::InputHandler, InputHandler);

enum ControllerButtons {
    BUTTON_COUNT = Joystick::ButtonCount,
    AXIS_COUNT = Joystick::AxisCount
};

class IController {
private:
public:
    /**
     * @brief Returns the id of the controller.
     *
     * @return unsigned int
     */
    virtual unsigned int id() const = 0;

    /**
     * @brief Tells if the controller is connected.
     * @return true If the controller is connected.
     */
    virtual bool isConnected() const = 0;

    /**
     * @brief Tells if the controller is disconnected.
     * @return true If the controller is disconnected.
     */
    virtual bool isDisconnected() const = 0;

    /**
     * @brief Tells if the controller is active.
     * @return true If the controller is active.
     */
    virtual bool isActive() const = 0;

    /**
     * @brief Tells if the controller button is pressed.
     * @return true If the controller button is pressed.
     */
    virtual bool isButtonPressed(const unsigned int&) const = 0;

    /**
     * @brief Tells if the controller button is down.
     * @return true If the controller button is down.
     */
    virtual bool isButtonDown(const unsigned int&) const = 0;

    /**
     * @brief Tells if the controller button is released.
     * @return true If the controller button is released.
     */
    virtual bool isButtonReleased(const unsigned int&) const = 0;

    /**
     * @brief Tells the current position of the controller axis.
     * @return from -100 to 100.
     */
    virtual float getAxis(const Joystick::Axis&) const = 0;

    /**
     * @brief Tells the current position of the controller axis in XY.
     * @return from -100 to 100. in a Vector2f.
     */
    virtual Vector2f getAxisXY() const = 0;

    /**
     * @brief Tells the current position of the controller axis in ZR.
     * @return from -100 to 100.
     */
    virtual Vector2f getAxisZR() const = 0;

    /**
     * @brief Tells the current position of the controller axis in UV.
     * @return from -100 to 100.
     */
    virtual Vector2f getAxisUV() const = 0;

    /**
     * @brief Tells the current position of the controller axis in POV.
     * @return from -100 to 100.
     */
    virtual Vector2f getAxisPOV() const = 0;

    HL_AUTO_COMPLETE_CANONICAL_FORM(IController);
};

class ControllerJoystick : public IController {
private:
    const unsigned int _id = 0;

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

    HL_AUTO_COMPLETE_CANONICAL_FORM(ControllerJoystick);
};

class ControllerKeyboard : public IController {
private:
    std::array<Keyboard::Key, Joystick::ButtonCount> _keys_to_button;
    std::array<Keyboard::Key, Joystick::AxisCount * 2> _keys_to_axis;

public:
    /**
     * @brief Set the keyboard keys to the controller.
     * @return ControllerKeyboard&
     */
    ControllerKeyboard& setKey(
        const unsigned int& button, const Keyboard::Key& key);

    /**
     * @brief Set the Axis object to the controller.
     *
     * @param axis The axis to set.
     * @param positiveAxis The positive axis key.
     * @param negativeAxis The negative axis key.
     * @return ControllerKeyboard&
     */
    ControllerKeyboard& setAxis(const Joystick::Axis& axis,
        const Keyboard::Key& positiveAxis, const Keyboard::Key& negativeAxis);

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

    HL_AUTO_COMPLETE_CANONICAL_FORM(ControllerKeyboard);
};

using Controller = std::shared_ptr<IController>;

}

/**
 * @brief Stream operator for the Controller class.
 */
std::ostream& operator<<(std::ostream& os, const paa::IController& controller);
