#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "shader.hpp"

class ParticleSystem
{
private:
    std::vector<glm::vec4> positions;
    std::vector<glm::vec4> velocities;
    std::vector<float> lifes;
    std::vector<glm::vec4> colors;

    size_t last_particle_used;

    const ComputeShader *compute_shader;
    const RenderShader *render_shader;
    unsigned int vao;
    unsigned int vbo_position;
    unsigned int vbo_velocity;
    unsigned int vbo_life;
    unsigned int vbo_color;

public:
    ParticleSystem(size_t count, const ComputeShader *compute_shader,
            const RenderShader *render_shader);
    ~ParticleSystem();

    void update(float dt, const glm::vec3 &attractor);
    void render(const glm::mat4 &view_projection) const;

    void spawn(size_t spawn_count, const glm::vec3 &position);
    size_t count() const;
};
