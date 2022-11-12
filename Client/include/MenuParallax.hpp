#pragma once

#include <PileAA/Parallax.hpp>

namespace rtype {
class MenuParallax {
    static inline std::shared_ptr<paa::Parallax> _menuInstance;

public:
    static paa::Parallax& recreate()
    {
        if (_menuInstance != nullptr) {
            clear();
        }
        _menuInstance = std::shared_ptr<paa::Parallax>(
            new paa::Parallax(paa::Vector2f(100, 80), paa::Vector2f(100, 0),
                { "parallax_menu_1", "parallax_menu_2", "parallax_menu_3" },
                    paa::Vector2f(4, 4),
                true));
        return *_menuInstance;
    }

    static paa::Parallax& get()
    {
        if (_menuInstance == nullptr) {
            return recreate();
        }
        return *_menuInstance;
    }

    static void clear() {
        _menuInstance = nullptr;
    }

    static void update() {
        get().update();
    }
};
}