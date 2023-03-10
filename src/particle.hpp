#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

struct ParticleBuffer
{
public:
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> velocity;
    std::vector<glm::vec4> colors;

    ParticleBuffer(size_t count);
    size_t count() const;
};

class ParticleRenderer
{
private:
    const ParticleBuffer *buffer;
    unsigned int vao;
    unsigned int vbo_position;
    unsigned int vbo_model;
    unsigned int vbo_color;
    unsigned int ebo;

    std::vector<glm::mat4> models;

public:
    ParticleRenderer(const ParticleBuffer *buffer);
    ~ParticleRenderer();

    void render();
};
