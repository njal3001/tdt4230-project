#include "camera.hpp"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include "graphics.hpp"

Camera::Camera(float fov, float aspect, float znear, float zfar)
    : position(0.0f), rotation(1.0f), fov(fov), aspect(aspect),
    znear(znear), zfar(zfar)
{}

void Camera::move(glm::vec3 delta)
{
    this->position += delta;
}

void Camera::move_forward(float amount)
{
    glm::vec3 forward = glm::vec4(0.0f, 0.0f, -1.0f, 1.0f) * this->rotation;
    this->move(forward * amount);
}

void Camera::move_backward(float amount)
{
    this->move_forward(-amount);
}

void Camera::move_right(float amount)
{
    glm::vec3 right =  glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) * this->rotation;
    this->move(right * amount);
}

void Camera::move_left(float amount)
{
    this->move_right(-amount);
}

void Camera::rotate(float rad, glm::vec3 axis)
{
    this->rotation = glm::rotate(glm::mat4(1.0f), rad, axis) * this->rotation;
}

glm::mat4 Camera::view() const
{
    return this->rotation * glm::translate(glm::mat4(1.0f), -this->position);
}

glm::mat4 Camera::projection() const
{
    return glm::perspective(this->fov, this->aspect, this->znear, this->zfar);
}

glm::mat4 Camera::matrix() const
{
    return this->projection() * this->view();
}

Ray Camera::ray_from_cursor() const
{
    Ray result;

    glm::mat4 P = this->projection();
    glm::mat4 V = this->view();

    glm::vec2 cursor = Graphics::get_normalized_cursor_position();

    glm::vec4 ray_clip = glm::vec4(2.0f * cursor.x - 1.0f,
            2.0f * cursor.y - 1.0f, -1.0f, 1.0f);
    glm::vec4 ray_eye = glm::inverse(P) * ray_clip;
    ray_eye.z = -1.0f;
    ray_eye.w = 0.0f;

    glm::vec3 ray_world = glm::inverse(V) * ray_eye;

    result.origin = this->position;
    result.direction = glm::normalize(ray_world);

    return result;
}
