#pragma once
#include "scene/Scene.hpp"
#include "ecs/systems/PhysicsSystem.hpp"
#include "ecs/systems/RenderSystem.hpp"
#include "collision/CollisionSystem.hpp"
#include "collision/CollisionEvent.hpp"
#include "assets/Handle.hpp"

namespace engine { class Texture; }

class GameScene : public engine::Scene {
public:
    void onEnter(engine::AssetManager& assets) override;
    void onExit (engine::AssetManager& assets) override;
    void update (float dt, engine::InputManager& input) override;
    void render (engine::RenderQueue& queue, engine::AssetManager& assets,
                 engine::DebugDraw& debug, float alpha) override;

private:
    engine::PhysicsSystem           m_physics;
    engine::CollisionSystem         m_collision;
    engine::RenderSystem            m_renderer;
    engine::CollisionEventQueue     m_collisionEvents;
    engine::Handle<engine::Texture> m_checkerHandle;
};
