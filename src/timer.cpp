#include "timer.hpp"
#include <GLFW/glfw3.h>

Timer::Timer()
    : previous_time(glfwGetTime())
{}

float Timer::delta()
{
    float now = glfwGetTime();
    float prev = this->previous_time;

    this->previous_time = now;

    return now - prev;
}
