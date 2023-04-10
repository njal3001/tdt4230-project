#pragma once
#include <glm/gtc/matrix_transform.hpp>

namespace Calc
{
    float frand();
    float frandrange(float min, float max);

    int randrange(int min, int max);

    glm::vec2 randdir();

    int mid(int low, int value, int high);
};
