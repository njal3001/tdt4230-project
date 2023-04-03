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
        float angle;
        float padding;
    };

private:
    size_t num_agents;
    glm::ivec2 size;

    ComputeShader agent_shader;
    ComputeShader diffuse_shader;

    Texture trail_texture;
    Texture diffused_trail_texture;

    unsigned int vbo_agent;

    const unsigned int trail_texture_index = 0;
    const unsigned int diffused_trail_texture_index = 1;

    const unsigned int bounds_index = 2;
    const unsigned int dt_index = 3;

    const unsigned int num_agents_index = 4;
    const unsigned int move_speed_index = 5;
    const unsigned int turn_speed_index = 6;
    const unsigned int sense_spacing_index = 7;
    const unsigned int sense_distance_index = 8;
    const unsigned int sense_size_index = 9;

    const unsigned int diffuse_speed_index = 4;
    const unsigned int decay_speed_index = 5;

    const float move_speed = 300.0f;
    const float turn_speed = 1440.0f;
    const float sense_spacing = 45.0f;
    const int sense_distance = 9;
    const int sense_size = 1;

    const float diffuse_speed = 0.1f;
    const float decay_speed = 0.5f;

public:
    SlimeSimulator(size_t num_agents, const glm::ivec2 &size);
    ~SlimeSimulator();

    void update(float dt);
    const Texture *output() const;
};
