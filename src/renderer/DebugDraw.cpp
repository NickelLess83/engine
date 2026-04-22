#include "DebugDraw.hpp"

#ifdef ENGINE_DEBUG

#include "ShaderProgram.hpp"
#include <glad/glad.h>
#include <cmath>

namespace engine
{

DebugDraw::DebugDraw()
{
    m_shader =
        std::make_unique<ShaderProgram>("assets/shaders/debug.vert", "assets/shaders/debug.frag");

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, MAX_VERTS * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

    // layout: position(2), color(4) — matches debug.vert
    constexpr int STRIDE = 6 * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, STRIDE, (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, STRIDE, (void *)(2 * sizeof(float)));

    glBindVertexArray(0);
}

DebugDraw::~DebugDraw()
{
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

void DebugDraw::pushVertex(glm::vec2 pos, glm::vec4 col)
{
    if (m_verts.size() >= MAX_VERTS)
        return;
    m_verts.push_back({pos.x, pos.y, col.r, col.g, col.b, col.a});
}

void DebugDraw::line(glm::vec2 a, glm::vec2 b, glm::vec4 color)
{
    if (!enabled)
        return;
    pushVertex(a, color);
    pushVertex(b, color);
}

void DebugDraw::aabb(glm::vec2 min, glm::vec2 max, glm::vec4 color)
{
    if (!enabled)
        return;
    line({min.x, min.y}, {max.x, min.y}, color);
    line({max.x, min.y}, {max.x, max.y}, color);
    line({max.x, max.y}, {min.x, max.y}, color);
    line({min.x, max.y}, {min.x, min.y}, color);
}

void DebugDraw::circle(glm::vec2 center, float radius, glm::vec4 color, int segments)
{
    if (!enabled)
        return;
    float step = 2.0f * 3.14159265f / static_cast<float>(segments);
    for (int i = 0; i < segments; ++i) {
        float a0 = step * i;
        float a1 = step * (i + 1);
        line({center.x + std::cos(a0) * radius, center.y + std::sin(a0) * radius},
             {center.x + std::cos(a1) * radius, center.y + std::sin(a1) * radius}, color);
    }
}

void DebugDraw::ray(glm::vec2 origin, glm::vec2 dir, glm::vec4 color)
{
    if (!enabled)
        return;
    line(origin, origin + dir, color);
}

void DebugDraw::flush(const glm::mat4 &viewProjection)
{
    if (!enabled || m_verts.empty())
        return;

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_verts.size() * sizeof(Vertex), m_verts.data());

    m_shader->bind();
    m_shader->setMat4("u_viewProjection", viewProjection);

    glDrawArrays(GL_LINES, 0, static_cast<int>(m_verts.size()));

    glBindVertexArray(0);
    m_verts.clear();
}

} // namespace engine

#else // ENGINE_DEBUG not defined — provide no-op stubs

namespace engine
{

DebugDraw::DebugDraw() = default;
DebugDraw::~DebugDraw() = default;

void DebugDraw::line(glm::vec2, glm::vec2, glm::vec4) {}
void DebugDraw::aabb(glm::vec2, glm::vec2, glm::vec4) {}
void DebugDraw::circle(glm::vec2, float, glm::vec4, int) {}
void DebugDraw::ray(glm::vec2, glm::vec2, glm::vec4) {}
void DebugDraw::flush(const glm::mat4 &) {}

} // namespace engine

#endif
