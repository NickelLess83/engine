#pragma once
#include <string>

namespace engine
{

class Texture
{
public:
    // Load from file (PNG, JPG, etc. via stb_image)
    explicit Texture(const std::string &path);

    // Create from raw RGBA pixel data
    Texture(int width, int height, const unsigned char *data);

    ~Texture();

    Texture(const Texture &) = delete;
    Texture &operator=(const Texture &) = delete;

    void bind(unsigned int slot = 0) const;

    int width() const { return m_width; }
    int height() const { return m_height; }
    unsigned int id() const { return m_id; }

private:
    void upload(int width, int height, const unsigned char *data);

    unsigned int m_id = 0;
    int m_width = 0;
    int m_height = 0;
};

} // namespace engine
