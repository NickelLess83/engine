#pragma once
#include <cstdint>
#include <functional> // std::hash

namespace engine
{

// Entity handle — 20-bit index (~1M entities) + 12-bit generation (stale detection)
struct Entity {
    static constexpr uint32_t INDEX_BITS = 20;
    static constexpr uint32_t GEN_BITS = 12;
    static constexpr uint32_t INDEX_MASK = (1u << INDEX_BITS) - 1u;
    static constexpr uint32_t GEN_MASK = (1u << GEN_BITS) - 1u;

    uint32_t id = 0;

    uint32_t index() const { return id & INDEX_MASK; }
    uint32_t generation() const { return (id >> INDEX_BITS) & GEN_MASK; }

    static Entity make(uint32_t index, uint32_t generation)
    {
        return {(generation << INDEX_BITS) | (index & INDEX_MASK)};
    }

    bool operator==(const Entity &o) const { return id == o.id; }
    bool operator!=(const Entity &o) const { return id != o.id; }
};

inline constexpr Entity NULL_ENTITY = {~0u};

} // namespace engine

template<> struct std::hash<engine::Entity> {
    size_t operator()(engine::Entity e) const noexcept { return std::hash<uint32_t>{}(e.id); }
};
