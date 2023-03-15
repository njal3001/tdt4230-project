#include "particle.hpp"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include "calc.hpp"

ParticleBuffer::ParticleBuffer(size_t count, const ComputeShader *shader)
    : shader(shader)
{
    this->positions.resize(count);
    this->velocity.resize(count);
    this->colors.resize(count);

    for (size_t i = 0; i < this->count(); i++)
    {
        this->positions[i].x = Calc::randrange(-10.0f, 10.0f);
        this->positions[i].y = Calc::randrange(-10.0f, 10.0f);
        this->positions[i].z = -500.0f;
        this->colors[i].r = Calc::randrange(0.0f, 1.0f);
        this->colors[i].g = Calc::randrange(0.0f, 1.0f);
        this->colors[i].b = Calc::randrange(0.0f, 1.0f);
        this->colors[i].a = 1.0f;

        this->velocity[i].x = Calc::randrange(-25.0f, 25.0f);
        this->velocity[i].y = Calc::randrange(-25.0f, 25.0f);
        this->velocity[i].z = 0.0f;
    }
}

size_t ParticleBuffer::count() const
{
    return this->positions.size();
}

void ParticleBuffer::update(float dt)
{
    for (size_t i = 0; i < this->count(); i++)
    {
        this->positions[i] += this->velocity[i] * dt;
    }
}

ParticleRenderer::ParticleRenderer(const ParticleBuffer *buffer, const RenderShader *shader)
    : buffer(buffer), models(buffer->count()), shader(shader)
{
    std::vector<glm::vec3> positions =
    {
        glm::vec3(-0.5f, 0.5f, 0.0f),
        glm::vec3(-0.5f, -0.5f, 0.0f),
        glm::vec3(0.5f, -0.5f, 0.0f),
        glm::vec3(0.5f, 0.5f, 0.0f),
    };

    std::vector<unsigned int> indices =
    {
        0, 1, 2,
        0, 2, 3,
    };

    glGenBuffers(1, &this->vbo_position);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec3),
            positions.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
            (const void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &this->vbo_model);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_model);
    glBufferData(GL_ARRAY_BUFFER, buffer->count() * sizeof(glm::mat4),
            nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
            (const void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
            (const void*)(1 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
            (const void*)(2 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
            (const void*)(3 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(4);

    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);

    glGenBuffers(1, &this->vbo_color);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
    glBufferData(GL_ARRAY_BUFFER, buffer->count() * sizeof(glm::vec4),
            nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4),
            (const void*)0);
    glEnableVertexAttribArray(5);

    glVertexAttribDivisor(5, 1);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            indices.size() * sizeof(unsigned int),
            indices.data(), GL_STATIC_DRAW);
}

ParticleRenderer::~ParticleRenderer()
{
    glDeleteBuffers(1, &this->vbo_position);
    glDeleteBuffers(1, &this->vbo_model);
    glDeleteBuffers(1, &this->vbo_color);
    glDeleteBuffers(1, &this->ebo);
    glDeleteVertexArrays(1, &this->vao);
}

void ParticleRenderer::render()
{
    this->shader->bind();

    for (size_t i = 0; i < this->buffer->count(); i++)
    {
        this->models[i] = glm::translate(glm::mat4(1.0f), this->buffer->positions[i]);
    }

    glBindVertexArray(this->vao);

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_model);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
            models.size() * sizeof(glm::mat4), models.data());

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_color);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
            this->buffer->count() * sizeof(glm::vec4), this->buffer->colors.data());

    glDrawElementsInstanced(GL_TRIANGLES, 6,
            GL_UNSIGNED_INT, 0, this->buffer->count());
}
