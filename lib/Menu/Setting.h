#pragma once
#include <MenuComponent.h>
template <typename T>
struct NamedValue {
    const char* name;
    T value;
};

template <typename T>
class Setting {
    NamedValue<T> m_value;
    bool m_isSet;

   public:
    Setting(NamedValue<T> value) : m_value(value){};
};

template <typename T, uint8_t size>
class SettingGroup {
    NamedValue<T> m_options[size];
    uint8_t m_selectedOption = 0;

   public:
    SettingGroup(NamedValue<T> options[size]) {
        for (uint8_t i = 0; i < size; i++) {
            m_options[i] = options[i];
        }
    };
    SettingGroup(const char* values[size]) {
        for (uint8_t i = 0; i < size; i++) {
            m_options[i] = NamedValue<uint8_t>{values[i], i};
        }
    };

    typedef void (SettingGroup<T, size>::*setting_func)(uint8_t);

    void selectOption(uint8_t index) {
        m_selectedOption = index;
    }

    T getSelectedValue() {
        return m_options[m_selectedOption].value;
    }

    SubMenu* createSettingsMenu(const char* name, std::function<void()> callback = nullptr) {
        SubMenu* menu = new SubMenu(name);
        for (uint8_t i = 0; i < size; i++) {
            menu->addChild(new Leaf(
                m_options[i].name,
                [this, callback, i]() {
                    selectOption(i);
                    if (callback) callback();
                },
                [this, i]() {
                    if (m_selectedOption == i) return "(X)";
                    return "( )";
                }));
        }
        return menu;
    }
};