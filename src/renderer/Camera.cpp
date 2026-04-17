#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace engine {

Camera::Camera(int viewportWidth, int viewportHeight)
    : m_width(viewportWidth), m_height(viewportHeight)
{}

glm::mat4 Camera::viewProjection() const
{
    float halfW = (m_width  * 0.5f) / zoom;
    float halfH = (m_height * 0.5f) / zoom;

    return glm::ortho(
        position.x - halfW, position.x + halfW,
        position.y - halfH, position.y + halfH,
        -1.0f, 1.0f
    );
}

glm::vec2 Camera::screenToWorld(glm::vec2 screenPos) const
{
    return {
        position.x + (screenPos.x - m_width  * 0.5f) / zoom,
        position.y + (m_height * 0.5f - screenPos.y) / zoom
    };
}

} // namespace engine
