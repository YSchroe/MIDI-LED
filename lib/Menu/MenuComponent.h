#pragma once
#include <LinkedList.h>

#include <functional>

class MenuComponent {
   protected:
    int m_selectedIndex = 0;
    const char* m_name;
    MenuComponent* m_parent;

   public:
    MenuComponent(const char* name) : m_name(name){};

    const char* getName() {
        return m_name;
    };
    void setParent(MenuComponent* parent) {
        m_parent = parent;
    };
    MenuComponent* getParent() {
        return m_parent;
    };
    int getSelectedIndex() {
        return m_selectedIndex;
    };

    virtual LinkedList<MenuComponent*>* getChildren() {
        return nullptr;
    };
    virtual MenuComponent* getSelectedComponent() {
        return nullptr;
    };
    virtual int addChild(MenuComponent* child) {
        return -1;
    };
    virtual int moveCursor(int direction) {
        return -1;
    };
    virtual int getChildCount() {
        return 0;
    };
    virtual const char* getDisplayValue() {
        return "";
    };
    virtual void execute(){};
};

class SubMenu : public MenuComponent {
   private:
    LinkedList<MenuComponent*> m_children;

   public:
    SubMenu(const char* name) : MenuComponent(name){};

    LinkedList<MenuComponent*>* getChildren() {
        return &m_children;
    };
    MenuComponent* getSelectedComponent() {
        return m_children.get(m_selectedIndex);
    }
    int getChildCount() {
        return m_children.size();
    };
    int addChild(MenuComponent* child);
    int moveCursor(int direction);
};

class Leaf : public MenuComponent {
    std::function<void()> m_func;
    std::function<const char*()> m_valueFunc;

   public:
    Leaf(const char* name, std::function<void()> func = nullptr, std::function<const char*()> valueFunc = nullptr) : MenuComponent(name),
                                                                                                                     m_func(func),
                                                                                                                     m_valueFunc(valueFunc){};

    void execute() {
        if (m_func) m_func();
    }

    const char* getDisplayValue() {
        if (m_valueFunc) return m_valueFunc();
        return "";
    };
};
