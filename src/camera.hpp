#pragma once
#include <glm/glm.hpp>
#include "ray.hpp"

class Camera
{
private:
    glm::vec3 position;
    glm::mat4 rotation;

    float fov;
    float aspect;
    float znear;
    float zfar;

public:
    Camera(float fov, float aspect, float znear, float zfar);

    void move(const glm::vec3 &delta);
    void move_forward(float amount);
    void move_backward(float amount);
    void move_right(float amount);
    void move_left(float amount);

    void rotate(float rad, const glm::vec3 &axis);
    void rotate_around(float rad, glm::vec3 axis, const glm::vec3 &point);
    void lookat(const glm::vec3 &point, const glm::vec3 &up);

    glm::vec3 get_position() const;
    glm::mat4 view() const;
    glm::mat4 projection() const;
    glm::mat4 matrix() const;

    Ray ray_from_cursor() const;
};
