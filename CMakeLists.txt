cmake_minimum_required(VERSION 3.12)
project(particle-system)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)

set(IMGUI_DIR src/external/imgui)

add_executable(particle-system
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

    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
)

include_directories(. ${GLEW_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS}
    private ${IMGUI_DIR})
target_link_libraries(particle-system glfw ${OPENGL_LIBRARIES}
    GLEW::GLEW glm::glm)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

add_custom_target(run
    COMMAND ./particle-system
    DEPENDS particle-system
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR})
