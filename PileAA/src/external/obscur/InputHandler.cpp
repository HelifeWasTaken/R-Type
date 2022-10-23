#include "PileAA/external/obscur/InputHandler.hpp"

namespace obscur {

InputHandler::InputHandler()
{
    for (JoystickIndex i = 0; i < sf::Joystick::Count; i++) {
        _joysticks[i].is_connected = sf::Joystick::isConnected(i);
    }
}

void InputHandler::updateKeysToNextState()
{
    if (_keysPrivateEvent.empty())
        return;

    for (auto& key : _keysPrivateEvent) {
        if (key.event_type == sf::Event::KeyPressed) {
            _keys[key.value] = InputHandler::KeyState::Held;
        } else if (key.event_type == sf::Event::KeyReleased) {
            _keys[key.value] = InputHandler::KeyState::Idle;
        }
    }
    _keysPrivateEvent.clear();
}

void InputHandler::updateJoystickToNextState()
{
    if (_joystickPrivateEvent.empty())
        return;

    for (auto& joystick : _joystickPrivateEvent) {
        if (joystick.event_type == sf::Event::JoystickButtonPressed) {
            _joysticks[joystick.value.id].buttons[joystick.value.button] = Held;
        } else if (joystick.event_type == sf::Event::JoystickButtonReleased) {
            _joysticks[joystick.value.id].buttons[joystick.value.button] = Idle;
        }
    }
    _joystickPrivateEvent.clear();
}

void InputHandler::eventKeyPressed(const sf::Event& event)
{
    _keys[event.key.code] = Pressed;
    _keysPrivateEvent.push_back({ event.key.code, event.type });
    if (_keyTrace.has_value())
        _keyTrace.value().addFollower(event.key.code);
    else
        _keyTrace.emplace(event.key.code);
}

void InputHandler::eventKeyReleased(const sf::Event& event)
{
    _keys[event.key.code] = Released;
    _keysPrivateEvent.push_back({ event.key.code, event.type });
    if (_keyTrace.has_value()) {
        _keyTraces.push_back(_keyTrace.value());
        if (event.key.code == _keyTrace.value().getOrigin())
            _keyTrace.reset();
        else
            _keyTrace.value().removeFollower(event.key.code);
    }
}

void InputHandler::eventJoystickConnected(const sf::Event& event)
{
    _joysticks[event.joystickConnect.joystickId].is_connected = true;
}

void InputHandler::eventJoystickDisconnected(const sf::Event& event)
{
    _joysticks[event.joystickConnect.joystickId] = JoystickState();
}

void InputHandler::eventJoystickButtonPressed(const sf::Event& event)
{
    _joysticks[event.joystickButton.joystickId]
        .buttons[event.joystickButton.button]
        = Pressed;
    _joystickPrivateEvent.push_back(
        { { event.joystickButton.joystickId, event.joystickButton.button },
            event.type });
    if (_joystickTrace.at(event.joystickButton.joystickId).has_value())
        _joystickTrace.at(event.joystickButton.joystickId)
            .value()
            .addFollower(event.joystickButton.button);
    else
        _joystickTrace.at(event.joystickButton.joystickId)
            .emplace(event.joystickButton.button);
}

void InputHandler::eventJoystickButtonReleased(const sf::Event& event)
{
    _joysticks[event.joystickButton.joystickId]
        .buttons[event.joystickButton.button]
        = Released;
    _joystickPrivateEvent.push_back(
        { { event.joystickButton.joystickId, event.joystickButton.button },
            event.type });
    if (_joystickTrace.at(event.joystickButton.joystickId).has_value()) {
        _joystickTraces.at(event.joystickButton.joystickId)
            .push_back(
                _joystickTrace.at(event.joystickButton.joystickId).value());
        if (event.joystickButton.button
            == _joystickTrace.at(event.joystickButton.joystickId)
                   .value()
                   .getOrigin())
            _joystickTrace.at(event.joystickButton.joystickId).reset();
        else
            _joystickTrace.at(event.joystickButton.joystickId)
                .value()
                .removeFollower(event.joystickButton.button);
    }
}

void InputHandler::eventJoystickMoved(const sf::Event& event)
{
    _joysticks[event.joystickMove.joystickId].axes[event.joystickMove.axis]
        = event.joystickMove.position;
    Vector2<float> pos = CENTER;
    if (event.joystickMove.axis == sf::Joystick::Axis::X
        || event.joystickMove.axis == sf::Joystick::Axis::Y) {
        pos = getAxisXYGlobal(event.joystickMove.joystickId);
    }
    if (event.joystickMove.axis == sf::Joystick::Axis::Z
        || event.joystickMove.axis == sf::Joystick::Axis::R) {
        pos = getAxisZRGlobal(event.joystickMove.joystickId);
    }
    if (event.joystickMove.axis == sf::Joystick::Axis::U
        || event.joystickMove.axis == sf::Joystick::Axis::V) {
        pos = getAxisUVGlobal(event.joystickMove.joystickId);
    }
    if (event.joystickMove.axis == sf::Joystick::Axis::PovX
        || event.joystickMove.axis == sf::Joystick::Axis::PovY) {
        pos = getAxisPOV(event.joystickMove.joystickId);
    }
}

InputHandler& InputHandler::getInstance()
{
    if (_instance == nullptr) {
        _instance = new InputHandler();
    }
    return *_instance;
}

void InputHandler::destroyInstance()
{
    if (_instance != nullptr) {
        delete _instance;
        _instance = nullptr;
    }
}

void InputHandler::update()
{
    _instance->updateKeysToNextState();
    _instance->updateJoystickToNextState();
}

void InputHandler::eventUpdate(const sf::Event& event)
{
    switch (event.type) {
    case sf::Event::KeyPressed:
        eventKeyPressed(event);
        break;
    case sf::Event::KeyReleased:
        eventKeyReleased(event);
        break;
    case sf::Event::JoystickConnected:
        eventJoystickConnected(event);
        break;
    case sf::Event::JoystickDisconnected:
        eventJoystickDisconnected(event);
        break;
    case sf::Event::JoystickButtonPressed:
        eventJoystickButtonPressed(event);
        break;
    case sf::Event::JoystickButtonReleased:
        eventJoystickButtonReleased(event);
        break;
    case sf::Event::JoystickMoved:
        eventJoystickMoved(event);
        break;
    default:
        break;
    }
}

InputHandler::KeyState InputHandler::getKeyState(
    const sf::Keyboard::Key& key) const
{
    return _keys[key];
}

InputHandler::KeyState InputHandler::getJoystickButtonState(
    const JoystickIndex& joystick_id, const JoystickButton& button) const
{
    return _joysticks[joystick_id].buttons[button];
}

bool InputHandler::isKeyPressed(const sf::Keyboard::Key& key) const
{
    return _keys[key] == InputHandler::KeyState::Pressed;
}

bool InputHandler::isKeyHeld(const sf::Keyboard::Key& key) const
{
    return _keys[key] == InputHandler::KeyState::Held;
}

bool InputHandler::isKeyReleased(const sf::Keyboard::Key& key) const
{
    return _keys[key] == InputHandler::KeyState::Released;
}

bool InputHandler::isKeyPressedOrHeld(const sf::Keyboard::Key& key) const
{
    return isKeyPressed(key) || isKeyHeld(key);
}

bool InputHandler::isKeyCombinationInput(
    const std::vector<sf::Keyboard::Key>& keys) const
{
    for (auto iter = _keyTraces.begin(); iter != _keyTraces.end(); ++iter) {
        if (iter->checkCombination(keys)) {
            _keyTraces.erase(iter);
            return true;
        }
    }
    return false;
}

bool InputHandler::isButtonPressed(
    const JoystickIndex& joystick_id, const JoystickButton& button) const
{
    return _joysticks[joystick_id].buttons[button]
        == InputHandler::KeyState::Pressed;
}

bool InputHandler::isButtonHeld(
    const JoystickIndex& joystick_id, const JoystickButton& button) const
{
    return _joysticks[joystick_id].buttons[button]
        == InputHandler::KeyState::Held;
}

bool InputHandler::isButtonReleased(
    const JoystickIndex& joystick_id, const JoystickButton& button) const
{
    return _joysticks[joystick_id].buttons[button]
        == InputHandler::KeyState::Released;
}

bool InputHandler::isButtonPressedOrHeld(
    const JoystickIndex& joystick_id, const JoystickButton& button) const
{
    return isButtonPressed(joystick_id, button)
        || isButtonHeld(joystick_id, button);
}

bool InputHandler::isButtonCombinationInput(const JoystickIndex& joystick_id,
    const std::vector<JoystickButton>& buttons) const
{
    for (auto iter = _joystickTraces.at(joystick_id).begin();
         iter != _joystickTraces.at(joystick_id).end(); ++iter) {
        if (iter->checkCombination(buttons)) {
            _joystickTraces.at(joystick_id).erase(iter);
            return true;
        }
    }
    return false;
}

void InputHandler::setAxisDeadZone(
    const JoystickIndex& joystick_id, const float& radius)
{
    _deadZones[joystick_id] = radius < 0.f ? 0.f : radius;
}

JoystickAxis InputHandler::getAxisPosition(
    const JoystickIndex& joystick_id, const JoystickAxisIndex& axis) const
{
    float zone = _deadZones.at(joystick_id);
    if (_joysticks[joystick_id].axes[axis] < -zone)
        return _joysticks[joystick_id].axes[axis];
    else if (_joysticks[joystick_id].axes[axis] > zone)
        return _joysticks[joystick_id].axes[axis];
    return 0.f;
}

Vector2<float> InputHandler::getAxisXY(const JoystickIndex& joystick_id) const
{
    return Vector2<float>(getAxisPosition(joystick_id, sf::Joystick::Axis::X),
        getAxisPosition(joystick_id, sf::Joystick::Axis::Y));
}

Vector2<float> InputHandler::getAxisXYGlobal(
    const JoystickIndex& joystick_id) const
{
    auto pos = getAxisXY(joystick_id);
    if (pos.x() == 0.f && pos.y() == 0.f)
        return Vector2<float>(0.f, 0.f);
    pos = Vector2<float>(pos.x(), -pos.y());
    double angle = std::atan2(pos.y(), pos.x()) * 180.f / obscur::PI + 180.f;
    if (isInRange<double, false>(angle, 22.5f, 67.5f))
        return Vector2<float>(-100.f, 100.f);
    else if (isInRange<double, false>(angle, 67.5f, 112.5f))
        return Vector2<float>(0.f, 100.f);
    else if (isInRange<double, false>(angle, 112.5f, 157.5f))
        return Vector2<float>(100.f, 100.f);
    else if (isInRange<double, false>(angle, 157.5f, 202.5f))
        return Vector2<float>(100.f, 0.f);
    else if (isInRange<double, false>(angle, 202.5f, 247.5f))
        return Vector2<float>(100.f, -100.f);
    else if (isInRange<double, false>(angle, 247.5f, 292.5f))
        return Vector2<float>(0.f, -100.f);
    else if (isInRange<double, false>(angle, 292.5f, 337.5f))
        return Vector2<float>(-100.f, -100.f);
    return Vector2<float>(-100.f, 0.f); // don't need to check the last range
}

Vector2<float> InputHandler::getAxisZR(const JoystickIndex& joystick_id) const
{
    return Vector2<float>(getAxisPosition(joystick_id, sf::Joystick::Axis::Z),
        getAxisPosition(joystick_id, sf::Joystick::Axis::R));
}

Vector2<float> InputHandler::getAxisZRGlobal(
    const JoystickIndex& joystick_id) const
{
    auto pos = getAxisZR(joystick_id);
    if (pos.x() == 0.f && pos.y() == 0.f)
        return Vector2<float>(0.f, 0.f);
    pos = Vector2<float>(pos.x(), -pos.y());
    double angle = std::atan2(pos.y(), pos.x()) * 180.f / obscur::PI + 180.f;
    if (isInRange<double, false>(angle, 22.5f, 67.5f))
        return Vector2<float>(-100.f, 100.f);
    else if (isInRange<double, false>(angle, 67.5f, 112.5f))
        return Vector2<float>(0.f, 100.f);
    else if (isInRange<double, false>(angle, 112.5f, 157.5f))
        return Vector2<float>(100.f, 100.f);
    else if (isInRange<double, false>(angle, 157.5f, 202.5f))
        return Vector2<float>(100.f, 0.f);
    else if (isInRange<double, false>(angle, 202.5f, 247.5f))
        return Vector2<float>(100.f, -100.f);
    else if (isInRange<double, false>(angle, 247.5f, 292.5f))
        return Vector2<float>(0.f, -100.f);
    else if (isInRange<double, false>(angle, 292.5f, 337.5f))
        return Vector2<float>(-100.f, -100.f);
    return Vector2<float>(-100.f, 0.f); // don't need to check the last range
}

Vector2<float> InputHandler::getAxisUV(const JoystickIndex& joystick_id) const
{
    return Vector2<float>(getAxisPosition(joystick_id, sf::Joystick::Axis::U),
        getAxisPosition(joystick_id, sf::Joystick::Axis::V));
}

Vector2<float> InputHandler::getAxisUVGlobal(
    const JoystickIndex& joystick_id) const
{
    auto pos = getAxisUV(joystick_id);
    if (pos.x() == 0.f && pos.y() == 0.f)
        return Vector2<float>(0.f, 0.f);
    pos = Vector2<float>(pos.x(), -pos.y());
    double angle = std::atan2(pos.y(), pos.x()) * 180.f / obscur::PI + 180.f;
    if (isInRange<double, false>(angle, 22.5f, 67.5f))
        return Vector2<float>(-100.f, 100.f);
    else if (isInRange<double, false>(angle, 67.5f, 112.5f))
        return Vector2<float>(0.f, 100.f);
    else if (isInRange<double, false>(angle, 112.5f, 157.5f))
        return Vector2<float>(100.f, 100.f);
    else if (isInRange<double, false>(angle, 157.5f, 202.5f))
        return Vector2<float>(100.f, 0.f);
    else if (isInRange<double, false>(angle, 202.5f, 247.5f))
        return Vector2<float>(100.f, -100.f);
    else if (isInRange<double, false>(angle, 247.5f, 292.5f))
        return Vector2<float>(0.f, -100.f);
    else if (isInRange<double, false>(angle, 292.5f, 337.5f))
        return Vector2<float>(-100.f, -100.f);
    return Vector2<float>(-100.f, 0.f); // don't need to check the last range
}

Vector2<float> InputHandler::getAxisPOV(const JoystickIndex& joystick_id) const
{
    return Vector2<float>(
        getAxisPosition(joystick_id, sf::Joystick::Axis::PovX),
        getAxisPosition(joystick_id, sf::Joystick::Axis::PovY));
}

bool InputHandler::isJoystickConnected(const JoystickIndex& joystick_id) const
{
    return _joysticks[joystick_id].is_connected;
}

const Vector2<float> InputHandler::DOWN_LEFT = Vector2<float>(-100.f, 100.f);
const Vector2<float> InputHandler::DOWN = Vector2<float>(0.f, 100.f);
const Vector2<float> InputHandler::DOWN_RIGHT = Vector2<float>(100.f, 100.f);
const Vector2<float> InputHandler::RIGHT = Vector2<float>(100.f, 0.f);
const Vector2<float> InputHandler::UP_RIGHT = Vector2<float>(100.f, -100.f);
const Vector2<float> InputHandler::UP = Vector2<float>(0.f, -100.f);
const Vector2<float> InputHandler::UP_LEFT = Vector2<float>(-100.f, -100.f);
const Vector2<float> InputHandler::LEFT = Vector2<float>(-100.f, 0.f);
const Vector2<float> InputHandler::CENTER = Vector2<float>(0.f, 0.f);

AController::AController(const ControllerType& type)
    : _type(type)
{
}

bool AController::isConnected() const { return true; }

const ControllerType& AController::getType() const { return _type; }

IController& AController::simulateButtonPress(const JoystickButton& button)
{
    return *this;
}

IController& AController::simulateButtonHeld(const JoystickButton& button)
{
    return *this;
}

IController& AController::simulateButtonRelease(const JoystickButton& button)
{
    return *this;
}

IController& AController::simulateButtonIdle(const JoystickButton& button)
{
    return *this;
}

IController& AController::simulateAxisMovement(
    const JoystickAxisIndex& axis, const JoystickAxis& value)
{
    return *this;
}

IController& AController::resetSimulatedInputs() { return *this; }

SimulatedController::SimulatedController()
    : AController(ControllerType::SIMULATED)
{
}

bool SimulatedController::isButtonPressed(const JoystickButton& button) const
{
    return _buttons[button] == InputHandler::KeyState::Pressed;
}

bool SimulatedController::isButtonHeld(const JoystickButton& button) const
{
    return _buttons[button] == InputHandler::KeyState::Held;
}

bool SimulatedController::isButtonReleased(const JoystickButton& button) const
{
    return _buttons[button] == InputHandler::KeyState::Released;
}

bool SimulatedController::isButtonPressedOrHeld(
    const JoystickButton& button) const
{
    return isButtonPressed(button) || isButtonHeld(button);
}

bool SimulatedController::isButtonCombinationInput(
    const std::vector<JoystickButton>& buttons) const
{
    for (auto iter = _joystickTraces.begin(); iter != _joystickTraces.end();
         ++iter) {
        if (iter->checkCombination(buttons)) {
            _joystickTraces.erase(iter);
            return true;
        }
    }
    return false;
}

/*bool SimulatedController::isComboInput(const std::vector<Input> &inputs) const
{ return true;
}*/

void SimulatedController::setAxisDeadZone(const float& radius)
{
    _deadZone = radius < 0.f ? 0.f : radius;
}

JoystickAxis SimulatedController::getAxisPosition(
    const JoystickAxisIndex& axis) const
{
    float zone = _deadZone;
    if (_axes[axis] < -zone)
        return _axes[axis];
    else if (_axes[axis] > zone)
        return _axes[axis];
    return 0.f;
}

Vector2<float> SimulatedController::getAxisXY() const
{
    return Vector2<float>(
        _axes[sf::Joystick::Axis::X], _axes[sf::Joystick::Axis::Y]);
}

Vector2<float> SimulatedController::getAxisXYGlobal() const
{
    auto pos = getAxisXY();
    if (pos.x() == 0.f && pos.y() == 0.f)
        return Vector2<float>(0.f, 0.f);
    pos = Vector2<float>(pos.x(), -pos.y());
    double angle = std::atan2(pos.y(), pos.x()) * 180.f / obscur::PI + 180.f;
    if (isInRange<double, false>(angle, 22.5f, 67.5f))
        return Vector2<float>(-100.f, 100.f);
    else if (isInRange<double, false>(angle, 67.5f, 112.5f))
        return Vector2<float>(0.f, 100.f);
    else if (isInRange<double, false>(angle, 112.5f, 157.5f))
        return Vector2<float>(100.f, 100.f);
    else if (isInRange<double, false>(angle, 157.5f, 202.5f))
        return Vector2<float>(100.f, 0.f);
    else if (isInRange<double, false>(angle, 202.5f, 247.5f))
        return Vector2<float>(100.f, -100.f);
    else if (isInRange<double, false>(angle, 247.5f, 292.5f))
        return Vector2<float>(0.f, -100.f);
    else if (isInRange<double, false>(angle, 292.5f, 337.5f))
        return Vector2<float>(-100.f, -100.f);
    return Vector2<float>(-100.f, 0.f); // don't need to check the last range
}

Vector2<float> SimulatedController::getAxisZR() const
{
    return Vector2<float>(
        _axes[sf::Joystick::Axis::Z], _axes[sf::Joystick::Axis::R]);
}

Vector2<float> SimulatedController::getAxisZRGlobal() const
{
    auto pos = getAxisZR();
    if (pos.x() == 0.f && pos.y() == 0.f)
        return Vector2<float>(0.f, 0.f);
    pos = Vector2<float>(pos.x(), -pos.y());
    double angle = std::atan2(pos.y(), pos.x()) * 180.f / obscur::PI + 180.f;
    if (isInRange<double, false>(angle, 22.5f, 67.5f))
        return Vector2<float>(-100.f, 100.f);
    else if (isInRange<double, false>(angle, 67.5f, 112.5f))
        return Vector2<float>(0.f, 100.f);
    else if (isInRange<double, false>(angle, 112.5f, 157.5f))
        return Vector2<float>(100.f, 100.f);
    else if (isInRange<double, false>(angle, 157.5f, 202.5f))
        return Vector2<float>(100.f, 0.f);
    else if (isInRange<double, false>(angle, 202.5f, 247.5f))
        return Vector2<float>(100.f, -100.f);
    else if (isInRange<double, false>(angle, 247.5f, 292.5f))
        return Vector2<float>(0.f, -100.f);
    else if (isInRange<double, false>(angle, 292.5f, 337.5f))
        return Vector2<float>(-100.f, -100.f);
    return Vector2<float>(-100.f, 0.f); // don't need to check the last range
}

Vector2<float> SimulatedController::getAxisUV() const
{
    return Vector2<float>(
        _axes[sf::Joystick::Axis::U], _axes[sf::Joystick::Axis::V]);
}

Vector2<float> SimulatedController::getAxisUVGlobal() const
{
    auto pos = getAxisUV();
    if (pos.x() == 0.f && pos.y() == 0.f)
        return Vector2<float>(0.f, 0.f);
    pos = Vector2<float>(pos.x(), -pos.y());
    double angle = std::atan2(pos.y(), pos.x()) * 180.f / obscur::PI + 180.f;
    if (isInRange<double, false>(angle, 22.5f, 67.5f))
        return Vector2<float>(-100.f, 100.f);
    else if (isInRange<double, false>(angle, 67.5f, 112.5f))
        return Vector2<float>(0.f, 100.f);
    else if (isInRange<double, false>(angle, 112.5f, 157.5f))
        return Vector2<float>(100.f, 100.f);
    else if (isInRange<double, false>(angle, 157.5f, 202.5f))
        return Vector2<float>(100.f, 0.f);
    else if (isInRange<double, false>(angle, 202.5f, 247.5f))
        return Vector2<float>(100.f, -100.f);
    else if (isInRange<double, false>(angle, 247.5f, 292.5f))
        return Vector2<float>(0.f, -100.f);
    else if (isInRange<double, false>(angle, 292.5f, 337.5f))
        return Vector2<float>(-100.f, -100.f);
    return Vector2<float>(-100.f, 0.f); // don't need to check the last range
}

Vector2<float> SimulatedController::getAxisPOV() const
{
    return Vector2<float>(
        _axes[sf::Joystick::Axis::PovX], _axes[sf::Joystick::Axis::PovY]);
}

IController& SimulatedController::simulateButtonPress(
    const JoystickButton& button)
{
    _buttons[button] = InputHandler::KeyState::Pressed;
    if (_joystickTrace.has_value())
        _joystickTrace.value().addFollower(button);
    else
        _joystickTrace.emplace(button);
    return *this;
}

IController& SimulatedController::simulateButtonHeld(
    const JoystickButton& button)
{
    _buttons[button] = InputHandler::KeyState::Held;
    if (_joystickTrace.has_value())
        _joystickTrace.value().addFollower(button);
    else
        _joystickTrace.emplace(button);
    return *this;
}

IController& SimulatedController::simulateButtonRelease(
    const JoystickButton& button)
{
    _buttons[button] = InputHandler::KeyState::Released;
    if (_joystickTrace.has_value()) {
        _joystickTraces.push_back(_joystickTrace.value());
        _joystickTrace.reset();
    }
    return *this;
}

IController& SimulatedController::simulateButtonIdle(
    const JoystickButton& button)
{
    _buttons[button] = InputHandler::KeyState::Idle;
    return *this;
}

IController& SimulatedController::simulateAxisMovement(
    const JoystickAxisIndex& axis, const JoystickAxis& value)
{
    _axes[axis] = value;
    return *this;
}

IController& SimulatedController::resetSimulatedInputs()
{
    abort();
    return *this;
}

ManualController::ManualController(const ControllerType& type)
    : AController(type)
{
}

Joystick::Joystick(const JoystickIndex& joystick_id)
    : ManualController(JOYSTICK)
    , _joystick_id(joystick_id)
{
}

bool Joystick::isConnected() const
{
    return _input_handler.isJoystickConnected(_joystick_id);
}

bool Joystick::isButtonPressed(const JoystickButton& button) const
{
    return _input_handler.isButtonPressed(_joystick_id, button);
}

bool Joystick::isButtonHeld(const JoystickButton& button) const
{
    return _input_handler.isButtonHeld(_joystick_id, button);
}

bool Joystick::isButtonReleased(const JoystickButton& button) const
{
    return _input_handler.isButtonReleased(_joystick_id, button);
}

bool Joystick::isButtonPressedOrHeld(const JoystickButton& button) const
{
    return _input_handler.isButtonPressedOrHeld(_joystick_id, button);
}

bool Joystick::isButtonCombinationInput(
    const std::vector<JoystickButton>& buttons) const
{
    return _input_handler.isButtonCombinationInput(_joystick_id, buttons);
}

/*bool Joystick::isComboInput(const std::vector<Input> &inputs) const
{
    auto trace = InputHandler::getInstance().getJoystickInputs(_joystick_id,
inputs.size());

    for (size_t i = 0; i < inputs.size(); ++i) {
        if (inputs.at(i) != trace.at(i))
            return false;
    }
    return true;
}*/

void Joystick::setAxisDeadZone(const float& radius)
{
    _input_handler.setAxisDeadZone(_joystick_id, radius);
}

JoystickAxis Joystick::getAxisPosition(const JoystickAxisIndex& axis) const
{
    return _input_handler.getAxisPosition(_joystick_id, axis);
}

Vector2<float> Joystick::getAxisXY() const
{
    return _input_handler.getAxisXY(_joystick_id);
}

Vector2<float> Joystick::getAxisXYGlobal() const
{
    return _input_handler.getAxisXYGlobal(_joystick_id);
}

Vector2<float> Joystick::getAxisZR() const
{
    return _input_handler.getAxisZR(_joystick_id);
}

Vector2<float> Joystick::getAxisZRGlobal() const
{
    return _input_handler.getAxisZRGlobal(_joystick_id);
}

Vector2<float> Joystick::getAxisUV() const
{
    return _input_handler.getAxisUV(_joystick_id);
}

Vector2<float> Joystick::getAxisUVGlobal() const
{
    return _input_handler.getAxisUVGlobal(_joystick_id);
}

Vector2<float> Joystick::getAxisPOV() const
{
    return _input_handler.getAxisPOV(_joystick_id);
}

const JoystickIndex& Joystick::getJoystickIndex() const { return _joystick_id; }

Keyboard::Keyboard()
    : ManualController(KEYBOARD)
{
}

Keyboard& Keyboard::setKey(
    const JoystickButton& button, const sf::Keyboard::Key& key)
{
    _buttonKeys[button] = key;
    return *this;
}

Keyboard& Keyboard::setAxis(const JoystickAxisIndex& axis,
    const sf::Keyboard::Key& positiveAxis,
    const sf::Keyboard::Key& negativeAxis)
{
    _axisKeys[axis * 2] = positiveAxis;
    _axisKeys[axis * 2 + 1] = negativeAxis;
    return *this;
}

bool Keyboard::isConnected() const { return true; }

bool Keyboard::isButtonPressed(const JoystickButton& button) const
{
    return _input_handler.isKeyPressed(_buttonKeys[button]);
}

bool Keyboard::isButtonHeld(const JoystickButton& button) const
{
    return _input_handler.isKeyHeld(_buttonKeys[button]);
}

bool Keyboard::isButtonReleased(const JoystickButton& button) const
{
    return _input_handler.isKeyReleased(_buttonKeys[button]);
}

bool Keyboard::isButtonPressedOrHeld(const JoystickButton& button) const
{
    return _input_handler.isKeyPressedOrHeld(_buttonKeys[button]);
}

bool Keyboard::isButtonCombinationInput(
    const std::vector<JoystickButton>& buttons) const
{
    std::vector<sf::Keyboard::Key> _keys;
    for (auto& button : buttons)
        _keys.push_back(_buttonKeys[button]);
    return _input_handler.isKeyCombinationInput(_keys);
}

/*bool Keyboard::isComboInput(const std::vector<Input> &inputs) const
{
    // std::vector<Input>
    // auto trace = InputHandler::getInstance().getKeyInputs(inputs.size());

    // for (size_t i = 0; i < inputs.size(); ++i) {
    //     if (inputs.at(i) != trace.at(i))
    //         return false;
    // }
    return true;
}*/

void Keyboard::setAxisDeadZone(const float& radius)
{
    _deadZone = radius < 0 ? 0 : radius;
}

JoystickAxis Keyboard::getAxisPosition(const JoystickAxisIndex& axis) const
{
    const uint8_t is_positive
        = _input_handler.isKeyPressedOrHeld(_axisKeys[axis * 2]);
    const uint8_t is_negative
        = _input_handler.isKeyPressedOrHeld(_axisKeys[axis * 2 + 1]);

    float zone = _deadZone;
    float value = (is_positive - is_negative) * 100;
    if (value < -zone)
        return static_cast<JoystickAxis>(value);
    else if (value > zone)
        return static_cast<JoystickAxis>(value);
    return 0.f;
}

Vector2<float> Keyboard::getAxisXY() const
{
    return Vector2<float>(getAxisPosition(sf::Joystick::Axis::X),
        getAxisPosition(sf::Joystick::Axis::Y));
}

Vector2<float> Keyboard::getAxisXYGlobal() const { return getAxisXY(); }

Vector2<float> Keyboard::getAxisZR() const
{
    return Vector2<float>(getAxisPosition(sf::Joystick::Axis::Z),
        getAxisPosition(sf::Joystick::Axis::R));
}

Vector2<float> Keyboard::getAxisZRGlobal() const { return getAxisZR(); }

Vector2<float> Keyboard::getAxisUV() const
{
    return Vector2<float>(getAxisPosition(sf::Joystick::Axis::U),
        getAxisPosition(sf::Joystick::Axis::V));
}

Vector2<float> Keyboard::getAxisUVGlobal() const { return getAxisUV(); }

Vector2<float> Keyboard::getAxisPOV() const
{
    return Vector2<float>(getAxisPosition(sf::Joystick::Axis::PovX),
        getAxisPosition(sf::Joystick::Axis::PovY));
}
}
