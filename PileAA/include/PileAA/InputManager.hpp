#pragma once
#include "PileAA/App.hpp"
#include "PileAA/InputHandler.hpp"

namespace paa {
class InputManagement {
public:
    using callback_button_t = std::function<void(PAA_ENTITY)>;
    using callback_axis_t = std::function<void(PAA_ENTITY, IController&)>;
    /**
     * @brief  Default constructor
     * @retval None
     */
    InputManagement() = default;
    /**
     * @brief  Default destructor
     * @retval None
     */
    virtual ~InputManagement() = default;
    /**
     * @brief  Set the Button Pressed Callback object
     * @param  entity: The entity to update
     * @param  controller: The controller to use
     * @retval None
     */
    void update(PAA_ENTITY& entity, IController& controller);
    /**
     * @brief  Set the Button Pressed Callback object
     * @param  button: The button to check
     * @param  callback: The callback to call
     * @retval None
     */
    InputManagement& InputManagement::setButtonPressedCallback(
        const unsigned int& button, const callback_button_t& callback);
    /**
     * @brief  Set the Button Down Callback object
     * @param  button: The button to check
     * @param  callback: The callback to call
     * @retval None
     */
    InputManagement& InputManagement::setButtonDownCallback(
        const unsigned int& button, const callback_button_t& callback);
    /**
     * @brief  Set the Button Released Callback object
     * @param  button: The button to check
     * @param  callback: The callback to call
     * @retval None
     */
    InputManagement& InputManagement::setButtonReleasedCallback(
        const unsigned int& button, const callback_button_t& callback);
    /**
     * @brief  Set the Axis Callback object
     * @param  callback: The callback to call
     * @retval None
     */
    InputManagement& InputManagement::setAxisCallback(
        const callback_axis_t& callback);

protected:
private:
    std::unordered_map<unsigned int, callback_button_t>
        _isButtonPressedCallbacks;
    std::unordered_map<unsigned int, callback_button_t> _isButtonDownCallbacks;
    std::unordered_map<unsigned int, callback_button_t>
        _isbButtonReleasedCallbacks;

    callback_axis_t _callback_axis = nullptr;
};
}

// InputManagent manager(controller, entity);

// manager.setIsButtonPressedCallback(0,
//     [](PAA_ENTITY entity, IController& controller) {
//          entity.move(controller.getAxisXY());
//     });
