#pragma once
#include "RenderQueue.hpp"
#include <cstddef>

namespace engine {

class SpriteBatch {
public:
    explicit SpriteBatch(size_t maxSprites = 10000);
    ~SpriteBatch();

    SpriteBatch(const SpriteBatch&)            = delete;
    SpriteBatch& operator=(const SpriteBatch&) = delete;

    // Sort by layer then texture, batch consecutive same-texture commands,
    // upload vertices once, issue one draw call per texture change.
    void flush(const RenderQueue& queue);

private:
    static void buildQuad(std::vector<float>& verts, const DrawCommand& cmd);

    unsigned int m_vao        = 0;
    unsigned int m_vbo        = 0;
    unsigned int m_ebo        = 0;
    size_t       m_maxSprites = 0;
};

} // namespace engine
