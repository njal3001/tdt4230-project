#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include "shader.hpp"
#include "texture.hpp"

class SlimeSimulator
{
private:
    struct Agent
    {
        glm::vec3 position;
        float theta;
        float phi;
        glm::vec3 padding;
        glm::vec4 color;
    };

private:
    glm::ivec3 size;
    int num_agents;

    ComputeShader agent_shader;
    ComputeShader diffuse_shader;

    Texture3D trail_texture;
    Texture3D diffused_trail_texture;

    unsigned int vbo_agent;

    const unsigned int trail_texture_unit = 0;
    const unsigned int diffused_trail_texture_unit = 1;

    const unsigned int bounds_index = 0;
    const unsigned int dt_index = 1;
    const unsigned int time_index = 2;

    const unsigned int num_agents_index = 3;
    const unsigned int move_speed_index = 4;
    const unsigned int turn_amount_index = 5;
    const unsigned int trail_weight_index = 6;
    const unsigned int sense_spacing_index = 7;
    const unsigned int sense_distance_index = 8;
    const unsigned int sense_size_index = 9;

    const unsigned int diffuse_speed_index = 3;
    const unsigned int decay_speed_index = 4;
    const unsigned int blur_radius_index = 5;

    const size_t steps_per_frame = 1;

    float move_speed = 1.0f;
    float turn_amount = 15.0f;
    float trail_weight = 1.0f;
    float sense_spacing = 15.0f;
    int sense_distance = 20;
    int sense_size = 1;

    float diffuse_speed = 3.0f;
    float decay_speed = 0.1f;
    int blur_radius = 1;

public:
    SlimeSimulator(int num_agents, const glm::ivec3 &size);
    ~SlimeSimulator();

    void update(float dt);

    const Texture3D *trail() const;

    void update_debug_window();

private:
    void step_update(float dt);
};
