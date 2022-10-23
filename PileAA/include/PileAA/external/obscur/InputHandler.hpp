#pragma once

/**
** co authored with Mattis DALLEAU (mattis.dalleau@epitech.eu) under license agreement (GPLV3)
**/

#include <array>
#include <vector>
#include <optional>
#include <queue>

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Joystick.hpp>

#include "InputTrace.hpp"
#include "Math.hpp"

namespace obscur {

    using JoystickIndex = size_t;
    using JoystickButton = size_t;
    using JoystickAxis = float;
    using JoystickAxisIndex = size_t;

    /**
     * @brief Handles input from Joysticks and Keyboard.
     */
    class InputHandler {
    public:
        /**
         * @brief The state of a button
         */
        enum KeyState { Idle, Pressed, Held, Released };

        /**
         * @brief JoystickState is a struct that contains the state of a joystick.
         */
        struct JoystickState {
            bool is_connected = false;
            KeyState buttons[sf::Joystick::ButtonCount] = { Idle };
            JoystickAxis axes[sf::Joystick::AxisCount] = { 0.0f };
        };

        static const Vector2<float> DOWN_LEFT;
        static const Vector2<float> DOWN;
        static const Vector2<float> DOWN_RIGHT;
        static const Vector2<float> RIGHT;
        static const Vector2<float> UP_RIGHT;
        static const Vector2<float> UP;
        static const Vector2<float> UP_LEFT;
        static const Vector2<float> LEFT;
        static const Vector2<float> CENTER;

    private:
        /**
         * @brief The state of the keyboard.
         */
        std::array<KeyState, sf::Keyboard::KeyCount> _keys = { Idle };

        /**
         * @brief The state of the joysticks.
         */
        std::array<JoystickState, sf::Joystick::Count> _joysticks;

        /**
         * @brief The event of the last frame.
         *
         * @tparam T The type of the value.
         */
        template <typename T> struct EventFrameInfo {
            T value;
            sf::Event::EventType event_type;
        };

        template <typename T> using EventFrameInfoList = std::vector<EventFrameInfo<T>>;

        EventFrameInfoList<sf::Keyboard::Key> _keysPrivateEvent;

        /**
         * @brief Information about the joystick (Generally used in EventFrameInfo)
         */
        struct JoystickPrivateKeyInfo {
            JoystickIndex id;
            JoystickButton button;
        };

        EventFrameInfoList<JoystickPrivateKeyInfo> _joystickPrivateEvent;

        mutable std::optional<InputTrace<sf::Keyboard::Key>> _keyTrace;
        mutable std::vector<InputTrace<sf::Keyboard::Key>> _keyTraces;

        mutable std::array<std::optional<InputTrace<JoystickButton>>, sf::Joystick::Count> _joystickTrace;
        mutable std::array<std::vector<InputTrace<JoystickButton>>, sf::Joystick::Count> _joystickTraces;

        std::array<float, sf::Joystick::Count> _deadZones = { 10.f };

        // InputList _keyInputs;
        // std::array<InputList, sf::Joystick::Count> _joystickInputs;

        /**
         * @brief Instance of the input handler.
         */
        static inline InputHandler* _instance = nullptr;

        /**
         * @brief Construct a new Input Handler object
         */
        InputHandler();

        /**
         * @brief Update the state of the keyboard to the next frame.
         */
        void updateKeysToNextState();

        /**
         * @brief Update the state of the joysticks to the next frame.
         */
        void updateJoystickToNextState();

        /**
         * @brief Update the state of the keyboard when a key is pressed.
         *
         * @param event The event that was triggered.
         */
        void eventKeyPressed(const sf::Event& event);

        /**
         * @brief Update the state of the keyboard when a key is released.
         *
         * @param event The event that was triggered.
         */
        void eventKeyReleased(const sf::Event& event);

