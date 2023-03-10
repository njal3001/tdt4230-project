#include "shader.hpp"
#include <GL/glew.h>
#include <stdio.h>
#include <glm/gtc/type_ptr.hpp>
#include "file.hpp"

static unsigned int compile_shader(const std::string &path, unsigned int type)
{
    std::string content = File::content(path);
    const char *ccontent = content.c_str();

    unsigned int id = glCreateShader(type);
    glShaderSource(id, 1, &ccontent, NULL);
    glCompileShader(id);

    char shader_error_msg[512];
    int success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(id, 512, NULL, shader_error_msg);
        printf("Failed to compile shader %s\n%s", path.c_str(),
                shader_error_msg);
        return 0;
    }

    return id;
}

Shader::Shader(const std::string &vertex_path, const std::string &fragment_path)
{
    unsigned int vertex_id =
        compile_shader(vertex_path, GL_VERTEX_SHADER);
    if (!vertex_id)
    {
        this->id = 0;
        return;
    }

    unsigned int fragment_id =
        compile_shader(fragment_path, GL_FRAGMENT_SHADER);
    if (!fragment_id)
    {
        glDeleteShader(fragment_id);
        this->id = 0;
        return;
    }

    this->id = glCreateProgram();
    glAttachShader(this->id, vertex_id);
    glAttachShader(this->id, fragment_id);
    glLinkProgram(this->id);

    glDeleteShader(vertex_id);
    glDeleteShader(fragment_id);

    GLint link_success;
    glGetProgramiv(this->id, GL_LINK_STATUS, &link_success);
    if (!link_success)
    {
        this->id = 0;
        printf("Failed to link shaders: %s, %s", vertex_path.c_str(),
                fragment_path.c_str());
    }
}

Shader::~Shader()
{
    glDeleteProgram(this->id);
}

bool Shader::valid() const
{
    return this->id;
}

void Shader::bind() const
{
    glUseProgram(this->id);
}

void Shader::set_float(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(this->id, name.c_str()), value);
}

void Shader::set_vec2(const std::string &name, const glm::vec2 &value) const
{
    glUniform2fv(glGetUniformLocation(this->id, name.c_str()), 1,
            glm::value_ptr(value));
}

void Shader::set_vec3(const std::string &name, const glm::vec3 &value) const
{
    glUniform3fv(glGetUniformLocation(this->id, name.c_str()), 1,
            glm::value_ptr(value));
}

void Shader::set_vec4(const std::string &name, const glm::vec4 &value) const
{
    glUniform4fv(glGetUniformLocation(this->id, name.c_str()), 1,
            glm::value_ptr(value));
}

void Shader::set_mat4(const std::string &name, const glm::mat4 &value) const
{
    glUniformMatrix4fv(glGetUniformLocation(this->id, name.c_str()), 1,
            GL_FALSE, glm::value_ptr(value));
}
