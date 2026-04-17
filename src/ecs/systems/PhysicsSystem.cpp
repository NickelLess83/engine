#include "PhysicsSystem.hpp"
#include "ecs/Registry.hpp"
#include "ecs/View.hpp"
#include "ecs/Components.hpp"
#include "collision/CollisionEvent.hpp"
#include <glm/glm.hpp>

namespace engine {

void PhysicsSystem::update(Registry& reg, float dt)
{
    for (auto [entity, transform, velocity] : reg.view<Transform, Velocity>()) {
        transform.position += velocity.linear  * dt;
        transform.rotation += velocity.angular * dt;
    }
}

void PhysicsSystem::resolveCollisions(Registry& reg, const CollisionEventQueue& events)
{
    // normal points A → B.
    // relVel = dot(velA - velB, normal) > 0 means they are approaching — resolve.
    constexpr float RESTITUTION = 0.8f;

    for (const auto& e : events.events()) {
        bool hasVelA = reg.has<Velocity>(e.entityA);
        bool hasVelB = reg.has<Velocity>(e.entityB);
        if (!hasVelA && !hasVelB) continue;

        auto& tA = reg.get<Transform>(e.entityA);
        auto& tB = reg.get<Transform>(e.entityB);

        if (hasVelA && hasVelB) {
            tA.position -= e.normal * (e.depth * 0.5f);
            tB.position += e.normal * (e.depth * 0.5f);

            auto& vA = reg.get<Velocity>(e.entityA);
            auto& vB = reg.get<Velocity>(e.entityB);

            float relVel = glm::dot(vA.linear - vB.linear, e.normal);
            if (relVel > 0.0f) {
                // J = -(1+e)*relVel/2  (negative → pushes A back, B forward)
                float J = -(1.0f + RESTITUTION) * relVel * 0.5f;
                vA.linear += J * e.normal;
                vB.linear -= J * e.normal;
            }
        } else if (hasVelA) {
            // B is static (infinite mass) — push A away fully
            tA.position -= e.normal * e.depth;
            auto& vA = reg.get<Velocity>(e.entityA);
            float relVel = glm::dot(vA.linear, e.normal);
            if (relVel > 0.0f)
                vA.linear += -(1.0f + RESTITUTION) * relVel * e.normal;
        } else {
            // A is static — push B away fully
            tB.position += e.normal * e.depth;
            auto& vB = reg.get<Velocity>(e.entityB);
            float relVel = glm::dot(vB.linear, -e.normal);
            if (relVel > 0.0f)
                vB.linear += -(1.0f + RESTITUTION) * relVel * (-e.normal);
        }
    }
}

} // namespace engine