        /**
         * @brief Update the state of the joystick when it is connected.
         *
         * @param event The event that was triggered.
         */
        void eventJoystickConnected(const sf::Event& event);

        /**
         * @brief Update the state of the joystick when it is disconnected.
         *
         * @param event The event that was triggered.
         */
        void eventJoystickDisconnected(const sf::Event& event);

        /**
         * @brief Update the state of the joystick when a button is pressed.
         *
         * @param event The event that was triggered.
         */
        void eventJoystickButtonPressed(const sf::Event& event);

        /**
         * @brief Update the state of the joystick when a button is released.
         *
         * @param event The event that was triggered.
         */
        void eventJoystickButtonReleased(const sf::Event& event);

        /**
         * @brief Update the state of the joystick when an axis is moved.
         *
         * @param event The event that was triggered.
         */
        void eventJoystickMoved(const sf::Event& event);

    public:
        /**
         * @brief Destroy the Input Handler object
         */
        ~InputHandler() = default;

        /**
         * @brief Get the Instance object
         *
         * @return InputHandler&
         */
        static InputHandler& getInstance();

        /**
         * @brief Destroy the Instance object
         */
        static void destroyInstance();

        /**
         * @brief Update the state of the keyboard and joysticks.
         */
        void update();

        /**
         * @brief Check if a keyboard or joystick is updated.
         *
         * @param event The event that was triggered.
         */
        void eventUpdate(const sf::Event& event);

        /**
         * @brief Check if a key is pressed.
         *
         * @param key The key to check.
         * @return KeyState The state of the key.
         */
        KeyState getKeyState(const sf::Keyboard::Key& key) const;

        /**
         * @brief Check if a joystick button is pressed.
         *
         * @param joystick The joystick to check.
         * @param button The button to check.
         * @return KeyState The state of the button.
         */
        KeyState getJoystickButtonState(const JoystickIndex& joystick, const JoystickButton& button) const;

        /*
        // WIP
        std::vector<Input> getKeyInputs(size_t size) const;

        std::vector<Input> getJoystickInputs(const JoystickIndex& joystick, size_t size) const;
        */

        /**
         * @brief Check if a key was just pressed
         *
         * @param joystick The joystick to check.
         * @return bool True if the key was just pressed.
         */
        bool isKeyPressed(const sf::Keyboard::Key& key) const;

        /**
         * @brief Check if a key is held
         *
         * @param key The key to check.
         * @return bool True if the key is held.
         */
        bool isKeyHeld(const sf::Keyboard::Key& key) const;

        /**
         * @brief Check if a key was just released
         *
         * @param key The key to check.
         * @return bool True if the key was just released.
         */
        bool isKeyReleased(const sf::Keyboard::Key& key) const;

        /**
         * @brief Check if a key is pressed or held.
         *
         * @param key The key to check.
         * @return bool True if the key is pressed or held.
         */
        bool isKeyPressedOrHeld(const sf::Keyboard::Key& key) const;

        /**
         * @brief Check if a combination of key was inputed.
         *
         * @param keys The list of key to check.
         * @return bool True if the combination of key is pressed.
         */
        bool isKeyCombinationInput(const std::vector<sf::Keyboard::Key>& keys) const;

        /**
         * @brief Check if a joystick button is just pressed.
         *
         * @param joystick_id The joystick to check.
         * @param button The button to check.
         * @return bool True if the button was just pressed.
         */
        bool isButtonPressed(
            const JoystickIndex& joystick_id, const JoystickButton& button) const;

        /**
         * @brief Check if a joystick button is just held.
         *
         * @param joystick_id The joystick to check.
         * @param button The button to check.
         * @return bool True if the button is held.
         */
        bool isButtonHeld(
            const JoystickIndex& joystick_id, const JoystickButton& button) const;

        /**
         * @brief Check if a joystick button is just released.
         *
         * @param joystick_id The joystick to check.
         * @param button The button to check.
         * @return bool True if the button was just released.
         */
        bool isButtonReleased(
            const JoystickIndex& joystick_id, const JoystickButton& button) const;

