#include "particle.hpp"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include "calc.hpp"
#include "graphics.hpp"

ParticleSystem::ParticleSystem(size_t count, const ComputeShader *compute_shader,
        const RenderShader *render_shader)
    : compute_shader(compute_shader), render_shader(render_shader)
{
    this->positions.resize(count);
    this->velocities.resize(count);
    this->colors.resize(count);

    const size_t ring_count = 64;
    const float max_radius_x = 1.0f;
    const float max_radius_y = 0.7f;
    const float min_radius_x = 0.01f;
    const float min_radius_y = max_radius_y * (min_radius_x / max_radius_x);
    const float central_intensity = 3.0f;
    const float intensity_scale_length = 0.5f;

    size_t particles_per_ring = count / ring_count;
    float delta_angle = 2.0f * glm::pi<float>() / static_cast<float>(particles_per_ring);
    float delta_radius_x = (max_radius_x - min_radius_x) / static_cast<float>(ring_count);
    float delta_radius_y = (max_radius_y - min_radius_y) / static_cast<float>(ring_count);
    float ring_tilt_delta = glm::pi<float>() / static_cast<float>(ring_count);

    for (size_t ring_index = 0; ring_index < ring_count; ring_index++)
    {
        float radius_x = min_radius_x + delta_radius_x * static_cast<float>(ring_index);
        float radius_y = min_radius_y + delta_radius_y * static_cast<float>(ring_index);
        float ring_tilt = ring_tilt_delta * static_cast<float>(ring_index);

        for (size_t i = 0; i < particles_per_ring; i++)
        {
            size_t particle_index = ring_index * particles_per_ring + i;
            float angle = delta_angle * static_cast<float>(i);

            float px = glm::cos(angle) * radius_x;
            float py = glm::sin(angle) * radius_y;
            float prx = px * glm::cos(ring_tilt) - py * glm::sin(ring_tilt);
            float pry = px * glm::sin(ring_tilt) + py * glm::cos(ring_tilt);

            this->positions[particle_index].x = prx;
            this->positions[particle_index].y = pry;
            this->positions[particle_index].z = -5.0f;

            this->velocities[particle_index] = glm::vec4(0.0f);

            // float center_distance = glm::sqrt(prx * prx + pry * pry);
            // float intensity = central_intensity *  glm::exp(-center_distance / intensity_scale_length);

            this->colors[particle_index] = glm::vec4(1.0f);
        }
    }


    // TODO: Needed?
    glCreateVertexArrays(1, &this->vao);

    glCreateBuffers(1, &this->vbo_position);
    glNamedBufferData(this->vbo_position, count * sizeof(glm::vec4),
            this->positions.data(), GL_STATIC_DRAW);

    glCreateBuffers(1, &this->vbo_velocity);
    glNamedBufferData(this->vbo_velocity, count * sizeof(glm::vec4),
            this->velocities.data(), GL_STATIC_DRAW);

    glCreateBuffers(1, &this->vbo_color);
    glNamedBufferData(this->vbo_color, count * sizeof(glm::vec4),
            this->colors.data(), GL_STATIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, this->vbo_position);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, this->vbo_color);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, this->vbo_velocity);
}

ParticleSystem::~ParticleSystem()
{
    glDeleteBuffers(1, &this->vbo_position);
    glDeleteBuffers(1, &this->vbo_velocity);
    glDeleteBuffers(1, &this->vbo_color);
    glDeleteVertexArrays(1, &this->vao);
}

size_t ParticleSystem::count() const
{
    return this->positions.size();
}

void ParticleSystem::update(float dt)
{
    this->compute_shader->bind();
    this->compute_shader->set_float(0, dt);

    glDispatchCompute(this->count() / 128, 1, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void ParticleSystem::render(const glm::mat4 &view_projection) const
{
    this->render_shader->bind();
    this->render_shader->set_mat4(0, view_projection);

    glDrawArraysInstanced(GL_POINTS, 0, 1, this->count());
}
