#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "graphics.hpp"
#include "shader.hpp"
#include "particle.hpp"
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

    RenderShader render_shader("assets/shaders/particle.vert",
            "assets/shaders/particle.frag");
    ComputeShader compute_shader("assets/shaders/particle.comp");

    if (!render_shader.valid() || !compute_shader.valid())
    {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    render_shader.bind();

    // glm::mat4 vp = glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f,
    //         0.1f, 1000.0f);
    // render_shader.set_mat4("VP", vp);

    ParticleSystem system(128 * 10000, &compute_shader, &render_shader);

    Timer frame_timer;
    Timer update_timer;
    Timer render_timer;

    while (!glfwWindowShouldClose(window))
    {
        float dt = frame_timer.delta();

        update_timer.delta();
        system.update(dt);
        printf("Update: %f\n", update_timer.delta());

        render_timer.delta();
        Graphics::clear_screen();
        system.render();

        glfwPollEvents();
        glfwSwapBuffers(window);

        printf("Render: %f\n", render_timer.delta());
        printf("Total: %f\n", dt);
    }

    glfwTerminate();

    return EXIT_SUCCESS;
}
