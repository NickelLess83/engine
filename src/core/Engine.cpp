#include "Engine.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <algorithm>

namespace engine {

Engine::Engine()
    : m_window(WindowConfig{})
    , m_camera(1280, 720)
{
    m_input.bindAction("quit",         Key::Escape);
    m_input.bindAction("cam_left",     Key::A);
    m_input.bindAction("cam_right",    Key::D);
    m_input.bindAction("cam_up",       Key::W);
    m_input.bindAction("cam_down",     Key::S);
    m_input.bindAction("zoom_in",      Key::E);
    m_input.bindAction("zoom_out",     Key::Q);
    m_input.bindAction("toggle_debug", Key::F1);

    m_spriteShaderHandle = m_assets.load<ShaderProgram>("sprite",
        "assets/shaders/sprite.vert",
        "assets/shaders/sprite.frag"
    );

    m_spriteBatch = std::make_unique<SpriteBatch>();
}

void Engine::run()
{
    uint64_t frequency   = SDL_GetPerformanceFrequency();
    uint64_t prevTime    = SDL_GetPerformanceCounter();
    float    accumulator = 0.0f;

    while (m_window.isOpen()) {
        uint64_t now = SDL_GetPerformanceCounter();
        float dt = static_cast<float>(now - prevTime) / static_cast<float>(frequency);
        prevTime = now;

        if (dt > MAX_FRAME_TIME)
            dt = MAX_FRAME_TIME;

        accumulator += dt;

        m_input.update();

        if (m_input.quitRequested() || m_input.isActionJustPressed("quit"))
            m_window.close();

        while (accumulator >= FIXED_TIMESTEP) {
            update(FIXED_TIMESTEP);
            accumulator -= FIXED_TIMESTEP;
        }

        float alpha = accumulator / FIXED_TIMESTEP;
        render(alpha);

        m_scenes.applyPendingTransitions(m_assets);
        m_input.endFrame();
    }
}

void Engine::update(float dt)
{
    constexpr float PAN_SPEED  = 400.0f;
    constexpr float ZOOM_SPEED = 1.5f;
    float speed = PAN_SPEED / m_camera.zoom;

    if (m_input.isActionHeld("cam_left"))  m_camera.position.x -= speed * dt;
    if (m_input.isActionHeld("cam_right")) m_camera.position.x += speed * dt;
    if (m_input.isActionHeld("cam_up"))    m_camera.position.y += speed * dt;
    if (m_input.isActionHeld("cam_down"))  m_camera.position.y -= speed * dt;

    if (m_input.isActionHeld("zoom_in"))
        m_camera.zoom = std::min(m_camera.zoom * (1.0f + ZOOM_SPEED * dt), 10.0f);
    if (m_input.isActionHeld("zoom_out"))
        m_camera.zoom = std::max(m_camera.zoom * (1.0f - ZOOM_SPEED * dt), 0.1f);

    if (m_input.isActionJustPressed("toggle_debug"))
        m_debug.enabled = !m_debug.enabled;

    m_scenes.update(dt, m_input);
    m_audio.update(m_audioQueue, m_assets, m_camera.position);
}

void Engine::render(float alpha)
{
    glClearColor(0.15f, 0.15f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glm::mat4 vp = m_camera.viewProjection();

    m_renderQueue.clear();
    m_scenes.render(m_renderQueue, m_assets, m_debug, alpha);

    ShaderProgram& shader = m_assets.resolve(m_spriteShaderHandle);
    shader.bind();
    shader.setMat4("u_viewProjection", vp);
    shader.setInt("u_texture", 0);
    glActiveTexture(GL_TEXTURE0);

    m_spriteBatch->flush(m_renderQueue);
    m_debug.flush(vp);

    m_window.swapBuffers();
}

} // namespace engine
