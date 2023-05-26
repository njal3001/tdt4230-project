#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Texture3D
{
private:
    unsigned int id;
    glm::ivec3 size;
    unsigned int internal_format;

public:
    Texture3D();
    ~Texture3D();

    void initialize(const glm::ivec3 &size, unsigned int internal_format);

    void set_data(const void *data) const;
    void set_sub_data(const void *data,
            int ox, int oy, int oz, int width, int height, int depth) const;
    void copy(const Texture3D *source) const;

    void bind_to_unit(unsigned int unit) const;
    unsigned int get_id() const;
};
