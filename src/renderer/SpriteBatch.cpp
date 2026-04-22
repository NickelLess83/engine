#include "SpriteBatch.hpp"
#include <glad/glad.h>
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace engine
{

SpriteBatch::SpriteBatch(size_t maxSprites) : m_maxSprites(maxSprites)
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // Dynamic vertex buffer — updated every frame
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(maxSprites * 4 * 8 * sizeof(float)),
                 nullptr, GL_DYNAMIC_DRAW);

    // Static index buffer — pattern never changes
    std::vector<unsigned int> indices(maxSprites * 6);
    for (size_t i = 0; i < maxSprites; ++i) {
        unsigned int b = static_cast<unsigned int>(i) * 4;
        indices[i * 6 + 0] = b + 0;
        indices[i * 6 + 1] = b + 1;
        indices[i * 6 + 2] = b + 2;
        indices[i * 6 + 3] = b + 2;
        indices[i * 6 + 4] = b + 3;
        indices[i * 6 + 5] = b + 0;
    }

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)), indices.data(),
                 GL_STATIC_DRAW);

    // layout: position(2), texCoord(2), color(4) — matches sprite.vert
    constexpr int STRIDE = 8 * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, STRIDE, nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, STRIDE, (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, STRIDE, (void *)(4 * sizeof(float)));

    glBindVertexArray(0);
}

SpriteBatch::~SpriteBatch()
{
    glDeleteBuffers(1, &m_ebo);
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

void SpriteBatch::flush(const RenderQueue &queue)
{
    const auto &src = queue.commands();
    if (src.empty())
        return;

    if (src.size() > m_maxSprites)
        throw std::runtime_error("SpriteBatch: too many sprites in one frame");

    // Sort: layer ascending, then texture to group batches
    auto cmds = src;
    std::stable_sort(cmds.begin(), cmds.end(), [](const DrawCommand &a, const DrawCommand &b) {
        if (a.layer != b.layer)
            return a.layer < b.layer;
        return a.textureID < b.textureID;
    });

    // Build all vertices in one pass
    std::vector<float> verts;
    verts.reserve(cmds.size() * 4 * 8);
    for (const auto &cmd : cmds)
        buildQuad(verts, cmd);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(verts.size() * sizeof(float)),
                    verts.data());

    // Issue one draw call per texture group
    uint32_t currentTex = cmds[0].textureID;
    size_t batchStart = 0;

    auto drawBatch = [&](size_t end) {
        size_t count = end - batchStart;
        if (count == 0)
            return;
        glBindTexture(GL_TEXTURE_2D, currentTex);
        glDrawElements(GL_TRIANGLES, static_cast<int>(count * 6), GL_UNSIGNED_INT,
                       (void *)(batchStart * 6 * sizeof(unsigned int)));
    };

    for (size_t i = 1; i < cmds.size(); ++i) {
        if (cmds[i].textureID != currentTex) {
            drawBatch(i);
            currentTex = cmds[i].textureID;
            batchStart = i;
        }
    }
    drawBatch(cmds.size());

    glBindVertexArray(0);
}

void SpriteBatch::buildQuad(std::vector<float> &verts, const DrawCommand &cmd)
{
    float hw = cmd.scale.x * 0.5f;
    float hh = cmd.scale.y * 0.5f;

    // Local corners: TL, TR, BR, BL
    float lx[4] = {-hw, hw, hw, -hw};
    float ly[4] = {hh, hh, -hh, -hh};

    float cosR = std::cos(cmd.rotation);
    float sinR = std::sin(cmd.rotation);

    // UV corners matching Y-up (stb_image loaded flipped): TL, TR, BR, BL
    float u[4] = {cmd.sourceRect.x, cmd.sourceRect.x + cmd.sourceRect.w,
                  cmd.sourceRect.x + cmd.sourceRect.w, cmd.sourceRect.x};
    float v[4] = {cmd.sourceRect.y + cmd.sourceRect.h, cmd.sourceRect.y + cmd.sourceRect.h,
                  cmd.sourceRect.y, cmd.sourceRect.y};

    for (int i = 0; i < 4; ++i) {
        float wx = lx[i] * cosR - ly[i] * sinR + cmd.position.x;
        float wy = lx[i] * sinR + ly[i] * cosR + cmd.position.y;

        verts.push_back(wx);
        verts.push_back(wy);
        verts.push_back(u[i]);
        verts.push_back(v[i]);
        verts.push_back(cmd.colorTint.r);
        verts.push_back(cmd.colorTint.g);
        verts.push_back(cmd.colorTint.b);
        verts.push_back(cmd.colorTint.a);
    }
}

} // namespace engine
