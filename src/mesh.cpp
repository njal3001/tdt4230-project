#include "mesh.hpp"
#include <vector>
#include <glad/glad.h>

Mesh::Mesh(const std::vector<Vertex> &vertices)
{
    this->num_vertices = vertices.size();

    glCreateVertexArrays(1, &this->vao);
    glCreateBuffers(1, &this->vbo);
    glNamedBufferData(this->vbo, sizeof(Vertex) * this->num_vertices,
            vertices.data(), GL_STATIC_COPY);

    glVertexArrayVertexBuffer(this->vao, 0, this->vbo, 0, sizeof(Vertex));

    glEnableVertexArrayAttrib(this->vao, 0);
    glEnableVertexArrayAttrib(this->vao, 1);

    glVertexArrayAttribFormat(this->vao, 0, 2, GL_FLOAT, GL_FALSE,
            offsetof(Vertex, position));
    glVertexArrayAttribFormat(this->vao, 1, 2, GL_FLOAT, GL_FALSE,
            offsetof(Vertex, uv));

    glVertexArrayAttribBinding(this->vao, 0, 0);
    glVertexArrayAttribBinding(this->vao, 1, 0);
}

Mesh::~Mesh()
{
   glDeleteBuffers(1, &this->vbo);
   glDeleteVertexArrays(1, &this->vao);
}

void Mesh::render() const
{
    glBindVertexArray(this->vao);
    glDrawArrays(GL_TRIANGLES, 0, this->num_vertices);
}

Mesh Mesh::quad(const glm::vec2 &position, const glm::vec2 &size)
{
    float x0 = position.x - size.x / 2.0f;
    float y0 = position.y - size.y / 2.0f;
    float x1 = position.x + size.x / 2.0f;
    float y1 = position.y + size.y / 2.0f;

    std::vector<Vertex> vertices =
    {
        { glm::vec2(x0, y1), glm::vec2(0.0f, 1.0f) },
        { glm::vec2(x0, y0), glm::vec2(0.0f, 0.0f) },
        { glm::vec2(x1, y0), glm::vec2(1.0f, 0.0f) },
        { glm::vec2(x0, y1),  glm::vec2(0.0f, 1.0f) },
        { glm::vec2(x1, y0),  glm::vec2(1.0f, 0.0f) },
        { glm::vec2(x1, y1),  glm::vec2(1.0f, 1.0f) },
    };

    return Mesh(vertices);
}
