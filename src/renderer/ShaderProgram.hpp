#pragma once
#include <string>
#include <glm/glm.hpp>

namespace engine {

class ShaderProgram {
public:
    ShaderProgram(const std::string& vertPath, const std::string& fragPath);
    ~ShaderProgram();

    ShaderProgram(const ShaderProgram&)            = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;

    void bind()   const;
    void unbind() const;

    void setInt  (const std::string& name, int value)             const;
    void setFloat(const std::string& name, float value)           const;
    void setVec2 (const std::string& name, const glm::vec2& value) const;
    void setVec4 (const std::string& name, const glm::vec4& value) const;
    void setMat4 (const std::string& name, const glm::mat4& value) const;

private:
    unsigned int m_id = 0;

    static std::string   loadSource(const std::string& path);
    static unsigned int  compileShader(unsigned int type, const std::string& source, const std::string& path);
};

} // namespace engine
