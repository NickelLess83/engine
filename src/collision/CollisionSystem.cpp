#include "CollisionSystem.hpp"
#include "ecs/Registry.hpp"
#include "ecs/View.hpp"
#include "ecs/Components.hpp"
#include <glm/glm.hpp>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace engine {

// --------------------------------------------------------------------------
// Narrowphase helpers
// --------------------------------------------------------------------------
struct AABB2 { glm::vec2 min, max; };

static bool testAABBvsAABB(AABB2 a, AABB2 b, glm::vec2& normal, float& depth)
{
    float dx1 = b.max.x - a.min.x, dx2 = a.max.x - b.min.x;
    float dy1 = b.max.y - a.min.y, dy2 = a.max.y - b.min.y;

    if (dx1 <= 0.0f || dx2 <= 0.0f || dy1 <= 0.0f || dy2 <= 0.0f) return false;

    float ox = std::min(dx1, dx2);
    float oy = std::min(dy1, dy2);

    if (ox < oy) {
        depth  = ox;
        normal = {dx1 < dx2 ? -1.0f : 1.0f, 0.0f};
    } else {
        depth  = oy;
        normal = {0.0f, dy1 < dy2 ? -1.0f : 1.0f};
    }
    return true;
}

static bool testCirclevsCircle(glm::vec2 ca, float ra, glm::vec2 cb, float rb,
                                glm::vec2& normal, float& depth)
{
    glm::vec2 d    = cb - ca;
    float     dist = glm::length(d);
    float     sum  = ra + rb;

    if (dist >= sum) return false;

    if (dist < 0.0001f) { normal = {1.0f, 0.0f}; }
    else                { normal = d / dist; }
    depth = sum - dist;
    return true;
}

static bool testAABBvsCircle(AABB2 box, glm::vec2 center, float radius,
                              glm::vec2& normal, float& depth)
{
    glm::vec2 closest = glm::clamp(center, box.min, box.max);
    glm::vec2 delta   = center - closest;
    float     distSq  = glm::dot(delta, delta);

    if (distSq >= radius * radius) return false;

    float dist = std::sqrt(distSq);
    if (dist < 0.0001f) {
        // Center inside box — push out along minimum separation axis
        float dxL = center.x - box.min.x, dxR = box.max.x - center.x;
        float dyD = center.y - box.min.y, dyU = box.max.y - center.y;
        float m   = std::min({dxL, dxR, dyD, dyU});
        if      (m == dxL) { normal = {-1,  0}; depth = radius + dxL; }
        else if (m == dxR) { normal = { 1,  0}; depth = radius + dxR; }
        else if (m == dyD) { normal = { 0, -1}; depth = radius + dyD; }
        else               { normal = { 0,  1}; depth = radius + dyU; }
    } else {
        normal = delta / dist;
        depth  = radius - dist;
    }
    return true;
}

// --------------------------------------------------------------------------
// Broadphase helpers
// --------------------------------------------------------------------------
static uint64_t cellKey(int cx, int cy)
{
    return (uint64_t)(uint32_t)cx << 32 | (uint32_t)cy;
}

static uint64_t pairKey(Entity a, Entity b)
{
    uint32_t lo = std::min(a.id, b.id);
    uint32_t hi = std::max(a.id, b.id);
    return (uint64_t)hi << 32 | lo;
}

static void registerInGrid(glm::vec2 bpMin, glm::vec2 bpMax, Entity entity,
                            float cellSize,
                            std::unordered_map<uint64_t, std::vector<Entity>>& grid)
{
    int minCX = (int)std::floor(bpMin.x / cellSize);
    int minCY = (int)std::floor(bpMin.y / cellSize);
    int maxCX = (int)std::floor(bpMax.x / cellSize);
    int maxCY = (int)std::floor(bpMax.y / cellSize);

    for (int cx = minCX; cx <= maxCX; ++cx)
        for (int cy = minCY; cy <= maxCY; ++cy)
            grid[cellKey(cx, cy)].push_back(entity);
}

// --------------------------------------------------------------------------
// CollisionSystem::update
// --------------------------------------------------------------------------
void CollisionSystem::update(Registry& reg, CollisionEventQueue& events, float cellSize)
{
    events.clear();

    // Build spatial grid
    std::unordered_map<uint64_t, std::vector<Entity>> grid;

    for (auto [entity, transform, collider] : reg.view<Transform, Collider>()) {
        glm::vec2 pos = transform.position + collider.offset;
        glm::vec2 bpMin, bpMax;

        if (collider.shape == Collider::Shape::AABB) {
            bpMin = pos - collider.halfExtents;
            bpMax = pos + collider.halfExtents;
        } else {
            bpMin = pos - glm::vec2{collider.radius};
            bpMax = pos + glm::vec2{collider.radius};
        }

        registerInGrid(bpMin, bpMax, entity, cellSize, grid);
    }

    // Narrowphase — test each candidate pair once
    std::unordered_set<uint64_t> checked;

    for (auto& [ck, entities] : grid) {
        for (size_t i = 0; i < entities.size(); ++i) {
            for (size_t j = i + 1; j < entities.size(); ++j) {
                Entity ea = entities[i], eb = entities[j];

                if (!checked.insert(pairKey(ea, eb)).second) continue;

                // Layer/mask filter
                if (reg.has<CollisionLayer>(ea) && reg.has<CollisionLayer>(eb)) {
                    auto& la = reg.get<CollisionLayer>(ea);
                    auto& lb = reg.get<CollisionLayer>(eb);
                    if (!(la.mask & lb.layer) && !(lb.mask & la.layer)) continue;
                }

                auto& ta = reg.get<Transform>(ea);
                auto& tb = reg.get<Transform>(eb);
                auto& ca = reg.get<Collider>(ea);
                auto& cb = reg.get<Collider>(eb);

                glm::vec2 posA = ta.position + ca.offset;
                glm::vec2 posB = tb.position + cb.offset;

                glm::vec2 normal{};
                float     depth = 0.0f;
                bool      hit   = false;

                if (ca.shape == Collider::Shape::AABB && cb.shape == Collider::Shape::AABB) {
                    hit = testAABBvsAABB(
                        {posA - ca.halfExtents, posA + ca.halfExtents},
                        {posB - cb.halfExtents, posB + cb.halfExtents},
                        normal, depth);
                }
                else if (ca.shape == Collider::Shape::Circle && cb.shape == Collider::Shape::Circle) {
                    hit = testCirclevsCircle(posA, ca.radius, posB, cb.radius, normal, depth);
                }
                else {
                    // Mixed: ensure AABB is always 'a' in the call
                    if (ca.shape == Collider::Shape::AABB) {
                        hit = testAABBvsCircle(
                            {posA - ca.halfExtents, posA + ca.halfExtents},
                            posB, cb.radius, normal, depth);
                    } else {
                        hit = testAABBvsCircle(
                            {posB - cb.halfExtents, posB + cb.halfExtents},
                            posA, ca.radius, normal, depth);
                        normal = -normal; // flip so normal points A → B
                    }
                }

                if (hit)
                    events.push({ea, eb, normal, depth});
            }
        }
    }
}

} // namespace engine
