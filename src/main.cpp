#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "graphics.hpp"
#include "shader.hpp"
#include "slimesimulator.hpp"
#include "calc.hpp"
#include "timer.hpp"

int main()
{
    if (!glfwInit())
    {
        return EXIT_FAILURE;
    }

    GLFWwindow *window = glfwCreateWindow(960, 540,
            "Particle System", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    Graphics::initialize(window);

    RenderShader render_shader("assets/shaders/render.vert",
            "assets/shaders/render.frag");
    assert(render_shader.valid());

    SlimeSimulator simulator(200000, glm::ivec2(1280, 720));

    Timer frame_timer;

    render_shader.bind();
    glBindTextureUnit(0, simulator.output()->get_id());

    glm::vec4 vertices[] =
    {
        glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f),
        glm::vec4(-1.0f, -1.0f, 0.0f, 0.0f),
        glm::vec4(1.0f, -1.0f, 1.0f, 0.0f),
        glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f),
        glm::vec4(1.0f, -1.0f, 1.0f, 0.0f),
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
    };

    unsigned int vao;
    unsigned int vbo;

    glCreateVertexArrays(1, &vao);
    glCreateBuffers(1, &vbo);
    glNamedBufferData(vbo, sizeof(vertices), vertices, GL_STATIC_COPY);

    glVertexArrayVertexBuffer(vao, 0, vbo, 0, 4 * sizeof(float));

    glEnableVertexArrayAttrib(vao, 0);
    glEnableVertexArrayAttrib(vao, 1);

    glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float));

    glVertexArrayAttribBinding(vao, 0, 0);
    glVertexArrayAttribBinding(vao, 1, 0);

    while (!glfwWindowShouldClose(window))
    {
        Graphics::begin_frame();
        simulator.update_debug_window();

        float dt = frame_timer.delta();
        simulator.update(dt);

        glBindVertexArray(vao);
        render_shader.bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);

        Graphics::end_frame();

        glfwSwapBuffers(window);
        glfwPollEvents();

        std::cout << "Frame: " << dt << " (FPS: " << 1.0f / dt << ")\n";
    }

    Graphics::shutdown();
    glfwTerminate();

    return EXIT_SUCCESS;
}
