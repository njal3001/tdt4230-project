#include "particle.hpp"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "calc.hpp"
#include "graphics.hpp"

ParticleSystem::ParticleSystem(size_t count, const ComputeShader *compute_shader,
        const RenderShader *render_shader)
    : compute_shader(compute_shader), render_shader(render_shader)
{
    this->positions.resize(count);
    this->velocity.resize(count);
    this->colors.resize(count);

    for (size_t i = 0; i < this->count(); i++)
    {
        this->positions[i].x = Calc::randrange(-1.0f, 1.0f);
        this->positions[i].y = Calc::randrange(-1.0f, 1.0f);
        this->positions[i].z = 0.0f;

        this->colors[i].r = Calc::randrange(0.0f, 1.0f);
        this->colors[i].g = Calc::randrange(0.0f, 1.0f);
        this->colors[i].b = Calc::randrange(0.0f, 1.0f);
        this->colors[i].a = 1.0f;

        this->velocity[i].x = Calc::randrange(-1.0f, 1.0f);
        this->velocity[i].y = Calc::randrange(-1.0f, 1.0f);
        this->velocity[i].z = 0.0f;
    }

    glGenVertexArrays(1, &this->vao);
    glBindVertexArray(this->vao);

    glGenBuffers(1, &this->vbo_position);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_position);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(glm::vec3),
            this->positions.data(), GL_DYNAMIC_DRAW);

    glGenBuffers(1, &this->vbo_velocity);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_velocity);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(glm::vec3),
            this->velocity.data(), GL_DYNAMIC_DRAW);

    glGenBuffers(1, &this->vbo_color);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_color);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(glm::vec4),
            this->colors.data(), GL_DYNAMIC_DRAW);

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
    this->compute_shader->set_float("dt", dt);

    glm::vec2 cursor = Graphics::get_normalized_cursor_position();
    this->compute_shader->set_vec2("cursor", cursor);

    glDispatchCompute(this->count() / 128, 1, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void ParticleSystem::render() const
{
    this->render_shader->bind();
    glDrawArraysInstanced(GL_POINTS, 0, 1, this->count());
}
