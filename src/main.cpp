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

    ParticleSystem system(128 * 10000, &compute_shader, &render_shader);

    Timer frame_timer;
    Timer update_timer;
    Timer render_timer;

    while (!glfwWindowShouldClose(window))
    {
        float dt = frame_timer.delta();

        glm::vec2 cursor = Graphics::get_normalized_cursor_position();
        glm::vec3 ndc_cursor = glm::vec3(2.0f * cursor.x - 1.0f, 2.0f * cursor.y - 1.0f, 0.0f);

        update_timer.delta();

        int mouse_state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        if (mouse_state == GLFW_PRESS)
        {
            system.spawn(10000, ndc_cursor);
        }

        system.update(dt);
        printf("Update: %f\n", update_timer.delta());

        render_timer.delta();
        Graphics::clear_screen();
        system.render();

        glfwPollEvents();
        glfwSwapBuffers(window);

        printf("Render: %f\n", render_timer.delta());
        printf("Total: %f (%f FPS)\n", dt, 1.0f / dt);
    }

    glfwTerminate();

    return EXIT_SUCCESS;
}
