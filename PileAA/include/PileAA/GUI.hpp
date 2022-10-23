#pragma once

#include "external/imgui/imgui-SFML.h"
#include "external/imgui/imgui.h"
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace paa {

enum GuiObjectType { MENU_ITEM, MENU_BAR, BUTTON, INPUT_TEXT, TEXT };

class GuiObject {
public:
    virtual void update() = 0;
    virtual GuiObjectType type() const = 0;

    GuiObject() = default;
    virtual ~GuiObject() = default;
};

template <typename T> using shared_gui = std::shared_ptr<T>;

class MenuItem : public GuiObject {
private:
    struct MenuItemData {
        std::string name;
        std::string shortcut;
        std::function<void()> callback;
    };

private:
    std::string _sectionName;
    std::vector<MenuItemData> _items;

public:
    MenuItem(const std::string& sectionName)
        : _sectionName(sectionName)
    {
    }

    ~MenuItem() override = default;

    void addItem(const std::string& name, const std::string& shortcut,
        std::function<void()> callback)
    {
        _items.push_back({ name, shortcut, callback });
    }

    void update() override
    {
        if (ImGui::BeginMenu(_sectionName.c_str())) {
            for (auto& item : _items) {
                if (ImGui::MenuItem(item.name.c_str(), item.shortcut.c_str())) {
                    item.callback();
                }
            }
            ImGui::EndMenu();
        }
    }

    GuiObjectType type() const override { return GuiObjectType::MENU_ITEM; }
};

class MenuBar : public GuiObject {
private:
    std::vector<MenuItem> _items;

public:
    MenuBar() = default;
    ~MenuBar() override = default;

    void addItem(const MenuItem& item) { _items.push_back(item); }

    void update() override
    {
        if (ImGui::BeginMenuBar()) {
            for (auto& item : _items) {
                item.update();
            }
            ImGui::EndMenuBar();
        }
    }

    GuiObjectType type() const override { return GuiObjectType::MENU_BAR; }
};

using shared_menu_bar = shared_gui<MenuBar>;

class Button : public GuiObject {
private:
    std::string _name;
    std::function<void()> _callback;

public:
    Button(const std::string& name, std::function<void()> callback)
        : _name(name)
        , _callback(callback)
    {
    }

    ~Button() override = default;

    void update() override
    {
        if (ImGui::Button(_name.c_str())) {
            _callback();
        }
    }

    GuiObjectType type() const override { return GuiObjectType::BUTTON; }
};

using shared_button = shared_gui<Button>;

class GuiText : public GuiObject {
private:
    std::string _text = "Hello World";

public:
    GuiText(const std::string& text)
        : _text(text)
    {
    }
    ~GuiText() override = default;

    void update() override { ImGui::Text(_text.c_str()); }

    void setText(const std::string& text) { _text = text; }

    GuiObjectType type() const override { return GuiObjectType::TEXT; }
};

using shared_gui_text = shared_gui<GuiText>;

class InputText : public GuiObject {
private:
    std::vector<char> _buffer;
    std::string _label;

public:
    InputText(const std::string& text, const std::string& label,
        const size_t maxSize = 512)
        : _label(label)
    {
        _buffer.resize(maxSize);
        std::copy(text.begin(), text.end(), _buffer.begin());
        _buffer[text.size()] = '\0';
    }

    ~InputText() override = default;

    void update() override
    {
        ImGui::InputText(_label.c_str(), _buffer.data(), _buffer.size());
    }

    std::string getText() const { return std::string(_buffer.data()); }

    GuiObjectType type() const override { return GuiObjectType::INPUT_TEXT; }
};

using shared_input_text = shared_gui<InputText>;

class Gui {
private:
    std::vector<shared_gui<GuiObject>> _objects;

public:
    Gui() = default;
    ~Gui() = default;

    void addObject(const shared_gui<GuiObject>& object)
    {
        _objects.push_back(object);
    }

    void addObject(GuiObject* object)
    {
        _objects.push_back(shared_gui<GuiObject>(object));
    }

    void update()
    {
        for (auto& object : _objects) {
            object->update();
        }
    }
};

class GuiFactory {
public:
    template <typename T, typename... Args,
        typename std::enable_if<std::is_base_of<GuiObject, T>::value, int>::type
        = 0>
    static shared_gui<T> new_gui_object(Args&&... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static shared_gui<MenuItem> new_menu_bar(Args&&... args)
    {
        return GuiFactory::new_gui_object<MenuBar>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static shared_gui<Button> new_button(Args&&... args)
    {
        return GuiFactory::new_gui_object<Button>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static shared_gui<GuiText> new_gui_text(Args&&... args)
    {
        return GuiFactory::new_gui_object<GuiText>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static shared_gui<InputText> new_input_text(Args&&... args)
    {
        return GuiFactory::new_gui_object<InputText>(
            std::forward<Args>(args)...);
    }
};

}
