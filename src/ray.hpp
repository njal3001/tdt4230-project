#pragma once
#include <glm/glm.hpp>

struct Ray
{
    struct Intersection
    {
        bool hit;
        glm::vec3 near;
        glm::vec3 far;
    };

    glm::vec3 origin;
    glm::vec3 direction;

    Intersection cube_intersection(const glm::vec3 &cube_position,
            float cube_scale);
};