        /**
         * @brief Check if a joystick button is pressed or held.
         *
         * @param joystick_id The joystick to check.
         * @param button The button to check.
         * @return bool True if the button is pressed or held.
         */
        bool isButtonPressedOrHeld(
            const JoystickIndex& joystick_id, const JoystickButton& button) const;

        /**
         * @brief Check if a combination of button was inputed.
         *
         * @param joystick_id The joystick to check.
         * @param buttons The list of button to check.
         * @return bool True if the combination of button is pressed.
         */
        bool isButtonCombinationInput(
            const JoystickIndex& joystick_id, const std::vector<JoystickButton>& buttons) const;

        /**
         * @brief Set Dead Zone for a Joystick's Axis.
         *
         * @param joystick_id The joystick to set the zone.
         * @param radius The radius of the zone.
         * @return void.
         */
        void setAxisDeadZone(const JoystickIndex& joystick_id, const float &radius);

        /**
         * @brief Check if a joystick axis is moved.
         *
         * @param joystick_id The joystick to check.
         * @param axis The axis to check.
         * @return JoystickAxis The position of the axis.
         */
        JoystickAxis getAxisPosition(
            const JoystickIndex& joystick_id, const JoystickAxisIndex& axis) const;

        /**
         * @brief Get the Axis X Y object.
         *
         * @param joystick_id The joystick to check.
         * @return Vector2<float> The position of the axis.
         */
        Vector2<float> getAxisXY(const JoystickIndex& joystick_id) const;

        /**
         * @brief Get a more global Position of the X Y Axis.
         *
         * @param joystick_id The joystick to check.
         * @return Vector2<float> The position of the axis.
         */
        Vector2<float> getAxisXYGlobal(const JoystickIndex& joystick_id) const;

        /**
         * @brief Get the Axis Z R object
         *
         * @param joystick_id The joystick to check.
         * @return Vector2<float> The position of the axis.
         */
        Vector2<float> getAxisZR(const JoystickIndex& joystick_id) const;

        /**
         * @brief Get a more global Position of the Z R Axis.
         *
         * @param joystick_id The joystick to check.
         * @return Vector2<float> The position of the axis.
         */
        Vector2<float> getAxisZRGlobal(const JoystickIndex& joystick_id) const;

        /**
         * @brief Get the Axis UD LR object
         *
         * @param joystick_id The joystick to check.
         * @return Vector2<float> The position of the axis.
         */
        Vector2<float> getAxisUV(const JoystickIndex& joystick_id) const;

        /**
         * @brief Get a more global Position of the U V Axis.
         *
         * @param joystick_id The joystick to check.
         * @return Vector2<float> The position of the axis.
         */
        Vector2<float> getAxisUVGlobal(const JoystickIndex& joystick_id) const;

        /**
         * @brief Get the Axis HV object
         *
         * @param joystick_id The joystick to check.
         * @return Vector2<float> The position of the axis.
         */
        Vector2<float> getAxisPOV(const JoystickIndex& joystick_id) const;

        /**
         * @brief Tells if a joystick is connected.
         *
         * @param joystick_id The joystick to check.
         * @return bool True if the joystick is connected.
         */
        bool isJoystickConnected(const JoystickIndex& joystick_id) const;
    };

    template<typename... Args>
    class CallbackInputHandler {
        InputHandler &_handler = InputHandler::getInstance();

        std::unordered_map<sf::Keyboard::Key, std::pair<InputHandler::KeyState, std::function<void(Args...)>>> _key_callbacks;
        std::unordered_map<JoystickButton, std::pair<JoystickIndex, std::pair<InputHandler::KeyState, std::function<void(Args...)>>>> _button_callbacks;

        public:
            CallbackInputHandler() = default;
            ~CallbackInputHandler() = default;

