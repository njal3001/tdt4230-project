#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

namespace Graphics
{
    void initialize(GLFWwindow *window);
    void shutdown();

    void begin_frame();
    void end_frame();

    void set_aspect(int width, int height);

    glm::vec2 get_normalized_cursor_position();
};
