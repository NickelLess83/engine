#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace engine
{

class ShaderProgram;

class DebugDraw
{
public:
    DebugDraw();
    ~DebugDraw();

    DebugDraw(const DebugDraw &) = delete;
    DebugDraw &operator=(const DebugDraw &) = delete;

    // Primitive submission — all no-ops when ENGINE_DEBUG is not defined
    void line(glm::vec2 a, glm::vec2 b, glm::vec4 color = {0.0f, 1.0f, 0.0f, 1.0f});
    void aabb(glm::vec2 min, glm::vec2 max, glm::vec4 color = {0.0f, 1.0f, 0.0f, 1.0f});
    void circle(glm::vec2 center, float radius, glm::vec4 color = {0.0f, 1.0f, 0.0f, 1.0f},
                int segments = 32);
    void ray(glm::vec2 origin, glm::vec2 dir, glm::vec4 color = {1.0f, 1.0f, 0.0f, 1.0f});

    // Draw all accumulated primitives then clear — call after SpriteBatch::flush
    void flush(const glm::mat4 &viewProjection);

    bool enabled = true;

private:
#ifdef ENGINE_DEBUG
    struct Vertex {
        float x, y, r, g, b, a;
    };

    void pushVertex(glm::vec2 pos, glm::vec4 col);

    std::vector<Vertex> m_verts;
    std::unique_ptr<ShaderProgram> m_shader;
    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;

    static constexpr size_t MAX_VERTS = 65536;
#endif
};

} // namespace engine
