#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include "shader.hpp"
#include "texture.hpp"

class SlimeSimulator
{
private:
    struct Agent
    {
        glm::vec2 position;
        glm::vec2 velocity;
    };

private:
    size_t num_agents;
    glm::ivec2 size;

    ComputeShader agent_shader;
    ComputeShader diffuse_shader;

    Texture trail_texture;
    Texture diffused_trail_texture;

    unsigned int vbo_agent;

public:
    SlimeSimulator(size_t num_agents, const glm::ivec2 &size);
    ~SlimeSimulator();

    void update(float dt);
    const Texture *output() const;
};
