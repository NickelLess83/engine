#pragma once
#include <string>

struct SDL_Window;
typedef void *SDL_GLContext;

namespace engine
{

struct WindowConfig {
    std::string title = "Engine";
    int width = 1280;
    int height = 720;
    bool vsync = true;
};

class Window
{
public:
    explicit Window(const WindowConfig &config);
    ~Window();

    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;

    bool isOpen() const { return m_open; }
    int width() const { return m_width; }
    int height() const { return m_height; }

    void swapBuffers();
    void close();

private:
    SDL_Window *m_window = nullptr;
    SDL_GLContext m_context = nullptr;
    int m_width = 0;
    int m_height = 0;
    bool m_open = false;
};

} // namespace engine
