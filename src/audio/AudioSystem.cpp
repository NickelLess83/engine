#include "AudioSystem.hpp"
#include "Sound.hpp"
#include "Music.hpp"
#include "assets/AssetManager.hpp"
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <algorithm>
#include <cmath>

namespace engine
{

AudioSystem::AudioSystem()
{
    if (SDL_WasInit(SDL_INIT_AUDIO) == 0)
        SDL_InitSubSystem(SDL_INIT_AUDIO);

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
        throw std::runtime_error(std::string("AudioSystem: Mix_OpenAudio failed: ") +
                                 Mix_GetError());

    Mix_AllocateChannels(CHANNELS);

    // Track when each channel finishes so we can clear priority info
    Mix_ChannelFinished([](int ch) {
        // We can't access the AudioSystem instance from a C callback directly;
        // channel cleanup happens in update() via Mix_Playing check instead.
        (void)ch;
    });
}

AudioSystem::~AudioSystem()
{
    Mix_HaltChannel(-1);
    Mix_HaltMusic();
    Mix_CloseAudio();
}

// --------------------------------------------------------------------------
// update
// --------------------------------------------------------------------------
void AudioSystem::update(AudioEventQueue &queue, AssetManager &assets, glm::vec2 listenerPos)
{
    // Refresh channel active flags from SDL2_mixer
    for (int i = 0; i < CHANNELS; ++i)
        if (m_channels[i].active && !Mix_Playing(i))
            m_channels[i] = {};

    for (const auto &e : queue.events()) {
        switch (e.type) {
        case AudioEvent::Type::PlaySound:
            playSound(e, assets, listenerPos);
            break;

        case AudioEvent::Type::PlayMusic:
            if (e.sound.valid()) {
                // music handle stored in e.music field
            }
            if (e.music.valid()) {
                auto &m = assets.resolve(e.music);
                if (m.valid()) {
                    int loops = -1; // loop forever
                    Mix_VolumeMusic(static_cast<int>(e.volume * MIX_MAX_VOLUME));
                    Mix_PlayMusic(m.raw(), loops);
                }
            }
            break;

        case AudioEvent::Type::StopMusic:
            Mix_HaltMusic();
            break;

        case AudioEvent::Type::FadeMusic:
            if (e.fadeDuration > 0.0f)
                Mix_FadeOutMusic(static_cast<int>(e.fadeDuration * 1000.0f));
            else
                Mix_HaltMusic();
            break;
        }
    }

    queue.clear();
}

// --------------------------------------------------------------------------
// crossfadeTo
// --------------------------------------------------------------------------
void AudioSystem::crossfadeTo(Handle<Music> handle, AssetManager &assets, float duration)
{
    auto &m = assets.resolve(handle);
    if (!m.valid())
        return;

    if (Mix_PlayingMusic())
        Mix_FadeOutMusic(static_cast<int>(duration * 500.0f)); // fade old out in half the time

    // Fade new track in over the full duration
    Mix_FadeInMusic(m.raw(), -1, static_cast<int>(duration * 1000.0f));
}

// --------------------------------------------------------------------------
// playSound
// --------------------------------------------------------------------------
void AudioSystem::playSound(const AudioEvent &e, AssetManager &assets, glm::vec2 listenerPos)
{
    if (!e.sound.valid())
        return;
    auto &snd = assets.resolve(e.sound);
    if (!snd.valid())
        return;

    int ch = findChannel(e.priority);
    if (ch < 0)
        return; // all channels busy with higher-priority sounds

    // Halt the channel if we're preempting something
    if (m_channels[ch].active)
        Mix_HaltChannel(ch);

    float vol = e.volume;

    // Spatial attenuation: simple linear falloff within [minDist, maxDist]
    if (e.worldPosition.has_value()) {
        constexpr float MIN_DIST = 50.0f;
        constexpr float MAX_DIST = 800.0f;
        float dist = glm::length(*e.worldPosition - listenerPos);
        float t = (dist - MIN_DIST) / (MAX_DIST - MIN_DIST);
        vol *= 1.0f - std::clamp(t, 0.0f, 1.0f);

        // Stereo pan: -1 (left) to +1 (right)
        float dx = e.worldPosition->x - listenerPos.x;
        float pan = std::clamp(dx / MAX_DIST, -1.0f, 1.0f);
        // SDL2_mixer panning: left + right in range [0, 255]
        Uint8 left = static_cast<Uint8>((1.0f - std::max(0.0f, pan)) * 127.5f);
        Uint8 right = static_cast<Uint8>((1.0f - std::max(0.0f, -pan)) * 127.5f);
        Mix_SetPanning(ch, left, right);
    }

    Mix_Volume(ch, static_cast<int>(vol * MIX_MAX_VOLUME));
    Mix_PlayChannel(ch, snd.chunk(), 0);

    m_channels[ch] = {true, e.priority};
}

// --------------------------------------------------------------------------
// findChannel
// --------------------------------------------------------------------------
int AudioSystem::findChannel(int priority) const
{
    // First look for a free channel
    for (int i = 0; i < CHANNELS; ++i)
        if (!m_channels[i].active)
            return i;

    // All busy — preempt lowest-priority channel if it's below our priority
    int lowestPri = priority;
    int candidate = -1;
    for (int i = 0; i < CHANNELS; ++i) {
        if (m_channels[i].priority < lowestPri) {
            lowestPri = m_channels[i].priority;
            candidate = i;
        }
    }
    return candidate;
}

} // namespace engine
