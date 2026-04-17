#pragma once

namespace engine {
class Registry;
class CollisionEventQueue;

class PhysicsSystem {
public:
    void update           (Registry& reg, float dt);
    void resolveCollisions(Registry& reg, const CollisionEventQueue& events);
};

} // namespace engine
