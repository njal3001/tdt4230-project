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

SlimeSimulator::SlimeSimulator()
    : size(0), num_agents(0),
    agent_shader("assets/shaders/agent.comp"),
    diffuse_shader("assets/shaders/diffuse.comp")
{
    assert(agent_shader.valid());
    assert(diffuse_shader.valid());
}

void SlimeSimulator::initialize(const std::vector<Agent> &agents,
        const glm::ivec2 &size)
{
    // FIXME: Remove global state
    Graphics::set_aspect(size.x, size.y);

    this->num_agents = agents.size();
    this->size = size;

    this->trail_texture.initialize(size, GL_RGBA32F);
    this->diffused_trail_texture.initialize(size, GL_RGBA32F);

    glCreateBuffers(1, &this->vbo_agent);
    glNamedBufferData(this->vbo_agent, this->num_agents * sizeof(Agent),
            agents.data(), GL_STATIC_COPY);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, this->vbo_agent);

    std::vector<glm::vec4> trail_pixels(size.x * size.y,
            glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    for (auto &agent : agents)
    {
        glm::ivec2 pixel = glm::ivec2(agent.position);
        trail_pixels[pixel.y * this->size.x + pixel.x] = agent.color;
    }

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

SlimeSimulator::SlimeSimulator(size_t num_agents, const glm::ivec2 &size,
        SpawnType spawn_type)
    : SlimeSimulator()
{
    glm::vec2 center = glm::vec2(size) / 2.0f;
    float max_radius = std::min(size.x, size.y) / 2.0f;

    std::vector<Agent> agents(num_agents);
    for (size_t i = 0; i < num_agents; i++)
    {
        Agent &agent = agents[i];
        switch (spawn_type)
        {
            case SpawnType::inward_circle:
            {
                float angle = Calc::frandrange(0.0f, 2.0f * glm::pi<float>());
                agent.position = center +
                    glm::vec2(glm::cos(angle), glm::sin(angle)) *
                    Calc::frandrange(0.0f, max_radius);
                agent.angle = glm::pi<float>() + angle;

                break;
            }
            case SpawnType::square:
            {
                agent.position.x = Calc::frandrange(0.0f, size.x);
                agent.position.y = Calc::frandrange(0.0f, size.y);
                agent.angle = Calc::frandrange(0.0f, 2.0f * glm::pi<float>());

                break;
            }
            case SpawnType::center:
            {
                agent.position = center;
                agent.angle = Calc::frandrange(0.0f, 2.0f * glm::pi<float>());

                break;
            }
        }

        if (Calc::frand() > 0.5f)
        {
            agent.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        }
        else
        {
            agent.color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
        }

        // agent.color = glm::vec4(1.0f);
    }

    this->initialize(agents, size);
}

SlimeSimulator::SlimeSimulator(size_t num_agents, const std::string &image_path)
    : SlimeSimulator()
{
    int channels;
    glm::ivec2 size;

    unsigned char *data = stbi_load(image_path.c_str(),
            &size.x, &size.y, &channels, 4);

    assert(data);

    std::vector<Agent> agents(num_agents);
    for (size_t i = 0; i < num_agents; i++)
    {
        Agent &agent = agents[i];

        agent.position.x = Calc::frandrange(0.0f, size.x);
        agent.position.y = Calc::frandrange(0.0f, size.y);
        agent.angle = Calc::frandrange(0.0f, 2.0f * glm::pi<float>());

        int x = agent.position.x;
        int y = agent.position.y;

        int y_inverse = size.y - y - 1;
        int offset = (y_inverse * size.x + x) * 4;
        unsigned char r = data[offset + 0];
        unsigned char g = data[offset + 1];
        unsigned char b = data[offset + 2];
        unsigned char a = data[offset + 3];

        agent.color.r = r / 255.0f;
        agent.color.g = g / 255.0f;
        agent.color.b = b / 255.0f;
        agent.color.a = a / 255.0f;
    }

    // for (int y = 0; y < size.y; y++)
    // {
    //     for (int x = 0; x < size.x; x++)
    //     {
    //         int y_inverse = size.y - y - 1;
    //         int offset = (y_inverse * size.x + x) * 4;
    //         unsigned char r = data[offset + 0];
    //         unsigned char g = data[offset + 1];
    //         unsigned char b = data[offset + 2];
    //         unsigned char a = data[offset + 3];
    //
    //         Agent &agent = agents[y * size.x + x];
    //         agent.position = glm::vec2(x + 0.5f, y + 0.5f);
    //         agent.angle = Calc::frandrange(0.0f, 2.0f * glm::pi<float>());
    //
    //         agent.color.r = r / 255.0f;
    //         agent.color.g = g / 255.0f;
    //         agent.color.b = b / 255.0f;
    //         agent.color.a = a / 255.0f;
    //     }
    // }

    stbi_image_free(data);

    this->initialize(agents, size);
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
            this->turn_amount_index, this->turn_amount);
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
            this->decay_index, this->decay);

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
    ImGui::DragFloat("Turn Amount", &this->turn_amount, 1.0f, 0.0f,
            std::numeric_limits<float>::max());
    ImGui::DragFloat("Trail Weight", &this->trail_weight, 1.0f, 0.0f,
            std::numeric_limits<float>::max());
    ImGui::DragFloat("Sense Spacing",
            &this->sense_spacing, 1.0f, 0.0f, 180.0f);
    ImGui::DragInt("Sense Distance", &this->sense_distance, 1, 1, 100);
    ImGui::DragInt("Sense Size", &this->sense_size, 1, 1, 3);

    ImGui::DragFloat("Diffuse Speed", &this->diffuse_speed, 0.1f, 0.0f, 5.0f);
    ImGui::DragFloat("Decay", &this->decay, 0.01f, 0.0f, 1.0f);
    ImGui::DragInt("Blur Radius", &this->blur_radius, 1, 1, 5);

    ImGui::End();
}
