#include "PileAA/GUI.hpp"

namespace paa {

MenuItem::MenuItem(const std::string& sectionName)
    : _sectionName(sectionName)
{
}

void MenuItem::addItem(const std::string& name, const std::string& shortcut,
    std::function<void()> callback)
{
    _items.push_back({ name, shortcut, callback });
}

void MenuItem::update()
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

GuiObjectType MenuItem::type() const
{
    return GuiObjectType::MENU_ITEM;
}

void MenuBar::addItem(const MenuItem& item)
{
    _items.push_back(item);
}

void MenuBar::update()
{
    if (ImGui::BeginMenuBar()) {
        for (auto& item : _items) {
            item.update();
        }
        ImGui::EndMenuBar();
    }
}

GuiObjectType MenuBar::type() const
{
    return GuiObjectType::MENU_BAR;
}

Button::Button(const std::string& name, std::function<void()> callback)
    : _name(name)
    , _callback(callback)
{
}

void Button::update()
{
    if (ImGui::Button(_name.c_str())) {
        _callback();
    }
}

GuiObjectType Button::type() const
{
    return GuiObjectType::BUTTON;
}

GuiText::GuiText(const std::string& text)
    : _text(text)
{
}

void GuiText::update()
{
    ImGui::Text(_text.c_str());
}

void GuiText::setText(const std::string& text)
{
    _text = text;
}

GuiObjectType GuiText::type() const
{
    return GuiObjectType::TEXT;
}

InputText::InputText(
    const std::string& text, const std::string& label, const size_t maxSize)
    : _label(label)
{
    _buffer.resize(maxSize);
    std::copy(text.begin(), text.end(), _buffer.begin());
    _buffer[text.size()] = '\0';
}

void InputText::update()
{
    ImGui::InputText(_label.c_str(), _buffer.data(), _buffer.size());
}

std::string InputText::getText() const
{
    return std::string(_buffer.data());
}

GuiObjectType InputText::type() const
{
    return GuiObjectType::INPUT_TEXT;
}

void Gui::addObject(const shared_gui<GuiObject>& object)
{
    _objects.push_back(object);
}

void Gui::addObject(GuiObject* object)
{
    _objects.push_back(shared_gui<GuiObject>(object));
}

void Gui::update()
{
    for (auto& object : _objects) {
        object->update();
    }
}

void Gui::clear()
{
    _objects.clear();
}

}