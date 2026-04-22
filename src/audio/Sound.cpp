#include "Sound.hpp"
#include <SDL2/SDL_mixer.h>
#include <stdexcept>

namespace engine
{

Sound::Sound(const std::string &path)
{
    m_chunk = Mix_LoadWAV(path.c_str());
    if (!m_chunk)
        throw std::runtime_error("Sound: failed to load '" + path + "': " + Mix_GetError());
}

Sound::~Sound()
{
    if (m_chunk)
        Mix_FreeChunk(m_chunk);
}

Sound::Sound(Sound &&o) noexcept : m_chunk(o.m_chunk) { o.m_chunk = nullptr; }

Sound &Sound::operator=(Sound &&o) noexcept
{
    if (this != &o) {
        if (m_chunk)
            Mix_FreeChunk(m_chunk);
        m_chunk = o.m_chunk;
        o.m_chunk = nullptr;
    }
    return *this;
}

} // namespace engine
