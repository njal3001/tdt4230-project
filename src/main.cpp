#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "graphics.hpp"
#include "shader.hpp"
#include "particle.hpp"
#include "calc.hpp"

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

    Shader shader("assets/shaders/particle.vert",
            "assets/shaders/particle.frag");
    if (!shader.valid())
    {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    shader.bind();

    glm::mat4 vp = glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 100.0f);
    shader.set_mat4("VP", vp);

    ParticleBuffer buffer(100000);
    for (size_t i = 0; i < buffer.count(); i++)
    {
        buffer.positions[i].x = Calc::randrange(-10.0f, 10.0f);
        buffer.positions[i].y = Calc::randrange(-10.0f, 10.0f);
        buffer.positions[i].z = -30.0f;
        buffer.colors[i].r = Calc::randrange(0.0f, 1.0f);
        buffer.colors[i].g = Calc::randrange(0.0f, 1.0f);
        buffer.colors[i].b = Calc::randrange(0.0f, 1.0f);
        buffer.colors[i].a = 1.0f;
    }

    ParticleRenderer renderer(&buffer);

    while (!glfwWindowShouldClose(window))
    {
        Graphics::clear_screen();
        renderer.render();

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();

    return EXIT_SUCCESS;
}
