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

SlimeSimulator::SlimeSimulator(int num_agents, const std::string &image_path)
    : num_agents(0),
    agent_shader("assets/shaders/agent.comp"),
    diffuse_shader("assets/shaders/diffuse.comp")
{
    assert(agent_shader.valid());
    assert(diffuse_shader.valid());

    int channels;
    float *data = stbi_loadf(image_path.c_str(),
            &size.x, &size.y, &channels, 4);
    if (!data)
    {
        std::cout << "Could not load image " << image_path << '\n';
        assert(false);
    }

    trail_texture.initialize(size, GL_RGBA32F);
    diffused_trail_texture.initialize(size, GL_RGBA32F);

    std::vector<glm::vec4> trail_pixels(size.x * size.y,
            glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    this->num_agents = std::max(num_agents, size.x * size.y);

    std::vector<Agent> agents(this->num_agents);
    for (int y = 0; y < size.y; y++)
    {
        for (int x = 0; x < size.x; x++)
        {
            Agent &agent = agents[y * size.x + x];
            agent.position.x = x + 0.5f;
            agent.position.y = y + 0.5f;
            agent.angle = Calc::frand() * 2.0f * glm::pi<float>();

            int y_inverse = size.y - y - 1;
            int offset = (y_inverse * size.x + x) * 4;
            agent.color.r = data[offset + 0];
            agent.color.g = data[offset + 1];
            agent.color.b = data[offset + 2];
            agent.color.a = data[offset + 3];

            trail_pixels[y * size.x + x] = agent.color;
        }
    }

    for (int i = size.x * size.y; i < this->num_agents; i++)
    {
        Agent &agent = agents[i];

        int rx = Calc::randrange(0, size.x);
        int ry = Calc::randrange(0, size.y);

        agent.position.x = rx + 0.5f;
        agent.position.y = ry + 0.5f;
        agent.angle = Calc::frand() * 2.0f * glm::pi<float>();

        int y_inverse = size.y - ry - 1;
        int offset = (y_inverse * size.x + rx) * 4;
        agent.color.r = data[offset + 0];
        agent.color.g = data[offset + 1];
        agent.color.b = data[offset + 2];
        agent.color.a = data[offset + 3];

        trail_pixels[ry * size.x + rx] = agent.color;
    }

    trail_texture.set_data(trail_pixels.data());

    trail_texture.bind_to_unit(trail_texture_unit);
    diffused_trail_texture.bind_to_unit(diffused_trail_texture_unit);

    glCreateBuffers(1, &vbo_agent);
    glNamedBufferData(vbo_agent, this->num_agents * sizeof(Agent),
            agents.data(), GL_STATIC_COPY);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vbo_agent);

    glm::uvec3 agent_work_group = glm::uvec3(std::ceil(this->num_agents / 64.0f),
            1, 1);
    glm::uvec3 image_work_group = glm::uvec3(std::ceil(size.x / 8.0f),
            std::ceil(size.y / 8.0f), 1);

    agent_shader.set_work_group(agent_work_group);
    diffuse_shader.set_work_group(image_work_group);

    agent_shader.bind();
    agent_shader.set_ivec2(bounds_index, size);
    agent_shader.set_int(num_agents_index, this->num_agents);

    diffuse_shader.bind();
    diffuse_shader.set_ivec2(bounds_index, size);

    stbi_image_free(data);
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
    diffuse_shader.set_float(dt_index, dt);
    diffuse_shader.set_float(time_index, Timer::time());
    diffuse_shader.set_float(diffuse_speed_index,
            diffuse_speed);
    diffuse_shader.set_float(decay_speed_index, decay_speed);
    diffuse_shader.dispatch_and_wait();

    trail_texture.copy(&diffused_trail_texture);
}

glm::ivec2 SlimeSimulator::map_size() const
{
    return size;
}

const Texture *SlimeSimulator::trail() const
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
