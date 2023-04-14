cmake_minimum_required(VERSION 3.12)
project(slime-simulator)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option (GLFW_INSTALL OFF)
option (GLFW_BUILD_DOCS OFF)
option (GLFW_BUILD_EXAMPLES OFF)
option (GLFW_BUILD_TESTS OFF)
add_subdirectory(lib/glfw)

add_subdirectory(lib/glew-cmake)

add_executable(slime-simulator
    src/main.cpp
    src/graphics.cpp
    src/shader.cpp
    src/file.cpp
    src/calc.cpp
    src/timer.cpp
    src/camera.cpp
    src/ray.cpp
    src/slimesimulator.cpp
    src/texture.cpp
    src/mesh.cpp

    lib/imgui/imgui.cpp
    lib/imgui/imgui_draw.cpp
    lib/imgui/imgui_widgets.cpp
    lib/imgui/imgui_tables.cpp
    lib/imgui/imgui_demo.cpp
    lib/imgui/backends/imgui_impl_opengl3.cpp
    lib/imgui/backends/imgui_impl_glfw.cpp
)

include_directories(.
    lib/glew-cmake/include
    lib/imgui/
    lib/stb/
    lib/glfw/include
    lib/glm
)

target_link_libraries(slime-simulator glfw libglew_static)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

add_custom_target(run
    COMMAND ./slime-simulator
    DEPENDS slime-simulator
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR})
