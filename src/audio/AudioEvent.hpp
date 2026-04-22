#pragma once
#include "assets/Handle.hpp"
#include <glm/vec2.hpp>
#include <optional>
#include <vector>

namespace engine
{

class Sound;
class Music;

struct AudioEvent {
    enum class Type { PlaySound, PlayMusic, StopMusic, FadeMusic };

    Type type;
    Handle<Sound> sound;
    Handle<Music> music;
    float volume = 1.0f;
    int priority = 0;
    float fadeDuration = 0.0f;
    std::optional<glm::vec2> worldPosition;
};

class AudioEventQueue
{
public:
    void push(AudioEvent e) { m_events.push_back(std::move(e)); }
    void clear() { m_events.clear(); }
    const std::vector<AudioEvent> &events() const { return m_events; }

private:
    std::vector<AudioEvent> m_events;
};

} // namespace engine
