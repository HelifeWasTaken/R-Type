#pragma once

#include <PileAA/external/obscur/InputHandler.hpp>

#include "BaseComponents.hpp"
#include "InputHandler.hpp"
#include "Timer.hpp"
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace paa {

class InputBox {

    std::string input;
    int cursorPos;
    paa::Timer repeatTimer;
    paa::Keyboard::Key lastInput;
    int maxLen;

    bool validated;

public:
    InputBox() {
        input = "";
        cursorPos = 0;
        lastInput = paa::Keyboard::Menu;
        repeatTimer.setTarget(300);
        repeatTimer.restart();
        validated = false;
    }

    ~InputBox() {
    }

    void setMaxLength(int maxLength = 255) {
        maxLen = maxLength;
    }

    bool isValidated() {
        return validated;
    }

    void setValidated(bool value) {
        validated = value;
    }

    void setCursorPos(int pos) {
        if (pos <= 0)
            cursorPos = 0;
        else if (pos > input.size())
            cursorPos = input.size();
        else
            cursorPos = pos;
    }

    int getCursorPos() {
        return cursorPos;
    }

    void setValue(const std::string& value) {
        if (cursorPos == input.size()) {
            cursorPos = value.size();
        }
        input = value;
        setCursorPos(getCursorPos());
    }

    std::string getValue() {
        return input;
    }

    bool isKeyPressed(int key) {
        return isKeyPressed(static_cast<paa::Keyboard::Key>(key));
    }

    bool isKeyPressed(paa::Keyboard::Key key) {
        if (!repeatTimer.isFinished() && PAA_INPUT.isKeyHeld(key))
            return false;
        if (PAA_INPUT.isKeyPressedOrHeld(key)) {
            lastInput = key;
            repeatTimer.restart();
            return true;
        }
        return false;
    }

    void appendToInput(char c) {
        if (input.size() >= maxLen)
            return;
        input.insert(input.begin() + cursorPos, c);
        if (cursorPos < input.size())
            cursorPos++;
    }

    void deleteBackInput() {
        if (input.size() == 0)
            return;
        if (cursorPos > 0) {
            input.erase(input.begin() + cursorPos - 1);
            cursorPos--;
        }
    }

    void deleteFrontInput() {
        if (input.size() < cursorPos + 1)
            return;
        input.erase(input.begin() + cursorPos);
    }

    void update() {
        if (PAA_INPUT.isKeyPressed(paa::Keyboard::Enter)) {
            validated = true;
        }
        if (validated)
            return;
        for(int i = paa::Keyboard::A; i <= paa::Keyboard::Z; i++) {
            if (isKeyPressed(i)) {
                appendToInput(i - paa::Keyboard::A + 'a');
            }
        }
        for(int i = paa::Keyboard::Num0; i <= paa::Keyboard::Num9; i++) {
            if (isKeyPressed(i)) {
                appendToInput(i - paa::Keyboard::Num0 + '0');
            }
        }
        for(int i = paa::Keyboard::Numpad0; i <= paa::Keyboard::Numpad9; i++) {
            if (isKeyPressed(i)) {
                appendToInput(i - paa::Keyboard::Numpad0 + '0');
            }
        }
        if (isKeyPressed(paa::Keyboard::Period)) {
            appendToInput('.');
        }
        if (isKeyPressed(paa::Keyboard::Hyphen) || isKeyPressed(paa::Keyboard::Subtract)) {
            appendToInput('-');
        }
        if (isKeyPressed(paa::Keyboard::Backspace)) {
            deleteBackInput();
        }
        if (isKeyPressed(paa::Keyboard::Delete)) {
            deleteFrontInput();
        }
        if (isKeyPressed(paa::Keyboard::Up)) {
            setCursorPos(0);
        }
        if (isKeyPressed(paa::Keyboard::Down)) {
            setCursorPos(input.size() + 1);
        }
        if (isKeyPressed(paa::Keyboard::Left)) {
            setCursorPos(getCursorPos() - 1);
        }
        if (isKeyPressed(paa::Keyboard::Right)) {
            setCursorPos(getCursorPos() + 1);
        }
    }
};

}