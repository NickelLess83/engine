#include "ShaderProgram.hpp"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace engine {

ShaderProgram::ShaderProgram(const std::string& vertPath, const std::string& fragPath)
{
    std::string vertSrc = loadSource(vertPath);
    std::string fragSrc = loadSource(fragPath);

    unsigned int vert = compileShader(GL_VERTEX_SHADER,   vertSrc, vertPath);
    unsigned int frag = compileShader(GL_FRAGMENT_SHADER, fragSrc, fragPath);

    m_id = glCreateProgram();
    glAttachShader(m_id, vert);
    glAttachShader(m_id, frag);
    glLinkProgram(m_id);

    int success;
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (!success) {
        char log[1024];
        glGetProgramInfoLog(m_id, sizeof(log), nullptr, log);
        glDeleteShader(vert);
        glDeleteShader(frag);
        glDeleteProgram(m_id);
        throw std::runtime_error("Shader link error:\n" + std::string(log));
    }

    glDeleteShader(vert);
    glDeleteShader(frag);
}

ShaderProgram::~ShaderProgram()
{
    if (m_id)
        glDeleteProgram(m_id);
}

void ShaderProgram::bind()   const { glUseProgram(m_id); }
void ShaderProgram::unbind() const { glUseProgram(0); }

void ShaderProgram::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void ShaderProgram::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

void ShaderProgram::setVec2(const std::string& name, const glm::vec2& value) const
{
    glUniform2fv(glGetUniformLocation(m_id, name.c_str()), 1, glm::value_ptr(value));
}

void ShaderProgram::setVec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(m_id, name.c_str()), 1, glm::value_ptr(value));
}

void ShaderProgram::setMat4(const std::string& name, const glm::mat4& value) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

std::string ShaderProgram::loadSource(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Could not open shader file: " + path);

    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

unsigned int ShaderProgram::compileShader(unsigned int type, const std::string& source, const std::string& path)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[1024];
        glGetShaderInfoLog(id, sizeof(log), nullptr, log);
        glDeleteShader(id);
        throw std::runtime_error("Shader compile error in " + path + ":\n" + std::string(log));
    }

    return id;
}

} // namespace engine
