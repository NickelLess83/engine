#include "Music.hpp"
#include <SDL2/SDL_mixer.h>
#include <stdexcept>

namespace engine
{

Music::Music(const std::string &path)
{
    m_music = Mix_LoadMUS(path.c_str());
    if (!m_music)
        throw std::runtime_error("Music: failed to load '" + path + "': " + Mix_GetError());
}

Music::~Music()
{
    if (m_music)
        Mix_FreeMusic(m_music);
}

Music::Music(Music &&o) noexcept : m_music(o.m_music) { o.m_music = nullptr; }

Music &Music::operator=(Music &&o) noexcept
{
    if (this != &o) {
        if (m_music)
            Mix_FreeMusic(m_music);
        m_music = o.m_music;
        o.m_music = nullptr;
    }
    return *this;
}

} // namespace engine
