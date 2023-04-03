#include "slimesimulator.hpp"
#include <GL/glew.h>
#include "calc.hpp"
#include <cmath>

SlimeSimulator::SlimeSimulator(size_t num_agents, const glm::ivec2 &size)
    : num_agents(num_agents), size(size),
    agent_shader("assets/shaders/agent.comp"),
    diffuse_shader("assets/shaders/diffuse.comp"),
    trail_texture(size),
    diffused_trail_texture(size)

{
    assert(agent_shader.valid());
    assert(diffuse_shader.valid());

    std::vector<Agent> agents(num_agents);
    for (Agent &agent : agents)
    {
        agent.position.x = Calc::frandrange(0.0f, size.x);
        agent.position.y = Calc::frandrange(0.0f, size.y);

        agent.angle = Calc::frandrange(0.0f, 2.0f * glm::pi<float>());
    }

    glCreateBuffers(1, &this->vbo_agent);
    glNamedBufferData(this->vbo_agent, num_agents * sizeof(Agent),
            agents.data(), GL_STATIC_COPY);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, this->vbo_agent);

    std::vector<glm::vec4> pixels(size.x * size.y,
            glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    this->trail_texture.set_data(pixels.data());
    this->diffused_trail_texture.set_data(pixels.data());

    this->agent_shader.bind();
    this->agent_shader.set_ivec2(this->bounds_index, this->size);
    this->agent_shader.set_int(this->num_agents_index, num_agents);
    this->agent_shader.set_float(this->move_speed_index, this->move_speed);
    this->agent_shader.set_float(this->turn_speed_index, this->turn_speed);
    this->agent_shader.set_float(this->sense_spacing_index,
            this->sense_spacing);
    this->agent_shader.set_int(this->sense_distance_index,
            this->sense_distance);
    this->agent_shader.set_int(this->sense_size_index, this->sense_size);

    glBindImageTexture(this->trail_texture_index, this->trail_texture.get_id(),
            0, false, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(this->diffused_trail_texture_index,
            this->diffused_trail_texture.get_id(), 0, false, 0,
            GL_READ_ONLY, GL_RGBA32F);

    this->diffuse_shader.bind();
    this->agent_shader.set_ivec2(this->bounds_index, this->size);
    this->agent_shader.set_float(this->diffuse_speed_index,
            this->diffuse_speed);
    this->agent_shader.set_float(this->decay_speed_index,
            this->decay_speed);

    glBindImageTexture(this->trail_texture_index, this->trail_texture.get_id(),
            0, false, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(this->diffused_trail_texture_index,
            this->diffused_trail_texture.get_id(), 0, false, 0,
            GL_READ_ONLY, GL_RGBA32F);
}

SlimeSimulator::~SlimeSimulator()
{
    glDeleteBuffers(1, &this->vbo_agent);
}

void SlimeSimulator::update(float dt)
{
    this->agent_shader.bind();
    this->agent_shader.set_float(this->dt_index, dt);

    glDispatchCompute(std::ceil(this->num_agents / 128.0f), 1, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    this->diffuse_shader.bind();
    this->diffuse_shader.set_float(this->dt_index, dt);

    glDispatchCompute(std::ceil(this->size.x / 8.0f),
            std::ceil(this->size.y / 8.0f), 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    glCopyImageSubData(this->diffused_trail_texture.get_id(), GL_TEXTURE_2D,
            0, 0, 0, 0, this->trail_texture.get_id(), GL_TEXTURE_2D, 0,
            0, 0, 0, this->size.x, this->size.y, 1);
}

const Texture *SlimeSimulator::output() const
{
    return &this->trail_texture;
}
