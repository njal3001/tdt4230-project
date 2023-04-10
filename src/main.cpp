#include <iostream>
#include <GL/glew.h>
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
        return EXIT_FAILURE;
    }

    GLFWwindow *window = glfwCreateWindow(960, 540,
            "Slime Simulator", NULL, NULL);
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

    glm::ivec2 texture_size(960, 540);
    SlimeSimulator simulator(0.05f, texture_size);

    Timer frame_timer;

    Mesh quad = Mesh::quad(glm::vec2(0.0f), glm::vec2(2.0f));

    while (!glfwWindowShouldClose(window))
    {
        Graphics::begin_frame();
        simulator.update_debug_window();

        bool mouse_left_pressed =
            glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS;
        bool mouse_right_pressed =
            glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS;

        if (!ImGui::GetIO().WantCaptureMouse &&
                (mouse_left_pressed || mouse_right_pressed))
        {
            glm::vec2 cursor = Graphics::get_normalized_cursor_position();
            glm::ivec2 wall_position = cursor * glm::vec2(texture_size);

            if (mouse_left_pressed)
            {
                simulator.add_wall(wall_position);
            }
            else
            {
                simulator.remove_wall(wall_position);
            }
        }

        float dt = frame_timer.delta();
        simulator.update(dt);

        render_shader.bind();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glBindTextureUnit(0, simulator.agents()->get_id());
            quad.render();
        }
        else
        {
            glBindTextureUnit(0, simulator.trail()->get_id());
            quad.render();

            glBindTextureUnit(0, simulator.walls()->get_id());
            quad.render();
        }

        Graphics::end_frame();

        glfwSwapBuffers(window);
        glfwPollEvents();

        std::cout << "Frame: " << dt << " (FPS: " << 1.0f / dt << ")\n";
    }

    Graphics::shutdown();
    glfwTerminate();

    return EXIT_SUCCESS;
}
