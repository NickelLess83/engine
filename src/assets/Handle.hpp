#pragma once
#include <cstdint>

namespace engine {

// Lightweight typed integer handle — trivially copyable, safe in ECS components.
// The AssetManager owns the underlying asset; handles are just IDs.
template<typename T>
struct Handle {
    static constexpr uint32_t INVALID_ID = 0;

    uint32_t id = INVALID_ID;

    bool valid() const { return id != INVALID_ID; }

    bool operator==(const Handle& o) const { return id == o.id; }
    bool operator!=(const Handle& o) const { return id != o.id; }
};

} // namespace engine
