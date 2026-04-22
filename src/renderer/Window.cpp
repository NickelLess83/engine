#include "Window.hpp"
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <stdexcept>

namespace engine
{

Window::Window(const WindowConfig &config) : m_width(config.width), m_height(config.height)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        throw std::runtime_error(SDL_GetError());

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    m_window =
        SDL_CreateWindow(config.title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                         config.width, config.height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (!m_window) {
        SDL_Quit();
        throw std::runtime_error(SDL_GetError());
    }

    m_context = SDL_GL_CreateContext(m_window);
    if (!m_context) {
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        throw std::runtime_error(SDL_GetError());
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        SDL_GL_DeleteContext(m_context);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        throw std::runtime_error("Failed to initialize GLAD");
    }

    SDL_GL_SetSwapInterval(config.vsync ? 1 : 0);

    glViewport(0, 0, config.width, config.height);

    m_open = true;
}

Window::~Window()
{
    if (m_context)
        SDL_GL_DeleteContext(m_context);
    if (m_window)
        SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void Window::swapBuffers() { SDL_GL_SwapWindow(m_window); }

void Window::close() { m_open = false; }

} // namespace engine
