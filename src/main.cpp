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

    SlimeSimulator simulator(1000000, "assets/images/leonid2.jpg");

    glm::ivec2 size = simulator.map_size();
    Graphics::set_aspect(size.x, size.y);

    Timer frame_timer;

    Mesh quad = Mesh::quad(glm::vec2(0.0f), glm::vec2(2.0f));

    bool run_simulation = false;
    bool space_down = false;

    while (!glfwWindowShouldClose(window))
    {
        bool new_space_down = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
        if (!space_down && new_space_down)
        {
            run_simulation = !run_simulation;
        }

        space_down = new_space_down;

        Graphics::begin_frame();
        simulator.update_debug_window();

        float dt = frame_timer.delta();

        if (run_simulation)
        {
            simulator.update(dt);
        }

        render_shader.bind();
        glBindTextureUnit(0, simulator.trail()->get_id());

        quad.render();

        Graphics::end_frame();

        glfwSwapBuffers(window);
        glfwPollEvents();

        // std::cout << "Frame: " << dt << " (FPS: " << 1.0f / dt << ")\n";
    }

    Graphics::shutdown();
    glfwTerminate();

    return EXIT_SUCCESS;
}
