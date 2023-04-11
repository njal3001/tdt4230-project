#include "slimesimulator.hpp"
#include <GL/glew.h>
#include <imgui.h>
#include <cmath>
#include <limits>
#include <iostream>
#include "calc.hpp"
#include "timer.hpp"

SlimeSimulator::SlimeSimulator(size_t num_agents, const glm::ivec2 &size)
    : size(size), num_agents(num_agents),
    agent_shader("assets/shaders/agent.comp"),
    diffuse_shader("assets/shaders/diffuse.comp"),
    trail_texture(size, GL_RGBA32F),
    diffused_trail_texture(size, GL_RGBA32F)
{
    assert(agent_shader.valid());
    assert(diffuse_shader.valid());

    glm::vec2 center = glm::vec2(size) / 2.0f;
    float max_radius = std::min(size.x, size.y) / 2.0f;

    std::vector<Agent> agents(num_agents);
    for (size_t i = 0; i < num_agents; i++)
    {
        Agent &agent = agents[i];
        agent.position = center;
        agent.angle = Calc::frandrange(0.0f, 2.0f * glm::pi<float>());

        if (num_species == 1)
        {
            agent.species_index = 0;
            agent.species_mask = glm::vec4(1.0);
        }
        else
        {
            agent.species_index = Calc::randrange(0, this->num_species);
            agent.species_mask = glm::vec4(agent.species_index == 0,
                    agent.species_index == 1, agent.species_index == 2,
                    1.0f);
        }
    }

    glCreateBuffers(1, &this->vbo_agent);
    glNamedBufferData(this->vbo_agent, this->num_agents * sizeof(Agent),
            agents.data(), GL_STATIC_COPY);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, this->vbo_agent);

    std::vector<glm::vec4> trail_pixels(size.x * size.y,
            glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    this->trail_texture.set_data(trail_pixels.data());

    this->trail_texture.bind_to_unit(this->trail_texture_unit);
    this->diffused_trail_texture.bind_to_unit(
            this->diffused_trail_texture_unit);

    glm::uvec3 agent_work_group = glm::uvec3(
            std::ceil(this->num_agents / 64.0f), 1, 1);
    glm::uvec3 texture_work_group = glm::uvec3(std::ceil(this->size.x / 8.0f),
            std::ceil(this->size.y / 8.0f), 1);

    this->agent_shader.set_work_group(agent_work_group);
    this->diffuse_shader.set_work_group(texture_work_group);

    this->agent_shader.bind();
    this->agent_shader.set_ivec2(this->bounds_index, this->size);
    this->agent_shader.set_int(this->num_agents_index, this->num_agents);

    this->diffuse_shader.bind();
    this->diffuse_shader.set_ivec2(this->bounds_index, this->size);
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
    this->agent_shader.set_float(
            this->move_speed_index, this->move_speed);
    this->agent_shader.set_float(
            this->turn_speed_index, this->turn_speed);
    this->agent_shader.set_float(
            this->trail_weight_index, this->trail_weight);
    this->agent_shader.set_float(this->sense_spacing_index,
            this->sense_spacing);
    this->agent_shader.set_int(this->sense_distance_index,
            this->sense_distance);
    this->agent_shader.set_int(this->sense_size_index, this->sense_size);

    this->agent_shader.dispatch_and_wait();

    this->diffuse_shader.bind();
    this->diffuse_shader.set_float(this->dt_index, dt);
    this->diffuse_shader.set_float(this->time_index, Timer::time());
    this->diffuse_shader.set_float(
            this->diffuse_speed_index, this->diffuse_speed);
    this->diffuse_shader.set_float(
            this->decay_speed_index, this->decay_speed);

    this->diffuse_shader.dispatch_and_wait();

    this->trail_texture.copy(&this->diffused_trail_texture);
}

const Texture *SlimeSimulator::trail() const
{
    return &this->trail_texture;
}

void SlimeSimulator::update_debug_window()
{
    ImGui::Begin("Debug");

    ImGui::DragFloat("Move Speed", &this->move_speed, 1.0f, 0.0f,
            std::numeric_limits<float>::max());
    ImGui::DragFloat("Turn Speed", &this->turn_speed, 1.0f, 0.0f,
            std::numeric_limits<float>::max());
    ImGui::DragFloat("Trail Weight", &this->trail_weight, 1.0f, 0.0f,
            std::numeric_limits<float>::max());
    ImGui::DragFloat("Sense Spacing",
            &this->sense_spacing, 1.0f, 0.0f, 180.0f);
    ImGui::DragInt("Sense Distance", &this->sense_distance, 1, 1, 100);
    ImGui::DragInt("Sense Size", &this->sense_size, 1, 1, 3);

    ImGui::DragFloat("Diffuse Speed", &this->diffuse_speed, 0.1f, 0.0f, 5.0f);
    ImGui::DragFloat("Decay Speed", &this->decay_speed, 0.1f, 0.0f, 5.0f);

    ImGui::End();
}
