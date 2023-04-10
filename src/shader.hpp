#pragma once
#include <string>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

class Shader
{
private:
    struct Source
    {
        std::string path;
        unsigned int type;
    };

protected:
    unsigned int id;

protected:
    Shader(const std::vector<Source> &sources);

public:
    ~Shader();

    bool valid() const;
    void bind() const;

    void set_int(unsigned int location, int value) const;
    void set_float(unsigned int location, float value) const;
    void set_vec2(unsigned int location, const glm::vec2 &value) const;
    void set_ivec2(unsigned int location, const glm::ivec2 &value) const;
    void set_vec3(unsigned int location, const glm::vec3 &value) const;
    void set_vec4(unsigned int location, const glm::vec4 &value) const;
    void set_mat4(unsigned int location, const glm::mat4 &value) const;

    void set_int(const std::string &name, int value) const;
    void set_float(const std::string &name, float value) const;
    void set_vec2(const std::string &name, const glm::vec2 &value) const;
    void set_ivec2(const std::string &name, const glm::ivec2 &value) const;
    void set_vec3(const std::string &name, const glm::vec3 &value) const;
    void set_vec4(const std::string &name, const glm::vec4 &value) const;
    void set_mat4(const std::string &name, const glm::mat4 &value) const;
};

class RenderShader : public Shader
{
public:
    RenderShader(const std::string &vertex_path,
            const std::string &fragment_path);
};

class ComputeShader : public Shader
{
private:
    glm::uvec3 work_group;

public:
    ComputeShader(const std::string &path);

    void set_work_group(const glm::uvec3 &work_group);
    void dispatch_and_wait() const;
};
