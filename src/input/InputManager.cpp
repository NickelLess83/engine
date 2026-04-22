#include "InputManager.hpp"
#include <SDL2/SDL.h>

namespace engine
{

static Key scancodeToKey(SDL_Scancode sc)
{
    switch (sc) {
    case SDL_SCANCODE_ESCAPE:
        return Key::Escape;
    case SDL_SCANCODE_SPACE:
        return Key::Space;
    case SDL_SCANCODE_RETURN:
        return Key::Enter;
    case SDL_SCANCODE_BACKSPACE:
        return Key::Backspace;
    case SDL_SCANCODE_TAB:
        return Key::Tab;
    case SDL_SCANCODE_LEFT:
        return Key::Left;
    case SDL_SCANCODE_RIGHT:
        return Key::Right;
    case SDL_SCANCODE_UP:
        return Key::Up;
    case SDL_SCANCODE_DOWN:
        return Key::Down;
    case SDL_SCANCODE_A:
        return Key::A;
    case SDL_SCANCODE_B:
        return Key::B;
    case SDL_SCANCODE_C:
        return Key::C;
    case SDL_SCANCODE_D:
        return Key::D;
    case SDL_SCANCODE_E:
        return Key::E;
    case SDL_SCANCODE_F:
        return Key::F;
    case SDL_SCANCODE_G:
        return Key::G;
    case SDL_SCANCODE_H:
        return Key::H;
    case SDL_SCANCODE_I:
        return Key::I;
    case SDL_SCANCODE_J:
        return Key::J;
    case SDL_SCANCODE_K:
        return Key::K;
    case SDL_SCANCODE_L:
        return Key::L;
    case SDL_SCANCODE_M:
        return Key::M;
    case SDL_SCANCODE_N:
        return Key::N;
    case SDL_SCANCODE_O:
        return Key::O;
    case SDL_SCANCODE_P:
        return Key::P;
    case SDL_SCANCODE_Q:
        return Key::Q;
    case SDL_SCANCODE_R:
        return Key::R;
    case SDL_SCANCODE_S:
        return Key::S;
    case SDL_SCANCODE_T:
        return Key::T;
    case SDL_SCANCODE_U:
        return Key::U;
    case SDL_SCANCODE_V:
        return Key::V;
    case SDL_SCANCODE_W:
        return Key::W;
    case SDL_SCANCODE_X:
        return Key::X;
    case SDL_SCANCODE_Y:
        return Key::Y;
    case SDL_SCANCODE_Z:
        return Key::Z;
    case SDL_SCANCODE_0:
        return Key::Num0;
    case SDL_SCANCODE_1:
        return Key::Num1;
    case SDL_SCANCODE_2:
        return Key::Num2;
    case SDL_SCANCODE_3:
        return Key::Num3;
    case SDL_SCANCODE_4:
        return Key::Num4;
    case SDL_SCANCODE_5:
        return Key::Num5;
    case SDL_SCANCODE_6:
        return Key::Num6;
    case SDL_SCANCODE_7:
        return Key::Num7;
    case SDL_SCANCODE_8:
        return Key::Num8;
    case SDL_SCANCODE_9:
        return Key::Num9;
    case SDL_SCANCODE_F1:
        return Key::F1;
    case SDL_SCANCODE_F2:
        return Key::F2;
    case SDL_SCANCODE_F3:
        return Key::F3;
    case SDL_SCANCODE_F4:
        return Key::F4;
    case SDL_SCANCODE_F5:
        return Key::F5;
    case SDL_SCANCODE_F6:
        return Key::F6;
    case SDL_SCANCODE_F7:
        return Key::F7;
    case SDL_SCANCODE_F8:
        return Key::F8;
    case SDL_SCANCODE_F9:
        return Key::F9;
    case SDL_SCANCODE_F10:
        return Key::F10;
    case SDL_SCANCODE_F11:
        return Key::F11;
    case SDL_SCANCODE_F12:
        return Key::F12;
    case SDL_SCANCODE_LSHIFT:
        return Key::LShift;
    case SDL_SCANCODE_RSHIFT:
        return Key::RShift;
    case SDL_SCANCODE_LCTRL:
        return Key::LCtrl;
    case SDL_SCANCODE_RCTRL:
        return Key::RCtrl;
    case SDL_SCANCODE_LALT:
        return Key::LAlt;
    case SDL_SCANCODE_RALT:
        return Key::RAlt;
    default:
        return Key::Unknown;
    }
}

void InputManager::update()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            m_quit = true;
            break;

        case SDL_KEYDOWN: {
            if (event.key.repeat)
                break;
            Key k = scancodeToKey(event.key.keysym.scancode);
            if (k == Key::Unknown)
                break;
            int idx = static_cast<int>(k);
            m_keys[idx] = KeyState::JustPressed;
            break;
        }

        case SDL_KEYUP: {
            Key k = scancodeToKey(event.key.keysym.scancode);
            if (k == Key::Unknown)
                break;
            int idx = static_cast<int>(k);
            m_keys[idx] = KeyState::JustReleased;
            break;
        }
        }
    }
}

void InputManager::endFrame()
{
    for (auto &state : m_keys) {
        if (state == KeyState::JustPressed)
            state = KeyState::Held;
        else if (state == KeyState::JustReleased)
            state = KeyState::None;
    }
}

void InputManager::bindAction(const std::string &action, Key key) { m_bindings[action] = key; }

bool InputManager::isActionJustPressed(const std::string &action) const
{
    auto it = m_bindings.find(action);
    if (it == m_bindings.end())
        return false;
    return m_keys[static_cast<int>(it->second)] == KeyState::JustPressed;
}

bool InputManager::isActionHeld(const std::string &action) const
{
    auto it = m_bindings.find(action);
    if (it == m_bindings.end())
        return false;
    auto state = m_keys[static_cast<int>(it->second)];
    return state == KeyState::Held || state == KeyState::JustPressed;
}

bool InputManager::isActionJustReleased(const std::string &action) const
{
    auto it = m_bindings.find(action);
    if (it == m_bindings.end())
        return false;
    return m_keys[static_cast<int>(it->second)] == KeyState::JustReleased;
}

} // namespace engine
