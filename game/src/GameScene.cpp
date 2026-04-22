#include "GameScene.hpp"
#include "assets/AssetManager.hpp"
#include "renderer/Texture.hpp"
#include "renderer/RenderQueue.hpp"
#include "renderer/DebugDraw.hpp"
#include "ecs/View.hpp"
#include "ecs/Components.hpp"
#include <vector>
#include <cmath>

static constexpr float HALF_W = 580.0f; // play area half-width  (viewport is 640)
static constexpr float HALF_H = 310.0f; // play area half-height (viewport is 360)

void GameScene::onEnter(engine::AssetManager &assets)
{
    using namespace engine;

    constexpr int SIZE = 64, CELL = 8;
    std::vector<unsigned char> pixels(static_cast<std::size_t>(SIZE) * SIZE * 4);
    for (int y = 0; y < SIZE; ++y) {
        for (int x = 0; x < SIZE; ++x) {
            bool light = ((x / CELL) + (y / CELL)) % 2 == 0;
            int i = (y * SIZE + x) * 4;
            pixels[i + 0] = light ? 220 : 60;
            pixels[i + 1] = light ? 180 : 60;
            pixels[i + 2] = light ? 100 : 180;
            pixels[i + 3] = 255;
        }
    }
    m_checkerHandle = assets.load<Texture>("checkerboard", SIZE, SIZE, pixels.data());

    // Spawn moving entities with evenly-spaced velocity directions so they
    // converge immediately and keep bouncing.
    constexpr int COUNT = 5;
    constexpr float SPEED = 220.0f;
    constexpr float PI = 3.14159265f;

    for (int i = 0; i < COUNT; ++i) {
        float angle = static_cast<float>(i) * (2.0f * PI / COUNT);
        Entity e = registry.create();
        registry.emplace<Transform>(e, glm::vec2{std::cos(angle) * 120.0f, std::sin(angle) * 80.0f},
                                    glm::vec2{80.0f, 80.0f}, 0.0f);
        registry.emplace<Velocity>(e,
                                   glm::vec2{std::cos(angle + PI) * SPEED, // point inward
                                             std::sin(angle + PI) * SPEED},
                                   (i % 2 == 0 ? 1.0f : -1.0f) * 1.2f);
        registry.emplace<Sprite>(e, m_checkerHandle, Rect{0.0f, 0.0f, 1.0f, 1.0f},
                                 glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}, 0);
        registry.emplace<Collider>(e, Collider::Shape::AABB, glm::vec2{0.0f},
                                   glm::vec2{40.0f, 40.0f} // half of 80x80 scale
        );
    }

    // Invisible boundary walls — Transform + Collider, no Sprite
    auto makeWall = [&](glm::vec2 pos, glm::vec2 half) {
        Entity w = registry.create();
        registry.emplace<Transform>(w, pos, glm::vec2{1.0f}, 0.0f);
        registry.emplace<Collider>(w, Collider::Shape::AABB, glm::vec2{0.0f}, half);
    };

    makeWall({0.0f, HALF_H + 50.0f}, {HALF_W + 60.0f, 50.0f});  // top
    makeWall({0.0f, -HALF_H - 50.0f}, {HALF_W + 60.0f, 50.0f}); // bottom
    makeWall({-HALF_W - 50.0f, 0.0f}, {50.0f, HALF_H + 60.0f}); // left
    makeWall({HALF_W + 50.0f, 0.0f}, {50.0f, HALF_H + 60.0f});  // right
}

void GameScene::onExit(engine::AssetManager &assets)
{
    assets.unload<engine::Texture>("checkerboard");
}

void GameScene::update(float dt, engine::InputManager & /*input*/)
{
    m_physics.update(registry, dt);
    m_collision.update(registry, m_collisionEvents);
    m_physics.resolveCollisions(registry, m_collisionEvents);
}

void GameScene::render(engine::RenderQueue &queue, engine::AssetManager &assets,
                       engine::DebugDraw &debug, float alpha)
{
    using namespace engine;

    m_renderer.extract(registry, assets, queue, alpha);

    for (auto [entity, transform, collider] : registry.view<Transform, Collider>()) {
        glm::vec2 pos = transform.position + collider.offset;
        if (collider.shape == Collider::Shape::AABB)
            debug.aabb(pos - collider.halfExtents, pos + collider.halfExtents,
                       {0.0f, 1.0f, 0.0f, 1.0f});
        else
            debug.circle(pos, collider.radius, {0.0f, 1.0f, 0.0f, 1.0f});
    }

    for (auto [entity, transform, velocity] : registry.view<Transform, Velocity>())
        debug.ray(transform.position, velocity.linear * 0.2f, {1.0f, 1.0f, 0.0f, 1.0f});
}
