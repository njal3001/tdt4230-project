#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/gtx/string_cast.hpp>
#include "graphics.hpp"
#include "shader.hpp"
#include "particle.hpp"
#include "calc.hpp"
#include "timer.hpp"
#include "camera.hpp"

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

    ParticleSystem system(128 * 100, &compute_shader, &render_shader);

    Timer frame_timer;
    Timer update_timer;
    Timer render_timer;

    Camera camera(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 100.0f);
    float camera_speed = 2.0f;
    float camera_rotation_speed = 0.5f;

    while (!glfwWindowShouldClose(window))
    {
        float dt = frame_timer.delta();

        update_timer.reset();

        float camera_move_amount = camera_speed * dt;
        float camera_rotation_amount = camera_rotation_speed * dt;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            camera.move_forward(camera_move_amount);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            camera.move_backward(camera_move_amount);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            camera.move_right(camera_move_amount);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            camera.move_left(camera_move_amount);
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            camera.rotate(-camera_rotation_amount, glm::vec3(1.0f, 0.0f, 0.0f));
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            camera.rotate(camera_rotation_amount, glm::vec3(1.0f, 0.0f, 0.0f));
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            camera.rotate(camera_rotation_amount, glm::vec3(0.0f, 1.0f, 0.0f));
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            camera.rotate(-camera_rotation_amount, glm::vec3(0.0f, 1.0f, 0.0f));
        }

        system.update(dt);
        printf("Update: %f\n", update_timer.delta());

        render_timer.reset();
        Graphics::clear_screen();

        system.render(camera.matrix());

        glfwSwapBuffers(window);
        printf("Render: %f\n", render_timer.delta());

        glfwPollEvents();

        printf("Total: %f (%f FPS)\n", dt, 1.0f / dt);
    }

    glfwTerminate();

    return EXIT_SUCCESS;
}