            void addKeyCallback(const sf::Keyboard::Key& key, const InputHandler::KeyState& state, std::function<void(Args...)> &&callback) {
                _key_callbacks[key] = std::make_pair(state, callback);
            }

            void addButtonCallback(const JoystickIndex& joystick_id, const JoystickButton& button, const InputHandler::KeyState& state, std::function<void(Args...)> &&callback) {
                _button_callbacks[button] = std::make_pair(joystick_id, std::make_pair(state, callback));
            }

            void update(const Args&... args) {
                for (auto &[key, pair] : _key_callbacks) {
                    if (_handler.getKeyState(key) == pair.first)
                        pair.second(args...);
                }

                for (auto &[button, pair] : _button_callbacks) {
                    if (_handler.getJoystickButtonState(pair.first, button) == pair.second.first)
                        pair.second.second(args...);
                }
            }
    };

    /**
     * @brief All the types of controller
     *
     */
    enum ControllerType { KEYBOARD, JOYSTICK, SIMULATED };

    /**
     * @brief The controller class interface
     *
     */
    class IController {
    public:
        /**
         * @brief Construct a new IController object
         *
         */
        IController() = default;

        /**
         * @brief Destroy the IController object
         *
         */
        virtual ~IController() = default;

        /**
         * @brief Tells if the controller is connected.
         */
        virtual bool isConnected() const = 0;

        /**
         * @brief Tells whether a key is just pressed.
         *
         * @param button The button to check.
         * @return true if the button is just pressed.
         */
        virtual bool isButtonPressed(const JoystickButton& button) const = 0;

        /**
         * @brief Tells whether a key is being held.
         *
         * @param button The button to check.
         * @return true if the button is being held.
         */
        virtual bool isButtonHeld(const JoystickButton& button) const = 0;

        /**
         * @brief Tells whether a key is just released.
         *
         * @param button The button to check.
         * @return true if the button is just released.
         */
        virtual bool isButtonReleased(const JoystickButton& button) const = 0;

        /**
         * @brief Tells whether a key is pressed or held.
         *
         * @param button The button to check.
         * @return true if the button is pressed or held.
         */
        virtual bool isButtonPressedOrHeld(const JoystickButton& button) const = 0;

        /**
         * @brief Check if a combination of button was inputed.
         *
         * @param buttons The list of button to check.
         * @return bool True if the combination of button is pressed.
         */
        virtual bool isButtonCombinationInput(const std::vector<JoystickButton>& buttons) const = 0;

        // virtual bool isComboInput(const std::vector<Input> &inputs) const = 0;

        /**
         * @brief Set Dead Zone for the Joystick Axis.
         *
         * @param radius The radius of the zone.
         * @return void.
         */
        virtual void setAxisDeadZone(const float &radius) = 0;

        /**
         * @brief Get the Axis Position object
         *
         * @param axis The axis to check.
         * @return JoystickAxis The position of the axis.
         */
        virtual JoystickAxis getAxisPosition(
            const JoystickAxisIndex& axis) const = 0;

        /**
         * @brief Get the Axis X Y object
         *
         * @return Vector2<float> The position of the axis.
         */
        virtual Vector2<float> getAxisXY() const = 0;

        /**
         * @brief Get a more global Position of the X Y Axis.
         *
         * @return Vector2<float> The position of the axis.
         */
        virtual Vector2<float> getAxisXYGlobal() const = 0;

        /**
         * @brief Get the Axis Z R object
         *
         * @return Vector2<float> The position of the axis.
         */
        virtual Vector2<float> getAxisZR() const = 0;

        /**
         * @brief Get a more global Position of the Z R Axis.
         *
         * @return Vector2<float> The position of the axis.
         */
        virtual Vector2<float> getAxisZRGlobal() const = 0;

        /**
         * @brief Get the Axis UD LR object
         *
         * @return Vector2<float> The position of the axis.
         */
        virtual Vector2<float> getAxisUV() const = 0;

