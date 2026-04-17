#pragma once
#include "Scene.hpp"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace engine {

class SceneManager {
public:
    // Register a factory — manager never includes concrete scene headers.
    void registerScene(const std::string& name,
                       std::function<std::unique_ptr<Scene>()> factory);

    // Deferred transitions — safe to call from inside update/render.
    void push   (const std::string& name);
    void pop    ();
    void replace(const std::string& name); // pop + push atomically

    // Call once per frame (plan step 14) to execute queued transitions.
    void applyPendingTransitions(AssetManager& assets);

    // Update: top scene always runs; propagates down while isActive() == true.
    void update(float dt, InputManager& input);

    // Render: find deepest visible scene (skip transparent stack), render upward.
    void render(RenderQueue& queue, AssetManager& assets, DebugDraw& debug, float alpha);

    bool empty() const { return m_stack.empty(); }

private:
    enum class TransType { Push, Pop, Replace };
    struct Transition { TransType type; std::string name; };

    std::vector<std::unique_ptr<Scene>>                          m_stack;
    std::unordered_map<std::string,
        std::function<std::unique_ptr<Scene>()>>                 m_factories;
    std::vector<Transition>                                      m_pending;
};

} // namespace engine
