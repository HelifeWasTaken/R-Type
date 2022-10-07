#include "PileAA/InputManager.hpp"

namespace paa {

void InputManagement::update(PAA_ENTITY entity, IController& controller)
{
    for (const auto& [button, callback] : _isButtonPressedCallbacks) {
        if (controller.isButtonPressed(button))
            callback(entity);
    }
    for (const auto& [button, callback] : _isButtonDownCallbacks) {
        if (controller.isButtonDown(button))
            callback(entity);
    }
    for (const auto& [button, callback] : _isbButtonReleasedCallbacks) {
        if (controller.isButtonReleased(button))
            callback(entity);
    }
    if (_callback_axis) {
        _callback_axis(entity, controller);
    }
}

InputManagement& InputManagement::setButtonPressedCallback(
    const unsigned int& button, const callback_button_t& callback)
{
    _isButtonPressedCallbacks[button] = callback;
    return (*this);
}

InputManagement& InputManagement::setButtonDownCallback(
    const unsigned int& button, const callback_button_t& callback)
{
    _isButtonDownCallbacks[button] = callback;
    return (*this);
}

InputManagement& InputManagement::setButtonReleasedCallback(
    const unsigned int& button, const callback_button_t& callback)
{
    _isbButtonReleasedCallbacks[button] = callback;
    return (*this);
}

InputManagement& InputManagement::setAxisCallback(
    const callback_axis_t& callback)
{
    _callback_axis = callback;
    return (*this);
}

}
