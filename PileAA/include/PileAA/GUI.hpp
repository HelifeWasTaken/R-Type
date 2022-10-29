#pragma once

#include "external/imgui/imgui-SFML.h"
#include "external/imgui/imgui.h"
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace paa {

enum GuiObjectType { MENU_ITEM, MENU_BAR, BUTTON, INPUT_TEXT, TEXT };

/**
 * @brief Interface for any GuiObject of ImGUI
 *
 */
class GuiObject {
public:
    /**
     * @brief Updates the object
     */
    virtual void update() = 0;

    /**
     * @brief Returns the type of the object
     *
     * @return GuiObjectType
     */
    virtual GuiObjectType type() const = 0;

    /**
     * @brief Construct a new Gui Object object
     */
    GuiObject() = default;

    /**
     * @brief Destroy the Gui Object object
     */
    virtual ~GuiObject() = default;
};

template <typename T>
concept isGuiObject = std::is_base_of_v<GuiObject, T>;

/**
 * @brief Abstraction of a gui element
 *
 * @tparam T
 */
template <isGuiObject T> using shared_gui = std::shared_ptr<T>;

/**
 * @brief Menu item of the menu bar
 *
 */
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
    /**
     * @brief Construct a new Menu Item object
     *
     * @param sectionName The name of the menu section
     */
    MenuItem(const std::string& sectionName);

    /**
     * @brief Destroy the Menu Item object
     */
    ~MenuItem() override = default;

    /**
     * @brief Add an item to the menu section
     *
     * @param name The name of the item
     * @param shortcut The shortcut of the item
     * @param callback The callback of the item
     */
    void addItem(const std::string& name, const std::string& shortcut,
        std::function<void()> callback);

    /**
     * @brief Update the menu item
     */
    void update() override;

    /**
     * @brief Get the type of the object
     *
     * @return GuiObjectType
     */
    GuiObjectType type() const override;
};

class MenuBar : public GuiObject {
private:
    std::vector<MenuItem> _items;

public:
    /**
     * @brief Construct a new Menu Bar object
     */
    MenuBar() = default;

    /**
     * @brief Destroy the Menu Bar object
     */
    ~MenuBar() override = default;

    /**
     * @brief Add an item to the menu bar
     *
     * @param item The item to add
     */
    void addItem(const MenuItem& item);

    /**
     * @brief Update the menu bar
     */
    void update() override;

    /**
     * @brief Get the type of the object
     *
     * @return GuiObjectType
     */
    GuiObjectType type() const override;
};

using shared_menu_bar = shared_gui<MenuBar>;

class Button : public GuiObject {
private:
    std::string _name;
    std::function<void()> _callback;

public:
    /**
     * @brief Construct a new Button object
     *
     * @param name The name of the button
     * @param callback The callback of the button
     */
    Button(const std::string& name, std::function<void()> callback);

    /**
     * @brief Destroy the Button object
     */
    ~Button() override = default;

    /**
     * @brief Update the button
     */
    void update() override;

    /**
     * @brief Get the type of the object
     *
     * @return GuiObjectType
     */
    GuiObjectType type() const override;
};

using shared_button = shared_gui<Button>;

class GuiText : public GuiObject {
private:
    std::string _text = "Hello World";

public:
    /**
     * @brief Construct a new Gui Text object
     *
     * @param text The text to display
     */
    GuiText(const std::string& text);

    /**
     * @brief Construct a new Gui Text object
     */
    GuiText() = default;

    /**
     * @brief Destroy the Gui Text object
     */
    ~GuiText() override = default;

    /**
     * @brief Update the text
     */
    void update() override;

    /**
     * @brief Get the type of the object
     *
     * @return GuiObjectType
     */
    GuiObjectType type() const override;

    /**
     * @brief Set the text
     *
     * @param text The text to set
     */
    void setText(const std::string& text);
};

using shared_gui_text = shared_gui<GuiText>;

class InputText : public GuiObject {
private:
    std::vector<char> _buffer;
    std::string _label;

public:
    /**
     * @brief Construct a new Input Text object
     *
     * @param text The text to display
     * @param label The label of the input text
     * @param maxSize The maximum size of the input text
     */
    InputText(const std::string& text, const std::string& label,
        const size_t maxSize = 512);

    /**
     * @brief Destroy the Input Text object
     */
    ~InputText() override = default;

    /**
     * @brief Update the input text
     */
    void update() override;

    /**
     * @brief Get the text
     *
     * @return std::string
     */
    std::string getText() const;

    /**
     * @brief Get the type of the object
     *
     * @return GuiObjectType
     */
    GuiObjectType type() const override;
};

using shared_input_text = shared_gui<InputText>;

class Gui {
private:
    std::vector<shared_gui<GuiObject>> _objects;

public:
    /**
     * @brief Construct a new Gui object
     */
    Gui() = default;

    /**
     * @brief Destroy the Gui object
     */
    ~Gui() = default;

    /**
     * @brief Add an object to the gui
     *
     * @tparam T
     * @param object The object to add
     */
    void addObject(const shared_gui<GuiObject>& object);

    /**
     * @brief Update the gui
     */
    void addObject(GuiObject* object);
    ;

    /**
     * @brief Update the gui
     */
    void update();

    /**
     * @brief Render the gui
     */
    void clear();
};

class GuiFactory {
public:
    template <isGuiObject T, typename... Args>
    static shared_gui<T> new_gui_object(Args&&... args)
    {
        GuiObject* obj = new T(std::forward<Args>(args)...);
        return std::shared_ptr<T>(static_cast<T*>(obj));
    }

    template <typename... Args>
    static shared_menu_bar new_menu_bar(Args&&... args)
    {
        return GuiFactory::new_gui_object<MenuBar>(std::forward<Args>(args)...);
    }

    template <typename... Args> static shared_button new_button(Args&&... args)
    {
        return GuiFactory::new_gui_object<Button>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static shared_gui_text new_gui_text(Args&&... args)
    {
        return GuiFactory::new_gui_object<GuiText>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static shared_input_text new_input_text(Args&&... args)
    {
        return GuiFactory::new_gui_object<InputText>(
            std::forward<Args>(args)...);
    }
};

}
