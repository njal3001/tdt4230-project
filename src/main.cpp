#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include "graphics.hpp"
#include "shader.hpp"
#include "slimesimulator.hpp"
#include "calc.hpp"
#include "timer.hpp"
#include "mesh.hpp"
#include "camera.hpp"

int main()
{
    if (!glfwInit())
    {
        std::cout << "Could not initialize GLFW\n";
        return EXIT_FAILURE;
    }

    GLFWwindow *window = glfwCreateWindow(960, 540,
            "Slime Simulator", NULL, NULL);
    if (!window)
    {
        std::cout << "Could not create GLFW window\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    gladLoadGL();

    Graphics::initialize(window);

    RenderShader render_shader("assets/shaders/render.vert",
            "assets/shaders/render.frag");
    assert(render_shader.valid());

    glm::ivec3 volume_size(100, 100, 100);

    SlimeSimulator simulator(1000000, volume_size);

    Graphics::set_aspect(1920, 1080);

    Timer frame_timer;

    Camera camera(45.0f, 1920.0f / 1080.0f, 0.1f, 10.0f);
    camera.move_backward(4.0f);

    // Mesh quad = Mesh::quad(glm::vec3(0.0f), glm::vec2(2.0f));
    Mesh cube = Mesh::cube(glm::vec3(0.0f), 2.0f);
    glm::mat4 cube_rotation = glm::mat4(1.0f);

    bool run_simulation = false;
    bool space_down = false;

    const float rotation_speed = 1.0f;

    while (!glfwWindowShouldClose(window))
    {
        float dt = frame_timer.delta();

        if (glfwGetKey(window, GLFW_KEY_UP))
            cube_rotation = glm::rotate(glm::mat4(1.0f), rotation_speed * dt, glm::vec3(1.0f, 0.0f, 0.0f)) * cube_rotation;
        else if (glfwGetKey(window, GLFW_KEY_DOWN))
            cube_rotation = glm::rotate(glm::mat4(1.0f), -rotation_speed * dt, glm::vec3(1.0f, 0.0f, 0.0f)) * cube_rotation;
        if (glfwGetKey(window, GLFW_KEY_RIGHT))
            cube_rotation = glm::rotate(glm::mat4(1.0f), rotation_speed * dt, glm::vec3(0.0f, 1.0f, 0.0f)) * cube_rotation;
        else if (glfwGetKey(window, GLFW_KEY_LEFT))
            cube_rotation = glm::rotate(glm::mat4(1.0f), -rotation_speed * dt, glm::vec3(0.0f, 1.0f, 0.0f)) * cube_rotation;

        bool new_space_down = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
        if (!space_down && new_space_down)
        {
            run_simulation = !run_simulation;
        }

        space_down = new_space_down;

        Graphics::begin_frame();
        simulator.update_debug_window();


        if (run_simulation)
        {
            simulator.update(dt);
        }

        render_shader.bind();
        render_shader.set_mat4("model", cube_rotation);
        render_shader.set_mat4("view_projection", camera.matrix());
        render_shader.set_ivec3("volume_size", volume_size);

        glBindTextureUnit(0, simulator.trail()->get_id());

        // quad.render();
        cube.render();

        Graphics::end_frame();

        glfwSwapBuffers(window);
        glfwPollEvents();

        // std::cout << "Frame: " << dt << " (FPS: " << 1.0f / dt << ")\n";
    }

    Graphics::shutdown();
    glfwTerminate();

    return EXIT_SUCCESS;
}
