#include "calc.hpp"
#include <cmath>

float Calc::frand()
{
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

float Calc::frandrange(float min, float max)
{
    return min + (max - min) *
        static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

glm::vec2 Calc::randdir()
{
    glm::vec2 dir;
    dir.x = frandrange(-1.0f, 1.0f);
    dir.y = frandrange(-1.0f, 1.0f);

    return glm::normalize(dir);
}
