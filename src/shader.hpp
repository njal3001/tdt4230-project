#pragma once
#include <string>
#include <glm/gtc/matrix_transform.hpp>

class Shader
{
private:
    unsigned int id;

public:
    Shader(const std::string &vertex_path, const std::string &fragment_path);
    ~Shader();

    bool valid() const;
    void bind() const;

    void set_float(const std::string &name, float value) const;
    void set_vec2(const std::string &name, const glm::vec2 &value) const;
    void set_vec3(const std::string &name, const glm::vec3 &value) const;
    void set_vec4(const std::string &name, const glm::vec4 &value) const;
    void set_mat4(const std::string &name, const glm::mat4 &value) const;
};
