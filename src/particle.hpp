#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "shader.hpp"

struct ParticleBuffer
{
public:
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> velocity;
    std::vector<glm::vec4> colors;

    const ComputeShader *shader;

    ParticleBuffer(size_t count, const ComputeShader *shader);
    void update(float dt);
    size_t count() const;
};

class ParticleRenderer
{
private:
    const ParticleBuffer *buffer;
    const RenderShader *shader;
    unsigned int vao;
    unsigned int vbo_position;
    unsigned int vbo_model;
    unsigned int vbo_color;
    unsigned int ebo;

    std::vector<glm::mat4> models;

public:
    ParticleRenderer(const ParticleBuffer *buffer, const RenderShader *shader);
    ~ParticleRenderer();

    void render();
};
