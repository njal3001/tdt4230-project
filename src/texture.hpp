#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Texture
{
private:
    unsigned int id;
    glm::ivec2 size;

public:
    Texture(const glm::ivec2 &size);
    ~Texture();

    void set_data(const glm::vec4 *data) const;
    unsigned int get_id() const;
};
