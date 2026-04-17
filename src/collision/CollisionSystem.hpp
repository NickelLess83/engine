#pragma once
#include "CollisionEvent.hpp"

namespace engine {
class Registry;

class CollisionSystem {
public:
    // cellSize should be roughly 2x the average collider size.
    void update(Registry& reg, CollisionEventQueue& events, float cellSize = 128.0f);
};

} // namespace engine
