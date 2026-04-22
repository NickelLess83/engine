#pragma once
#include "renderer/Window.hpp"
#include "renderer/SpriteBatch.hpp"
#include "renderer/RenderQueue.hpp"
#include "renderer/Camera.hpp"
#include "renderer/DebugDraw.hpp"
#include "renderer/ShaderProgram.hpp"
#include "renderer/Texture.hpp"
#include "input/InputManager.hpp"
#include "assets/AssetManager.hpp"
#include "scene/SceneManager.hpp"
#include "audio/AudioSystem.hpp"
#include "audio/AudioEvent.hpp"
#include <memory>

namespace engine
{

class Engine
{
public:
    Engine();
    void run();

    static constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;
    static constexpr float MAX_FRAME_TIME = 0.25f;

    // Exposed so game code can register scenes and push the first one.
    SceneManager &sceneManager() { return m_scenes; }
    AssetManager &assetManager() { return m_assets; }
    AudioSystem &audioSystem() { return m_audio; }
    AudioEventQueue &audioEventQueue() { return m_audioQueue; }

private:
    void update(float dt);
    void render(float alpha);

    Window m_window;
    InputManager m_input;
    Camera m_camera;
    AssetManager m_assets;
    SceneManager m_scenes;

    AudioSystem m_audio;
    AudioEventQueue m_audioQueue;

    Handle<ShaderProgram> m_spriteShaderHandle;
    std::unique_ptr<SpriteBatch> m_spriteBatch;
    RenderQueue m_renderQueue;
    DebugDraw m_debug;
};

} // namespace engine
