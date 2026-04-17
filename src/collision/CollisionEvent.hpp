#pragma once
#include "ecs/Entity.hpp"
#include <glm/glm.hpp>
#include <vector>

namespace engine {

struct CollisionEvent {
    Entity    entityA;
    Entity    entityB;
    glm::vec2 normal; // points from A toward B
    float     depth;  // penetration depth
};

class CollisionEventQueue {
public:
    void push(const CollisionEvent& e) { m_events.push_back(e); }
    void clear()                        { m_events.clear(); }

    const std::vector<CollisionEvent>& events() const { return m_events; }
    bool empty() const { return m_events.empty(); }

private:
    std::vector<CollisionEvent> m_events;
};

} // namespace engine
