#include "slimesimulator.hpp"
#include <glad/glad.h>
#include <imgui.h>
#include <cmath>
#include <limits>
#include <iostream>
#include "calc.hpp"
#include "timer.hpp"
#include "graphics.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

SlimeSimulator::SlimeSimulator(int num_agents, const glm::ivec3 &size)
    : size(size), num_agents(num_agents),
    agent_shader("assets/shaders/agent.comp"),
    diffuse_shader("assets/shaders/diffuse.comp")
{
    assert(agent_shader.valid());
    assert(diffuse_shader.valid());

    trail_texture.initialize(size, GL_RGBA32F);
    diffused_trail_texture.initialize(size, GL_RGBA32F);

    std::vector<glm::vec4> trail_pixels(size.x * size.y * size.z,
            glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

    float maxrad = std::min(size.x, std::min(size.y, size.z)) / 2.0f;
    glm::vec3 center = glm::vec3(size) / 2.0f;

    std::vector<Agent> agents(this->num_agents);
    for (auto &agent : agents)
    {
        float randtheta = Calc::frand() * 2.0f * glm::pi<float>();
        float randphi = Calc::frand() * 2.0f * glm::pi<float>();
        float r = Calc::frand() * maxrad;
        agent.position.x = center.x +
            glm::sin(randphi) * glm::cos(randtheta) * r;
        agent.position.y = center.y +
            glm::sin(randphi) * glm::sin(randtheta) * r;
        agent.position.z = center.z + glm::cos(randphi) * r;
        agent.theta = randtheta + glm::pi<float>();
        agent.phi = randphi + glm::pi<float>();

        agent.color = glm::vec4(1.0f);

        int px = std::floor(agent.position.x);
        int py = std::floor(agent.position.y);
        int pz = std::floor(agent.position.z);

        trail_pixels[px + py * size.x + pz * size.y * size.x] = agent.color;
    }

    trail_texture.set_data(trail_pixels.data());

    trail_texture.bind_to_unit(trail_texture_unit);
    diffused_trail_texture.bind_to_unit(diffused_trail_texture_unit);

    glCreateBuffers(1, &vbo_agent);
    glNamedBufferData(vbo_agent, this->num_agents * sizeof(Agent),
            agents.data(), GL_STATIC_COPY);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vbo_agent);

    glm::uvec3 agent_work_group =
        glm::uvec3(std::ceil(this->num_agents / 64.0f), 1, 1);
    glm::uvec3 image_work_group = glm::uvec3(std::ceil(size.x / 8.0f),
            std::ceil(size.y / 8.0f), 1);

    agent_shader.set_work_group(agent_work_group);
    diffuse_shader.set_work_group(image_work_group);
}

SlimeSimulator::~SlimeSimulator()
{
    glDeleteBuffers(1, &vbo_agent);
}

void SlimeSimulator::update(float dt)
{
    float step_dt = dt / static_cast<float>(steps_per_frame);
    for (size_t i = 0; i < steps_per_frame; i++)
    {
        step_update(step_dt);
    }
}

void SlimeSimulator::step_update(float dt)
{
    agent_shader.bind();
    agent_shader.set_ivec3(bounds_index, size);
    agent_shader.set_int(num_agents_index, num_agents);
    agent_shader.set_float(dt_index, dt);
    agent_shader.set_float(time_index, Timer::time());
    agent_shader.set_float(move_speed_index, move_speed);
    agent_shader.set_float(turn_amount_index, turn_amount);
    agent_shader.set_float(trail_weight_index, trail_weight);
    agent_shader.set_float(sense_spacing_index, sense_spacing);
    agent_shader.set_int(sense_distance_index, sense_distance);
    agent_shader.set_int(sense_size_index, sense_size);
    agent_shader.dispatch_and_wait();

    diffuse_shader.bind();
    diffuse_shader.set_ivec3(bounds_index, size);
    diffuse_shader.set_float(dt_index, dt);
    diffuse_shader.set_float(time_index, Timer::time());
    diffuse_shader.set_float(diffuse_speed_index,
            diffuse_speed);
    diffuse_shader.set_float(decay_speed_index, decay_speed);
    diffuse_shader.dispatch_and_wait();

    trail_texture.copy(&diffused_trail_texture);
}

const Texture3D *SlimeSimulator::trail() const
{
    return &trail_texture;
}

void SlimeSimulator::update_debug_window()
{
    ImGui::Begin("Parameters");

    ImGui::DragFloat("Move Speed", &move_speed, 1.0f, 0.0f,
            (std::numeric_limits<float>::max)());
    ImGui::DragFloat("Turn Amount", &turn_amount, 1.0f, 0.0f,
            (std::numeric_limits<float>::max)());
    ImGui::DragFloat("Trail Weight", &trail_weight, 0.1f, 0.0f,
            (std::numeric_limits<float>::max)());
    ImGui::DragFloat("Sense Spacing",
            &sense_spacing, 1.0f, 0.0f, 180.0f);
    ImGui::DragInt("Sense Distance", &sense_distance, 1, 1, 100);
    ImGui::DragInt("Sense Size", &sense_size, 1, 1, 3);

    ImGui::DragFloat("Diffuse Speed", &diffuse_speed, 0.05f, 0.0f, 10.0f);
    ImGui::DragFloat("Decay Speed", &decay_speed, 0.05f, 0.0f, 10.0f);
    ImGui::DragInt("Blur Radius", &blur_radius, 1, 1, 5);

    ImGui::End();
}
