#pragma once
#include <string>

// Forward-declare to keep SDL_mixer out of headers
struct Mix_Chunk;

namespace engine {

class Sound {
public:
    Sound() = default;
    explicit Sound(const std::string& path);
    ~Sound();

    Sound(const Sound&)            = delete;
    Sound& operator=(const Sound&) = delete;
    Sound(Sound&& o) noexcept;
    Sound& operator=(Sound&& o) noexcept;

    Mix_Chunk* chunk() const { return m_chunk; }
    bool       valid() const { return m_chunk != nullptr; }

private:
    Mix_Chunk* m_chunk = nullptr;
};

} // namespace engine
