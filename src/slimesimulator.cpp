#include "slimesimulator.hpp"
#include <GL/glew.h>
#include <imgui.h>
#include <cmath>
#include <limits>
#include <iostream>
#include "calc.hpp"
#include "timer.hpp"
#include "graphics.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

SlimeSimulator::SlimeSimulator(float agent_percentage, const glm::ivec2 &size)
    : size(size), num_agents(0),
    agent_shader("assets/shaders/agent.comp"),
    diffuse_shader("assets/shaders/diffuse.comp"),
    occupied_shader("assets/shaders/occupied.comp"),
    trail_texture(size, GL_RGBA32F),
    diffused_trail_texture(size, GL_RGBA32F),
    occupied_texture(size, GL_R32UI),
    agent_texture(size, GL_RGBA32F),
    wall_texture(size, GL_RGBA32F)
{
    assert(agent_shader.valid());
    assert(diffuse_shader.valid());
    assert(occupied_shader.valid());

    std::vector<glm::vec4> trail_pixels(size.x * size.y,
            glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    std::vector<unsigned int> occupied_pixels(size.x * size.y, 0);

    std::vector<glm::vec4> wall_pixels(size.x * size.y, glm::vec4(0.0f));

    for (size_t i = 0; i < 5 * size.x; i++)
    {
        wall_pixels[i + size.x * (size.y / 2)] = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
    }

    std::vector<Agent> agents;
    for (int y = 0; y < size.y; y++)
    {
        for (int x = 0; x < size.x; x++)
        {
            if (wall_pixels[y * size.x + x].a > 0.0f)
                continue;

            if (Calc::frand() > agent_percentage)
                continue;

            Agent agent;
            agent.position.x = x + 0.5f;
            agent.position.y = y + 0.5f;
            agent.angle = Calc::frandrange(0.0f, 2.0f * glm::pi<float>());

            agents.push_back(agent);
            this->num_agents++;

            glm::ivec2 pixel = glm::ivec2(agent.position);
            occupied_pixels[pixel.y * this->size.x + pixel.x] =
                this->num_agents;
            trail_pixels[pixel.y * this->size.x + pixel.x] = glm::vec4(1.0);
        }
    }

    this->trail_texture.set_data(trail_pixels.data());
    this->occupied_texture.set_data(occupied_pixels.data());
    this->agent_texture.set_data(trail_pixels.data());
    this->wall_texture.set_data(wall_pixels.data());

    this->trail_texture.bind_to_unit(this->trail_texture_unit);
    this->diffused_trail_texture.bind_to_unit(
            this->diffused_trail_texture_unit);
    this->occupied_texture.bind_to_unit(this->occupied_texture_unit);
    this->agent_texture.bind_to_unit(this->agent_texture_unit);
    this->wall_texture.bind_to_unit(this->wall_texture_unit);

    glCreateBuffers(1, &this->vbo_agent);
    glNamedBufferData(this->vbo_agent, this->num_agents * sizeof(Agent),
            agents.data(), GL_STATIC_COPY);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, this->vbo_agent);

    glm::uvec3 work_group = glm::uvec3(std::ceil(this->size.x / 8.0f),
            std::ceil(this->size.y / 8.0f), 1);

    this->agent_shader.set_work_group(work_group);
    this->diffuse_shader.set_work_group(work_group);
    this->occupied_shader.set_work_group(work_group);

    this->agent_shader.bind();
    this->agent_shader.set_ivec2(this->bounds_index, this->size);
    this->agent_shader.set_int(this->num_agents_index, this->num_agents);

    this->diffuse_shader.bind();
    this->diffuse_shader.set_ivec2(this->bounds_index, this->size);

    this->occupied_shader.bind();
    this->occupied_shader.set_ivec2(this->bounds_index, this->size);
}

SlimeSimulator::~SlimeSimulator()
{
    glDeleteBuffers(1, &this->vbo_agent);
}

void SlimeSimulator::update(float dt)
{
    float step_dt = dt / static_cast<float>(this->steps_per_frame);
    for (size_t i = 0; i < this->steps_per_frame; i++)
    {
        this->step_update(step_dt);
    }
}

void SlimeSimulator::step_update(float dt)
{
    this->agent_shader.bind();
    this->agent_shader.set_float(this->dt_index, dt);
    this->agent_shader.set_float(this->time_index, Timer::time());
    this->agent_shader.set_float(this->move_speed_index, this->move_speed);
    this->agent_shader.set_float(this->turn_amount_index, this->turn_amount);
    this->agent_shader.set_float(this->trail_weight_index, this->trail_weight);
    this->agent_shader.set_float(this->sense_spacing_index, this->sense_spacing);
    this->agent_shader.set_int(this->sense_distance_index, this->sense_distance);
    this->agent_shader.set_int(this->sense_size_index, this->sense_size);
    this->agent_shader.dispatch_and_wait();

    this->diffuse_shader.bind();
    this->diffuse_shader.set_float(this->dt_index, dt);
    this->diffuse_shader.set_float(this->time_index, Timer::time());
    this->diffuse_shader.set_float(this->diffuse_speed_index,
            this->diffuse_speed);
    this->diffuse_shader.set_float(this->decay_speed_index, this->decay_speed);
    this->diffuse_shader.dispatch_and_wait();

    this->occupied_shader.bind();
    this->occupied_shader.set_float(this->dt_index, dt);
    this->occupied_shader.set_float(this->time_index, Timer::time());
    this->occupied_shader.dispatch_and_wait();

    this->trail_texture.copy(&this->diffused_trail_texture);
}

const Texture *SlimeSimulator::trail() const
{
    return &this->trail_texture;
}

const Texture *SlimeSimulator::agents() const
{
    return &this->agent_texture;
}

const Texture *SlimeSimulator::walls() const
{
    return &this->wall_texture;
}

void SlimeSimulator::update_debug_window()
{
    ImGui::Begin("Debug");

    ImGui::DragFloat("Move Speed", &this->move_speed, 1.0f, 0.0f,
            std::numeric_limits<float>::max());
    ImGui::DragFloat("Turn Amount", &this->turn_amount, 1.0f, 0.0f,
            std::numeric_limits<float>::max());
    ImGui::DragFloat("Trail Weight", &this->trail_weight, 0.1f, 0.0f,
            std::numeric_limits<float>::max());
    ImGui::DragFloat("Sense Spacing",
            &this->sense_spacing, 1.0f, 0.0f, 180.0f);
    ImGui::DragInt("Sense Distance", &this->sense_distance, 1, 1, 100);
    ImGui::DragInt("Sense Size", &this->sense_size, 1, 1, 3);

    ImGui::DragFloat("Diffuse Speed", &this->diffuse_speed, 0.05f, 0.0f, 10.0f);
    ImGui::DragFloat("Decay Speed", &this->decay_speed, 0.05f, 0.0f, 10.0f);
    ImGui::DragInt("Blur Radius", &this->blur_radius, 1, 1, 5);

    ImGui::End();
}
