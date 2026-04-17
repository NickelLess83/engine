#pragma once
#include <glm/glm.hpp>

namespace engine {

class Camera {
public:
    Camera(int viewportWidth, int viewportHeight);

    // Combined view-projection matrix — pass directly to u_viewProjection uniform
    glm::mat4 viewProjection() const;

    // Convert a screen-space pixel position (origin top-left) to world space
    glm::vec2 screenToWorld(glm::vec2 screenPos) const;

    glm::vec2 position = {0.0f, 0.0f};
    float     zoom     = 1.0f;

private:
    int m_width;
    int m_height;
};

} // namespace engine