        /**
         * @brief Get a more global Position of the U V Axis.
         *
         * @return Vector2<float> The position of the axis.
         */
        virtual Vector2<float> getAxisUVGlobal() const = 0;

        /**
         * @brief Get the Axis HV object
         *
         * @return Vector2<float> The position of the axis.
         */
        virtual Vector2<float> getAxisPOV() const = 0;

        /**
         * @brief Get the Controller Type object
         *
         * @return ControllerType The type of the controller.
         */
        virtual const ControllerType& getType() const = 0;

        /**
         * @brief Simulate a button press.
         *        (By default the function will do nothing if the class does not
         * implement this)
         *
         */
        virtual IController& simulateButtonPress(const JoystickButton& button) = 0;

        /**
         * @brief Simulate a button held.
         *        (By default the function will do nothing if the class does not
         * implement this)
         *
         */
        virtual IController& simulateButtonHeld(const JoystickButton& button) = 0;

        /**
         * @brief Simulate a button release.
         *        (By default the function will do nothing if the class does not
         * implement this)
         *
         */
        virtual IController& simulateButtonRelease(const JoystickButton& button)
            = 0;

        /**
         * @brief Simulate an idle to the controller.
         *        (By default the function will do nothing if the class does not
         * implement this)
         *
         */
        virtual IController& simulateButtonIdle(const JoystickButton& button) = 0;

        /**
         * @brief Simulate an axis movement.
         *        (By default the function will do nothing if the class does not
         * implement this)
         *
         */
        virtual IController& simulateAxisMovement(
            const JoystickAxisIndex& axis, const JoystickAxis& value)
            = 0;

        /**
         * @brief Reset simulated infos.
         *        (By default the function will do nothing if the class does not
         * implement this)
         *
         */
        virtual IController& resetSimulatedInputs() = 0;
    };

    /**
     * @brief AController base class
     *
     */
    class AController : public IController {
    private:
        const ControllerType _type;

    public:
        /**
         * @brief Construct a new AController object
         *
         * @param type
         */
        AController(const ControllerType& type);

        /**
         * @brief Destroy the AController object
         *
         */
        virtual ~AController() override = default;

        /**
         * @brief Tells if the controller is connected.
         */
        bool isConnected() const override;

        /**
         * @brief Tells if the controller is connected.
         */
        virtual const ControllerType& getType() const override final;

        virtual IController& simulateButtonPress(
            const JoystickButton& button) override;
        virtual IController& simulateButtonHeld(
            const JoystickButton& button) override;
        virtual IController& simulateButtonRelease(
            const JoystickButton& button) override;
        virtual IController& simulateButtonIdle(
            const JoystickButton& button) override;
        virtual IController& simulateAxisMovement(
            const JoystickAxisIndex& axis, const JoystickAxis& value) override;
        virtual IController& resetSimulatedInputs() override;
    };

    /**
     * @brief A class to simulate controllers
     *
     */
    class SimulatedController final : public AController {
    private:
        std::array<InputHandler::KeyState, sf::Joystick::ButtonCount> _buttons
            = { InputHandler::Idle };
        std::array<double, sf::Joystick::AxisCount> _axes = { 0.0 };

        mutable std::optional<InputTrace<JoystickButton>> _joystickTrace;
        mutable std::vector<InputTrace<JoystickButton>> _joystickTraces;

        float _deadZone = 10.f;

    public:
        /**
         * @brief Construct a new Simulated Controller object
         */
        SimulatedController();

        /**
         * @brief Destroy the Simulated Controller object
         */
        virtual ~SimulatedController() override = default;

        /**
         * @brief Tells if the button is simulated as just pressed.
         */
        bool isButtonPressed(const JoystickButton& button) const override final;

        /**
         * @brief Tells if the button is simulated as being held.
         */
        bool isButtonHeld(const JoystickButton& button) const override final;

