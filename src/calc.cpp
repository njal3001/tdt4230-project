#include "calc.hpp"
#include <cmath>
#include <algorithm>

float Calc::frand()
{
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

float Calc::frandrange(float min, float max)
{
    return min + (max - min) *
        static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

int Calc::randrange(int min, int max)
{
    return min + rand() % (max - min);
}

glm::vec2 Calc::randdir()
{
    glm::vec2 dir;
    dir.x = frandrange(-1.0f, 1.0f);
    dir.y = frandrange(-1.0f, 1.0f);

    return glm::normalize(dir);
}

int Calc::mid(int low, int value, int high)
{
    return std::max(low, std::min(value, high));
}
