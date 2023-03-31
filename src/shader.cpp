#include "shader.hpp"
#include <GL/glew.h>
#include <iostream>
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

Shader::Shader(const std::vector<Source> &sources)
{
    this->id = glCreateProgram();

    std::vector<unsigned int> shader_ids;
    for (int i = 0; i < sources.size(); i++)
    {
        const Source &source = sources[i];
        unsigned int shader_id = compile_shader(source.path, source.type);

        if (!shader_id)
        {
            for (int j = 0; j < i; j++)
            {
                glDeleteShader(shader_ids[j]);
            }

            this->id = 0;
            return;
        }

        glAttachShader(this->id, shader_id);

        shader_ids.push_back(shader_id);
    }

    glLinkProgram(this->id);

    for (auto shader_id : shader_ids)
    {
        glDeleteShader(shader_id);
    }

    GLint link_success;
    glGetProgramiv(this->id, GL_LINK_STATUS, &link_success);
    if (!link_success)
    {
        this->id = 0;
        std::cout << "Failed to link shaders: ";
        for (size_t i = 0; i < sources.size(); i++)
        {
            std::cout << sources[i].path;
            if (i < sources.size() - 1)
            {
                std::cout << ", ";
            }
        }
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

void Shader::set_int(unsigned int location, int value) const
{
    glUniform1i(location, value);
}

void Shader::set_float(unsigned int location, float value) const
{
    glUniform1f(location, value);
}

void Shader::set_vec2(unsigned int location, const glm::vec2 &value) const
{
    glUniform2fv(location, 1, glm::value_ptr(value));
}

void Shader::set_vec3(unsigned int location, const glm::vec3 &value) const
{
    glUniform3fv(location, 1, glm::value_ptr(value));
}

void Shader::set_vec4(unsigned int location, const glm::vec4 &value) const
{
    glUniform4fv(location, 1, glm::value_ptr(value));
}

void Shader::set_mat4(unsigned int location, const glm::mat4 &value) const
{
    glUniformMatrix4fv(location, 1, false, glm::value_ptr(value));
}

void Shader::set_int(const std::string &name, int value) const
{
    this->set_int(glGetUniformLocation(this->id, name.c_str()), value);
}

void Shader::set_float(const std::string &name, float value) const
{
    this->set_float(glGetUniformLocation(this->id, name.c_str()), value);
}

void Shader::set_vec2(const std::string &name, const glm::vec2 &value) const
{
    this->set_vec2(glGetUniformLocation(this->id, name.c_str()), value);
}

void Shader::set_vec3(const std::string &name, const glm::vec3 &value) const
{
    this->set_vec3(glGetUniformLocation(this->id, name.c_str()), value);
}

void Shader::set_vec4(const std::string &name, const glm::vec4 &value) const
{
    this->set_vec4(glGetUniformLocation(this->id, name.c_str()), value);
}

void Shader::set_mat4(const std::string &name, const glm::mat4 &value) const
{
    this->set_mat4(glGetUniformLocation(this->id, name.c_str()), value);
}

RenderShader::RenderShader(const std::string &vertex_path,
        const std::string &fragment_path)
    : Shader({{vertex_path, GL_VERTEX_SHADER},
            {fragment_path, GL_FRAGMENT_SHADER}})
{ }

ComputeShader::ComputeShader(const std::string &path)
    : Shader({{path, GL_COMPUTE_SHADER}})
{}
