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
        int species_index;
        glm::vec4 species_mask;
    };

private:
    glm::ivec2 size;
    size_t num_agents;

    ComputeShader agent_move_shader;
    ComputeShader agent_sense_shader;
    ComputeShader diffuse_shader;
    ComputeShader occupied_shader;

    Texture occupied_texture;
    Texture trail_texture;
    Texture diffused_trail_texture;
    Texture agent_texture;

    unsigned int vbo_agent;

    const unsigned int trail_texture_unit = 0;
    const unsigned int diffused_trail_texture_unit = 1;
    const unsigned int agent_texture_unit = 2;
    const unsigned int occupied_texture_unit = 3;

    const unsigned int bounds_index = 0;
    const unsigned int dt_index = 1;
    const unsigned int time_index = 2;

    const unsigned int num_agents_index = 3;
    const unsigned int move_speed_index = 4;
    const unsigned int turn_speed_index = 5;
    const unsigned int trail_weight_index = 6;
    const unsigned int sense_spacing_index = 7;
    const unsigned int sense_distance_index = 8;
    const unsigned int sense_size_index = 9;

    const unsigned int diffuse_speed_index = 3;
    const unsigned int decay_speed_index = 4;

    const size_t steps_per_frame = 2;
    const size_t num_species = 1;

    float move_speed = 30.0f;
    float turn_speed = 360.0f;
    float trail_weight = 50.0f;
    float sense_spacing = 45.0f;
    int sense_distance = 9;
    int sense_size = 1;

    float diffuse_speed = 3.0f;
    float decay_speed = 0.2f;

public:
    SlimeSimulator(float agent_percentage, const glm::ivec2 &size);
    ~SlimeSimulator();

    void update(float dt);

    const Texture *trail() const;
    const Texture *agents() const;

    void update_debug_window();

private:
    void step_update(float dt);
};