        /**
         * @brief Tells if the button is simulated as just released.
         */
        bool isButtonReleased(const JoystickButton& button) const override final;

        /**
         * @brief Tells if the button is simulated as just pressed or held.
         */
        bool isButtonPressedOrHeld(
            const JoystickButton& button) const override final;

        /**
         * @brief Tells if the list of button is simulated as input.
         */
        bool isButtonCombinationInput(
            const std::vector<JoystickButton>& buttons) const override final;

        // bool isComboInput(const std::vector<Input> &inputs) const override final;

        /**
         * @brief Set Dead Zone for the Joystick Axis.
         *
         * @param radius The radius of the zone.
         * @return void.
         */
        virtual void setAxisDeadZone(const float &radius) override final;

        /**
         * @brief Tells the position of the simulated axis.
         */
        JoystickAxis getAxisPosition(
            const JoystickAxisIndex& axis) const override final;

        /**
         * @brief Tells the position of the simulated axis.
         */
        Vector2<float> getAxisXY() const override final;

        /**
         * @brief Tells the global position of the simulated axis.
         */
        Vector2<float> getAxisXYGlobal() const override final;

        /**
         * @brief Tells the position of the simulated axis.
         */
        Vector2<float> getAxisZR() const override final;

        /**
         * @brief Tells the global position of the simulated axis.
         */
        Vector2<float> getAxisZRGlobal() const override final;

        /**
         * @brief Tells the position of the simulated axis.
         */
        Vector2<float> getAxisUV() const override final;

        /**
         * @brief Tells the global position of the simulated axis.
         */
        Vector2<float> getAxisUVGlobal() const override final;

        /**
         * @brief Tells the position of the simulated axis.
         */
        Vector2<float> getAxisPOV() const override final;

        /**
         * @brief Simulate a button press.
         */
        virtual IController& simulateButtonPress(
            const JoystickButton& button) override final;

        /**
         * @brief Simulate a button held.
         */
        virtual IController& simulateButtonHeld(
            const JoystickButton& button) override final;

        /**
         * @brief Simulate a button release.
         */
        virtual IController& simulateButtonRelease(
            const JoystickButton& button) override final;

        /**
         * @brief Simulate an idle to the controller.
         */
        virtual IController& simulateButtonIdle(
            const JoystickButton& button) override final;

        /**
         * @brief Simulate an axis movement.
         */
        virtual IController& simulateAxisMovement(const JoystickAxisIndex& axis,
            const JoystickAxis& value) override final;

        /**
         * @brief Reset simulated infos.
         */
        virtual IController& resetSimulatedInputs() override final;
    };

    /**
     * @brief Abstract class of the IController
     */
    class ManualController : public AController {
    protected:
        InputHandler& _input_handler = InputHandler::getInstance();

    public:
        /**
         * @brief Construct a new AController object
         *
         * @param type
         */
        ManualController(const ControllerType& type);

        /**
         * @brief Destroy the AController object
         *
         */
        virtual ~ManualController() override = default;
    };

    /**
     * @brief Joystick controller class
     *
     */
    class Joystick final : public ManualController {
    private:
        JoystickIndex _joystick_id;

    public:

        enum XboxButton {
            Xbox_A,
            Xbox_B,
            Xbox_X,
            Xbox_Y,
            Xbox_LB,
            Xbox_RB,
            Xbox_Select,
            Xbox_Start,
            Xbox_LeftStick,
            Xbox_RightStick
        };

        enum PS4Button {
            PS4_Square,
            PS4_Cross,
            PS4_Circle,
            PS4_Triangle,
            PS4_L1,
            PS4_R1,
            PS4_L2,
            PS4_R2,
            PS4_Select,
            PS4_Start,
            PS4_LeftStick,
            PS4_RightStick,
            PS4_PS,
            PS4_Pad
        };

