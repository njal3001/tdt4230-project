#include "graphics.hpp"
#include <stdio.h>
#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

#ifdef _WIN32
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#else
#    define APIENTRY
#endif

namespace
{
    int window_width;
    int window_height;
    double cursorx;
    double cursory;
    int view_width;
    int view_height;
    int aspect_width;
    int aspect_height;
};

static void APIENTRY gl_message_callback(GLenum source, GLenum type, GLuint id,
                                  GLenum severity, GLsizei length,
                                  const GLchar *message,
                                  const void *user_param)
{
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION &&
        type == GL_DEBUG_TYPE_OTHER)
    {
        return;
    }

    const char *type_name = "";
    const char *severity_name = "";

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:
            type_name = "ERROR";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            type_name = "DEPRECATED BEHAVIOR";
            break;
        case GL_DEBUG_TYPE_MARKER:
            type_name = "MARKER";
            break;
        case GL_DEBUG_TYPE_OTHER:
            type_name = "OTHER";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            type_name = "PEROFRMANCE";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            type_name = "POP GROUP";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            type_name = "PORTABILITY";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            type_name = "PUSH GROUP";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            type_name = "UNDEFINED BEHAVIOR";
            break;
    }

    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
            severity_name = "HIGH";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            severity_name = "MEDIUM";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            severity_name = "LOW";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            severity_name = "NOTIFICATION";
            break;
    }

    printf("GL (%s:%s) %s\n", type_name, severity_name, message);
}

static void update_viewport(int width, int height)
{
    window_width = width;
    window_height = height;

    float x = width / 2.0f;
    float y = height / 2.0f;

    float sx, sy;

    const float aspect = aspect_width / static_cast<float>(aspect_height);
    if (width / aspect > height)
    {
        sx = height * aspect;
        sy = height;
    }
    else
    {
        sx = width;
        sy = width / aspect;
    }

    view_width = sx;
    view_height = sy;

    glViewport(x - sx / 2.0f, y - sy / 2.0f, sx, sy);
}

static void on_window_size_changed(GLFWwindow *window, int width, int height)
{
    update_viewport(width, height);
}

static void on_cursor_position_changed(GLFWwindow *window, double x, double y)
{
    cursorx = x;
    cursory = window_height - y;
}

void Graphics::initialize(GLFWwindow *window)
{
    aspect_width = 1920;
    aspect_height = 1080;

    glfwSetWindowSizeCallback(window, on_window_size_changed);
    glfwSetCursorPosCallback(window, on_cursor_position_changed);

    glfwGetWindowSize(window, &window_width, &window_height);
    glfwGetCursorPos(window, &cursorx, &cursory);

    update_viewport(window_width, window_height);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Disable VSync
    glfwSwapInterval(0);

    // Debug messages
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(gl_message_callback, NULL);

    ImGui::CreateContext();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450 core");
}

void Graphics::shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Graphics::set_aspect(int width, int height)
{
    aspect_width = width;
    aspect_height = height;

    update_viewport(window_width, window_height);
}

void Graphics::begin_frame()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Graphics::end_frame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

glm::vec2 Graphics::get_normalized_cursor_position()
{
    glm::vec2 result;
    result.x = cursorx - (window_width - view_width) / 2.0f;
    result.y = cursory - (window_height - view_height) / 2.0f;

    result.x /= static_cast<float>(view_width);
    result.y /= static_cast<float>(view_height);

    return result;
}
