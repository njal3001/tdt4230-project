#include "calc.hpp"
#include <cmath>

float Calc::randrange(float min, float max)
{
    return min + (max - min) *
        static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}