        enum PS3Button {
            PS3_Triangle,
            PS3_Circle,
            PS3_Cross,
            PS3_Square,
            PS3_L1,
            PS3_R1,
            PS3_L2,
            PS3_R2,
            PS3_Select,
            PS3_Start,
            PS3_LeftStick,
            PS3_RightStick
        };

        enum NintendoButton {

        };

        /**
         * @brief Construct a new Joystick object
         *
         * @param joystick_id The joystick to check.
         */
        Joystick(const JoystickIndex& joystick_id);

        /**
         * @brief Destroy the Joystick object
         */
        virtual ~Joystick() override = default;

        /**
         * @brief Tells if the controller is connected.
         */
        virtual bool isConnected() const override final;

        /**
         * @brief Tells whether a key is just pressed.
         *
         * @param button The button to check.
         * @return true if the button is just pressed.
         */
        virtual bool isButtonPressed(
            const JoystickButton& button) const override final;

        /**
         * @brief Tells whether a key is being held.
         *
         * @param button The button to check.
         * @return true if the button is being held.
         */
        virtual bool isButtonHeld(
            const JoystickButton& button) const override final;

        /**
         * @brief Tells whether a key is just released.
         *
         * @param button The button to check.
         * @return true if the button is just released.
         */
        virtual bool isButtonReleased(
            const JoystickButton& button) const override final;

        /**
         * @brief Tells whether a key is pressed or held.
         *
         * @param button The button to check.
         * @return true if the button is pressed or held.
         */
        virtual bool isButtonPressedOrHeld(
            const JoystickButton& button) const override final;

        /**
         * @brief Check if a combination of button was inputed.
         *
         * @param buttons The list of button to check.
         * @return bool True if the combination of button is pressed.
         */
        virtual bool isButtonCombinationInput(
            const std::vector<JoystickButton>& buttons) const override final;

        // bool isComboInput(const std::vector<Input> &inputs) const override final;

        /**
         * @brief Set Dead Zone for the Joystick Axis.
         *
         * @param radius The radius of the zone.
         * @return void.
         */
        virtual void setAxisDeadZone(const float &radius) override final;

        /**
         * @brief Get the Axis Position object
         *
         * @param axis The axis to check.
         * @return JoystickAxis The position of the axis.
         */
        virtual JoystickAxis getAxisPosition(
            const JoystickAxisIndex& axis) const override final;

        /**
         * @brief Get the Axis X Y object
         *
         * @return Vector2<float> The position of the axis.
         */
        virtual Vector2<float> getAxisXY() const override final;

        /**
         * @brief Get the Axis X Y Global object
         *
         * @return Vector2<float> The position of the axis.
         */
        virtual Vector2<float> getAxisXYGlobal() const override final;

        /**
         * @brief Get the Axis Z R object
         *
         * @return Vector2<float> The position of the axis.
         */
        virtual Vector2<float> getAxisZR() const override final;

        /**
         * @brief Get the Axis U V Global object
         *
         * @return Vector2<float> The position of the axis.
         */
        virtual Vector2<float> getAxisZRGlobal() const override final;

        /**
         * @brief Get the Axis U V object
         *
         * @return Vector2<float> The position of the axis.
         */
        virtual Vector2<float> getAxisUV() const override final;

        /**
         * @brief Get the Axis U V Global object
         *
         * @return Vector2<float> The position of the axis.
         */
        virtual Vector2<float> getAxisUVGlobal() const override final;

        /**
         * @brief Get the Axis POV object
         *
         * @return Vector2<float> The position of the axis.
         */
        virtual Vector2<float> getAxisPOV() const override final;

        /**
         * @brief Get the Axis UD LR object
         *
         * @return Vector2<float> The position of the axis.
         */
        const JoystickIndex& getJoystickIndex() const;
    };

