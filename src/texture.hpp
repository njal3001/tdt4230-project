#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Texture
{
private:
    unsigned int id;
    glm::ivec2 size;
    unsigned int internal_format;

public:
    Texture(const glm::ivec2 &size, unsigned int internal_format);
    ~Texture();

    void set_data(const void *data) const;
    void set_sub_data(const void *data,
            int ox, int oy, int width, int height) const;

    void bind_to_unit(unsigned int unit) const;
    unsigned int get_id() const;
};
