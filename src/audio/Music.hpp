#pragma once
#include <string>

struct Mix_Music;

namespace engine
{

class Music
{
public:
    Music() = default;
    explicit Music(const std::string &path);
    ~Music();

    Music(const Music &) = delete;
    Music &operator=(const Music &) = delete;
    Music(Music &&o) noexcept;
    Music &operator=(Music &&o) noexcept;

    Mix_Music *raw() const { return m_music; }
    bool valid() const { return m_music != nullptr; }

private:
    Mix_Music *m_music = nullptr;
};

} // namespace engine
