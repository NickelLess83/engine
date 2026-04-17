#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <cstdint>

namespace engine {

struct Rect {
    float x = 0.0f;
    float y = 0.0f;
    float w = 1.0f;
    float h = 1.0f;
};

struct DrawCommand {
    uint32_t  textureID = 0;
    Rect      sourceRect;                              // normalized UV [0,1]
    glm::vec2 position  = {0.0f, 0.0f};
    glm::vec2 scale     = {1.0f, 1.0f};               // world-space size
    float     rotation  = 0.0f;                       // radians
    glm::vec4 colorTint = {1.0f, 1.0f, 1.0f, 1.0f};
    int       layer     = 0;
};

class RenderQueue {
public:
    void submit(const DrawCommand& cmd) { m_commands.push_back(cmd); }
    void clear()                        { m_commands.clear(); }

    const std::vector<DrawCommand>& commands() const { return m_commands; }

private:
    std::vector<DrawCommand> m_commands;
};

} // namespace engine
