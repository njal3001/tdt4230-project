#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Mesh
{
public:
    struct Vertex
    {
        glm::vec3 position;
        glm::vec2 uv;
    };

private:
    unsigned int vao;
    unsigned int vbo;
    size_t num_vertices;

public:
    Mesh(const std::vector<Vertex> &vertices);
    ~Mesh();

    void render() const;

    static Mesh quad(const glm::vec3 &position, const glm::vec2 &size);
    static Mesh cube(const glm::vec3 &position, float size);
};