    /**
     * @brief Keyboard controller class
     *
     */
    class Keyboard final : public ManualController {
    private:
        std::array<sf::Keyboard::Key, sf::Joystick::ButtonCount> _buttonKeys
            = { sf::Keyboard::Unknown };
        std::array<sf::Keyboard::Key, sf::Joystick::AxisCount* 2> _axisKeys
            = { sf::Keyboard::Unknown };

        float _deadZone = 10.f;

    public:
        /**
        * @brief Construct a new Keyboard object
        */
        Keyboard();

        /**
        * @brief Destroy the Keyboard object
        */
        virtual ~Keyboard() override = default;

        /**
        * @brief Tells if the controller is connected.
        */
        Keyboard& setKey(
            const JoystickButton& button, const sf::Keyboard::Key& key);

        /**
        * @brief Tells if the controller is connected.
        */
        Keyboard& setAxis(const JoystickAxisIndex& axis,
            const sf::Keyboard::Key& positiveAxis,
            const sf::Keyboard::Key& negativeAxis);

        /**
        * @brief Tells if the controller is connected.
        */
        virtual bool isConnected() const override final;

        /**
        * @brief Tells whether a key is just pressed.
        *
        * @param button The button to check.
        * @return true if the button is just pressed.
        */
        virtual bool isButtonPressed(
            const JoystickButton& button) const override final;

        /**
        * @brief Tells whether a key is being held.
        *
        * @param button The button to check.
        * @return true if the button is being held.
        */
        virtual bool isButtonHeld(
            const JoystickButton& button) const override final;

        /**
        * @brief Tells whether a key is just released.
        *
        * @param button The button to check.
        * @return true if the button is just released.
        */
        virtual bool isButtonReleased(
            const JoystickButton& button) const override final;

        /**
        * @brief Tells whether a key is pressed or held.
        *
        * @param button The button to check.
        * @return true if the button is pressed or held.
        */
        virtual bool isButtonPressedOrHeld(
            const JoystickButton& button) const override final;

        /**
         * @brief Check if a combination of key was inputed.
         *
         * @param buttons The list of button to check.
         * @return bool True if the combination of button is pressed.
         */
        virtual bool isButtonCombinationInput(
            const std::vector<JoystickButton>& buttons) const override final;

        // bool isComboInput(const std::vector<Input> &inputs) const override final;

        /**
         * @brief Set Dead Zone for the Joystick Axis.
         *
         * @param radius The radius of the zone.
         * @return void.
         */
        virtual void setAxisDeadZone(const float &radius) override final;

        /**
        * @brief Get the Axis Position object
        *
        * @param axis The axis to check.
        * @return JoystickAxis The position of the axis.
        */
        virtual JoystickAxis getAxisPosition(
            const JoystickAxisIndex& axis) const override final;

        /**
        * @brief Get the Axis X Y object
        *
        * @return Vector2<float> The position of the axis.
        */
        virtual Vector2<float> getAxisXY() const override final;

        /**
         * @brief Get the Axis X Y Global object
         *
         * @return Vector2<float> The position of the axis.
         */
        virtual Vector2<float> getAxisXYGlobal() const override final;

        /**
        * @brief Get the Axis Z R object
        *
        * @return Vector2<float> The position of the axis.
        */
        virtual Vector2<float> getAxisZR() const override final;

        /**
         * @brief Get the Axis Z R Global object
         *
         * @return Vector2<float> The position of the axis.
         */
        virtual Vector2<float> getAxisZRGlobal() const override final;

        /**
        * @brief Get the Axis UD LR object
        *
        * @return Vector2<float> The position of the axis.
        */
        virtual Vector2<float> getAxisUV() const override final;

        /**
         * @brief Get the Axis UD LR Global object
         *
         * @return Vector2<float> The position of the axis.
         */
        virtual Vector2<float> getAxisUVGlobal() const override final;

        /**
        * @brief Get the Axis UD LR object
        *
        * @return Vector2<float> The position of the axis.
        */
        virtual Vector2<float> getAxisPOV() const override final;
    };
}
