#include "ray.hpp"

// FIXME?
Ray::Intersection Ray::cube_intersection(const glm::vec3 &cube_position,
        float cube_scale)
{
    Ray::Intersection result;

    glm::vec3 cube_min = glm::vec3(cube_position.x - cube_scale,
            cube_position.y - cube_scale, cube_position.z - cube_scale);
    glm::vec3 cube_max = glm::vec3(cube_position.x + cube_scale,
            cube_position.y + cube_scale, cube_position.z + cube_scale);

    glm::vec3 t_min = (cube_min - this->origin) / this->direction;
    glm::vec3 t_max = (cube_max - this->origin) / this->direction;

    glm::vec3 t1 = glm::min(t_min, t_max);
    glm::vec3 t2 = glm::max(t_min, t_max);

    float t_near = glm::max(glm::max(t1.x, t1.y), t1.z);
    float t_far = glm::min(glm::min(t2.x, t2.y), t2.z);

    result.hit = t_near <= t_far;
    result.near = this->origin + this->direction * t_near;
    result.far = this->origin + this->direction * t_far;

    return result;
}
