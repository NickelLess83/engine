#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

namespace engine
{

enum class Key {
    Unknown,
    Escape,
    Space,
    Enter,
    Backspace,
    Tab,
    Left,
    Right,
    Up,
    Down,
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    Num0,
    Num1,
    Num2,
    Num3,
    Num4,
    Num5,
    Num6,
    Num7,
    Num8,
    Num9,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    LShift,
    RShift,
    LCtrl,
    RCtrl,
    LAlt,
    RAlt,
    _Count
};

class InputManager
{
public:
    void update();
    void endFrame();

    void bindAction(const std::string &action, Key key);

    bool isActionJustPressed(const std::string &action) const;
    bool isActionHeld(const std::string &action) const;
    bool isActionJustReleased(const std::string &action) const;

    bool quitRequested() const { return m_quit; }

private:
    enum class KeyState : uint8_t { None, JustPressed, Held, JustReleased };

    static constexpr int KEY_COUNT = static_cast<int>(Key::_Count);

    KeyState m_keys[KEY_COUNT] = {};
    std::unordered_map<std::string, Key> m_bindings;
    bool m_quit = false;
};

} // namespace engine
