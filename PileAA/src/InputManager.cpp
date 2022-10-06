#pragma once
#include "PileAA/InputManager.hpp"

namespace paa {

void InputManagement::update(PAA_ENTITY& entity, IController& controller)
{
    for (auto& [button, callback] : _isButtonPressedCallbacks) {
        if (_controller.isButtonPressed(button))
            callback(entity);
    }
    for (auto& [button, callback] : _isButtonDownCallbacks) {
        if (_controller.isButtonDown(button))
            callback(entity);
    }
    for (auto& [button, callback] : _isbButtonReleasedCallbacks) {
        if (_controller.isButtonReleased(button))
            callback(entity);
    }
    if (_callback_axis) {
        _callback_axis(entity, _controller);
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