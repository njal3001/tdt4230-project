#include "particle.hpp"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "calc.hpp"
#include "graphics.hpp"

ParticleSystem::ParticleSystem(size_t count, const ComputeShader *compute_shader,
        const RenderShader *render_shader)
    : last_particle_used(0), compute_shader(compute_shader), render_shader(render_shader)
{
    this->positions.resize(count);
    this->velocities.resize(count);
    this->colors.resize(count);
    this->lifes.resize(count, 0.0f);

    // TODO: Needed?
    glCreateVertexArrays(1, &this->vao);

    glCreateBuffers(1, &this->vbo_position);
    glNamedBufferData(this->vbo_position, count * sizeof(glm::vec4),
            this->positions.data(), GL_DYNAMIC_DRAW);

    glCreateBuffers(1, &this->vbo_velocity);
    glNamedBufferData(this->vbo_velocity, count * sizeof(glm::vec4),
            this->velocities.data(), GL_DYNAMIC_DRAW);

    glCreateBuffers(1, &this->vbo_life);
    glNamedBufferData(this->vbo_life, count * sizeof(float),
            this->lifes.data(), GL_DYNAMIC_DRAW);

    glCreateBuffers(1, &this->vbo_color);
    glNamedBufferData(this->vbo_color, count * sizeof(glm::vec4),
            this->colors.data(), GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, this->vbo_position);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, this->vbo_color);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, this->vbo_velocity);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, this->vbo_life);
}

ParticleSystem::~ParticleSystem()
{
    glDeleteBuffers(1, &this->vbo_position);
    glDeleteBuffers(1, &this->vbo_velocity);
    glDeleteBuffers(1, &this->vbo_life);
    glDeleteBuffers(1, &this->vbo_color);
    glDeleteVertexArrays(1, &this->vao);
}

size_t ParticleSystem::count() const
{
    return this->positions.size();
}

void ParticleSystem::spawn(size_t spawn_count, const glm::vec3 &position)
{
    if (spawn_count + this->last_particle_used > this->count())
    {
        this->last_particle_used = 0;
    }

    glm::vec4 *position_map = static_cast<glm::vec4*>(glMapNamedBufferRange(this->vbo_position, this->last_particle_used * sizeof(glm::vec4),
                spawn_count * sizeof(glm::vec4), GL_MAP_WRITE_BIT));
    glm::vec4 *velocity_map = static_cast<glm::vec4*>(glMapNamedBufferRange(this->vbo_velocity, this->last_particle_used * sizeof(glm::vec4),
                spawn_count * sizeof(glm::vec4), GL_MAP_WRITE_BIT));
    float *life_map = static_cast<float*>(glMapNamedBufferRange(this->vbo_life, this->last_particle_used * sizeof(float),
                spawn_count * sizeof(float), GL_MAP_WRITE_BIT));
    glm::vec4 *color_map = static_cast<glm::vec4*>(glMapNamedBufferRange(this->vbo_color, this->last_particle_used * sizeof(glm::vec4),
                spawn_count * sizeof(glm::vec4), GL_MAP_WRITE_BIT));

    for (size_t i = 0; i < spawn_count; i++)
    {
        position_map[i].x = position.x;
        position_map[i].y = position.y;
        position_map[i].z = position.z;

        color_map[i].r = Calc::frandrange(0.0f, 1.0f);
        color_map[i].g = Calc::frandrange(0.0f, 1.0f);
        color_map[i].b = Calc::frandrange(0.0f, 1.0f);
        color_map[i].a = 1.0f;

        velocity_map[i].x = Calc::frandrange(-1.0f, 1.0f);
        velocity_map[i].y = Calc::frandrange(-1.0f, 1.0f);
        velocity_map[i].z = Calc::frandrange(-1.0f, 1.0f);

        life_map[i] = 1000.0f;
    }

    this->last_particle_used += spawn_count;

    glUnmapNamedBuffer(this->vbo_position);
    glUnmapNamedBuffer(this->vbo_velocity);
    glUnmapNamedBuffer(this->vbo_life);
    glUnmapNamedBuffer(this->vbo_color);
}

void ParticleSystem::update(float dt, const glm::vec3 &attractor)
{
    this->compute_shader->bind();
    this->compute_shader->set_float(0, dt);
    this->compute_shader->set_vec3(1, attractor);

    glDispatchCompute(this->count() / 128, 1, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void ParticleSystem::render(const glm::mat4 &view_projection) const
{
    this->render_shader->bind();
    this->render_shader->set_mat4(0, view_projection);

    glDrawArraysInstanced(GL_POINTS, 0, 1, this->count());
}
