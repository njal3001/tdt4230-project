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

    const ComputeShader *compute_shader;
    const RenderShader *render_shader;
    unsigned int vao;
    unsigned int vbo_position;
    unsigned int vbo_velocity;
    unsigned int vbo_life;
    unsigned int vbo_color;
    size_t last_particle_used;

public:
    ParticleSystem(size_t count, const ComputeShader *compute_shader,
            const RenderShader *render_shader);
    ~ParticleSystem();

    void update(float dt);
    void render() const;

    void spawn(size_t spawn_count, glm::vec3 position);
    size_t count() const;
};
