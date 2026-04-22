#pragma once
#include "AudioEvent.hpp"
#include <glm/vec2.hpp>
#include <array>

namespace engine
{

class AssetManager;

class AudioSystem
{
public:
    static constexpr int CHANNELS = 16;

    AudioSystem();
    ~AudioSystem();

    AudioSystem(const AudioSystem &) = delete;
    AudioSystem &operator=(const AudioSystem &) = delete;

    // Consume the queue. Call once per fixed-timestep tick.
    void update(AudioEventQueue &queue, AssetManager &assets, glm::vec2 listenerPos = {0.0f, 0.0f});

    // Convenience: crossfade to a new music track over `duration` seconds.
    void crossfadeTo(Handle<Music> music, AssetManager &assets, float duration = 1.0f);

private:
    void playSound(const AudioEvent &e, AssetManager &assets, glm::vec2 listenerPos);
    int findChannel(int priority) const;

    struct ChannelInfo {
        bool active = false;
        int priority = 0;
    };
    std::array<ChannelInfo, CHANNELS> m_channels{};
};

} // namespace engine
