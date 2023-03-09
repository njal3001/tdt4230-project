cmake_minimum_required(VERSION 3.12)
project(particle-system)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)

add_executable(particle-system
    src/main.cpp
)

include_directories(. ${GLEW_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS})
target_link_libraries(particle-system glfw ${OPENGL_LIBRARIES}
    GLEW::GLEW glm::glm)

add_custom_target(run
    COMMAND ./particle-system
    DEPENDS particle-system
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR})
