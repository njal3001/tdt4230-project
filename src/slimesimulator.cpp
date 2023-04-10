#include "slimesimulator.hpp"
#include <GL/glew.h>
#include <imgui.h>
#include <cmath>
#include <limits>
#include <iostream>
#include "calc.hpp"
#include "timer.hpp"

SlimeSimulator::SlimeSimulator(float agent_percentage, const glm::ivec2 &size)
    : size(size), num_agents(0),
    agent_move_shader("assets/shaders/agent_move.comp"),
    agent_sense_shader("assets/shaders/agent_sense.comp"),
    diffuse_shader("assets/shaders/diffuse.comp"),
    occupied_shader("assets/shaders/occupied.comp"),
    occupied_texture(size, GL_R32UI),
    wall_texture(size, GL_RGBA32F),
    trail_texture(size, GL_RGBA32F),
    diffused_trail_texture(size, GL_RGBA32F),
    agent_texture(size, GL_RGBA32F)
{
    assert(agent_move_shader.valid());
    assert(agent_sense_shader.valid());
    assert(diffuse_shader.valid());
    assert(occupied_shader.valid());

    glm::vec2 center = glm::vec2(size) / 2.0f;
    float max_radius = std::min(size.x, size.y) / 2.0f;

    std::vector<Agent> agents;
    std::vector<unsigned int> occupied_pixels(size.x * size.y, 0);

    for (int y = 0; y < size.y; y++)
    {
        for (int x = 0; x < size.x; x++)
        {
            if (agent_percentage < Calc::frand())
            {
                continue;
            }

            Agent agent;
            agent.position = glm::vec2(x + 0.5f, y + 0.5f);
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

            agents.push_back(agent);

            this->num_agents++;
            occupied_pixels[y * size.x + x] = this->num_agents;
        }
    }

    glCreateBuffers(1, &this->vbo_agent);
    glNamedBufferData(this->vbo_agent, this->num_agents * sizeof(Agent),
            agents.data(), GL_STATIC_COPY);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, this->vbo_agent);

    std::vector<glm::vec4> trail_pixels(size.x * size.y,
            glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    std::vector<glm::vec4> wall_pixels(size.x * size.y, glm::vec4(0.0));

    this->occupied_texture.set_data(occupied_pixels.data());
    this->wall_texture.set_data(wall_pixels.data());
    this->trail_texture.set_data(trail_pixels.data());

    this->occupied_texture.bind_to_unit(this->occupied_texture_unit);
    this->wall_texture.bind_to_unit(this->wall_texture_unit);
    this->agent_texture.bind_to_unit(this->agent_texture_unit);
    this->trail_texture.bind_to_unit(this->trail_texture_unit);
    this->diffused_trail_texture.bind_to_unit(
            this->diffused_trail_texture_unit);

    glm::uvec3 agent_work_group = glm::uvec3(
            std::ceil(this->num_agents / 64.0f), 1, 1);
    glm::uvec3 texture_work_group = glm::uvec3(std::ceil(this->size.x / 8.0f),
            std::ceil(this->size.y / 8.0f), 1);

    this->agent_move_shader.set_work_group(agent_work_group);
    this->agent_sense_shader.set_work_group(agent_work_group);
    this->diffuse_shader.set_work_group(texture_work_group);
    this->occupied_shader.set_work_group(texture_work_group);

    this->agent_move_shader.bind();
    this->agent_move_shader.set_ivec2(this->bounds_index, this->size);
    this->agent_move_shader.set_int(this->num_agents_index, this->num_agents);

    this->agent_sense_shader.bind();
    this->agent_sense_shader.set_ivec2(this->bounds_index, this->size);
    this->agent_sense_shader.set_int(this->num_agents_index, this->num_agents);

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
    this->agent_move_shader.bind();
    this->agent_move_shader.set_float(this->dt_index, dt);
    this->agent_move_shader.set_float(this->time_index, Timer::time());
    this->agent_move_shader.set_float(
            this->move_speed_index, this->move_speed);
    this->agent_move_shader.set_float(
            this->turn_speed_index, this->turn_speed);
    this->agent_move_shader.set_float(
            this->trail_weight_index, this->trail_weight);
    this->agent_move_shader.set_float(this->sense_spacing_index,
            this->sense_spacing);
    this->agent_move_shader.set_int(this->sense_distance_index,
            this->sense_distance);
    this->agent_move_shader.set_int(this->sense_size_index, this->sense_size);

    this->agent_move_shader.dispatch_and_wait();

    this->agent_sense_shader.bind();
    this->agent_sense_shader.set_float(this->dt_index, dt);
    this->agent_sense_shader.set_float(this->time_index, Timer::time());
    this->agent_sense_shader.set_float(
            this->move_speed_index, this->move_speed);
    this->agent_sense_shader.set_float(
            this->turn_speed_index, this->turn_speed);
    this->agent_sense_shader.set_float(
            this->trail_weight_index, this->trail_weight);
    this->agent_sense_shader.set_float(
            this->sense_spacing_index, this->sense_spacing);
    this->agent_sense_shader.set_int(
            this->sense_distance_index, this->sense_distance);
    this->agent_sense_shader.set_int(
            this->sense_size_index, this->sense_size);

    this->agent_sense_shader.dispatch_and_wait();

    this->diffuse_shader.bind();
    this->diffuse_shader.set_float(this->dt_index, dt);
    this->diffuse_shader.set_float(this->time_index, Timer::time());
    this->diffuse_shader.set_float(
            this->diffuse_speed_index, this->diffuse_speed);
    this->diffuse_shader.set_float(
            this->decay_speed_index, this->decay_speed);

    this->diffuse_shader.dispatch_and_wait();

    this->occupied_shader.bind();
    this->occupied_shader.set_float(this->dt_index, dt);
    this->occupied_shader.set_float(this->time_index, Timer::time());

    this->occupied_shader.dispatch_and_wait();

    glCopyImageSubData(this->diffused_trail_texture.get_id(), GL_TEXTURE_2D,
            0, 0, 0, 0, this->trail_texture.get_id(), GL_TEXTURE_2D, 0,
            0, 0, 0, this->size.x, this->size.y, 1);
}

void SlimeSimulator::add_wall(const glm::ivec2 &position)
{
    this->update_wall(position,
            glm::vec4(0.3f, 0.3f, 0.3f, 1.0f), this->brush_size);
}

void SlimeSimulator::remove_wall(const glm::ivec2 &position)
{
    this->update_wall(position,
            glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), this->eraser_size);
}

void SlimeSimulator::update_wall(const glm::ivec2 &position,
        const glm::vec4 &value, int size)
{
    int x_min = Calc::mid(0, position.x - size, this->size.x);
    int y_min = Calc::mid(0, position.y - size, this->size.y);
    int x_max = Calc::mid(0, position.x + size, this->size.x);
    int y_max = Calc::mid(0, position.y + size, this->size.y);

    int width = x_max - x_min;
    int heigth = y_max - y_min;

    if (width <= 0 || heigth <= 0)
    {
        return;
    }

    std::vector<glm::vec4> pixels(heigth * width, value);

    this->wall_texture.set_sub_data(pixels.data(),
            x_min, y_min, width, heigth);
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

    ImGui::DragInt("Brush Size", &this->brush_size, 1, 1, 100);
    ImGui::DragInt("Eraser Size", &this->eraser_size, 1, 1, 100);

    ImGui::End();
}
