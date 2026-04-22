#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Texture.hpp"
#include <glad/glad.h>
#include <stdexcept>

namespace engine
{

Texture::Texture(const std::string &path)
{
    stbi_set_flip_vertically_on_load(true);

    int channels;
    unsigned char *data = stbi_load(path.c_str(), &m_width, &m_height, &channels, STBI_rgb_alpha);
    if (!data)
        throw std::runtime_error("Failed to load texture: " + path + " — " + stbi_failure_reason());

    upload(m_width, m_height, data);
    stbi_image_free(data);
}

Texture::Texture(int width, int height, const unsigned char *data) { upload(width, height, data); }

Texture::~Texture()
{
    if (m_id)
        glDeleteTextures(1, &m_id);
}

void Texture::bind(unsigned int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture::upload(int width, int height, const unsigned char *data)
{
    m_width = width;
    m_height = height;

    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace engine
