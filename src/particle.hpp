#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "shader.hpp"

class ParticleSystem
{
private:
    std::vector<glm::vec4> positions;
    std::vector<glm::vec4> velocities;
    std::vector<glm::vec4> colors;

    const ComputeShader *compute_shader;
    const RenderShader *render_shader;
    unsigned int vao;
    unsigned int vbo_position;
    unsigned int vbo_velocity;
    unsigned int vbo_color;

public:
    ParticleSystem(size_t count, const ComputeShader *compute_shader,
            const RenderShader *render_shader);
    ~ParticleSystem();

    void update(float dt);
    void render(const glm::mat4 &view_projection) const;

    size_t count() const;
};
