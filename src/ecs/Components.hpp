#pragma once
#include "assets/Handle.hpp"
#include "renderer/RenderQueue.hpp"
#include <glm/glm.hpp>

// All components are plain data structs — no methods, no pointers to siblings.

namespace engine
{

class Texture; // forward declaration — Handle<Texture> doesn't need the full type

struct Transform {
    glm::vec2 position = {0.0f, 0.0f};
    glm::vec2 scale = {1.0f, 1.0f}; // world-space size
    float rotation = 0.0f;          // radians
};

struct Velocity {
    glm::vec2 linear = {0.0f, 0.0f};
    float angular = 0.0f; // radians/sec
};

struct Sprite {
    Handle<Texture> texture;
    Rect sourceRect;
    glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};
    int layer = 0;
};

struct Collider {
    enum class Shape { AABB, Circle }; // OBB deferred until needed (requires SAT)
    Shape shape = Shape::AABB;
    glm::vec2 offset = {0.0f, 0.0f};      // relative to Transform position
    glm::vec2 halfExtents = {0.0f, 0.0f}; // AABB
    float radius = 0.0f;                  // Circle
};

struct CollisionLayer {
    uint32_t layer = 1; // which layer this entity is on
    uint32_t mask = 1;  // which layers this entity collides with
};

} // namespace engine
