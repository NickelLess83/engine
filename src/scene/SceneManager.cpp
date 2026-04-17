#include "SceneManager.hpp"
#include "assets/AssetManager.hpp"
#include "input/InputManager.hpp"
#include "renderer/RenderQueue.hpp"
#include "renderer/DebugDraw.hpp"
#include <cassert>
#include <stdexcept>

namespace engine {

void SceneManager::registerScene(const std::string& name,
                                  std::function<std::unique_ptr<Scene>()> factory)
{
    m_factories[name] = std::move(factory);
}

void SceneManager::push(const std::string& name)
{
    m_pending.push_back({TransType::Push, name});
}

void SceneManager::pop()
{
    m_pending.push_back({TransType::Pop, ""});
}

void SceneManager::replace(const std::string& name)
{
    m_pending.push_back({TransType::Replace, name});
}

void SceneManager::applyPendingTransitions(AssetManager& assets)
{
    for (auto& t : m_pending) {
        switch (t.type) {

        case TransType::Push: {
            auto it = m_factories.find(t.name);
            if (it == m_factories.end())
                throw std::runtime_error("SceneManager: unknown scene '" + t.name + "'");
            auto scene = it->second();
            scene->onEnter(assets);
            m_stack.push_back(std::move(scene));
            break;
        }

        case TransType::Pop:
            assert(!m_stack.empty() && "SceneManager::pop on empty stack");
            m_stack.back()->onExit(assets);
            m_stack.pop_back();
            break;

        case TransType::Replace: {
            auto it = m_factories.find(t.name);
            if (it == m_factories.end())
                throw std::runtime_error("SceneManager: unknown scene '" + t.name + "'");
            if (!m_stack.empty()) {
                m_stack.back()->onExit(assets);
                m_stack.pop_back();
            }
            auto scene = it->second();
            scene->onEnter(assets);
            m_stack.push_back(std::move(scene));
            break;
        }
        }
    }
    m_pending.clear();
}

void SceneManager::update(float dt, InputManager& input)
{
    // Update from top; propagate downward while each scene says "below me is active".
    for (int i = (int)m_stack.size() - 1; i >= 0; --i) {
        m_stack[i]->update(dt, input);
        if (!m_stack[i]->isActive()) break;
    }
}

void SceneManager::render(RenderQueue& queue, AssetManager& assets,
                           DebugDraw& debug, float alpha)
{
    if (m_stack.empty()) return;

    // Find the deepest scene that must render (first non-transparent from the top).
    int startIdx = (int)m_stack.size() - 1;
    while (startIdx > 0 && m_stack[startIdx]->isTransparent())
        --startIdx;

    for (int i = startIdx; i < (int)m_stack.size(); ++i)
        m_stack[i]->render(queue, assets, debug, alpha);
}

} // namespace engine
