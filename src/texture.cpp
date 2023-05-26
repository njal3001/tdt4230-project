#include "texture.hpp"
#include <glad/glad.h>

Texture3D::Texture3D()
    : id(0), size(0), internal_format(0)
{}

void Texture3D::initialize(const glm::ivec3 &size, unsigned int internal_format)
{
    this->size = size;
    this->internal_format = internal_format;

    glCreateTextures(GL_TEXTURE_3D, 1, &id);
    glTextureParameteri(id,
            GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(id,
            GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(id,
            GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(id,
            GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureStorage3D(id, 1, internal_format,
            size.x, size.y, size.z);
}

Texture3D::~Texture3D()
{
    if (id)
    {
        glDeleteTextures(1, &this->id);
    }
}

void Texture3D::set_data(const void *data) const
{
    this->set_sub_data(data, 0, 0, 0, this->size.x, this->size.y, this->size.z);
}

void Texture3D::set_sub_data(const void *data,
        int ox, int oy, int oz, int width, int height, int depth) const
{
    assert(id);

    unsigned int format;
    unsigned int type;

    switch (this->internal_format)
    {
        case GL_RGBA32F:
            format = GL_RGBA;
            type = GL_FLOAT;
            break;
        case GL_R32UI:
            format = GL_RED_INTEGER;
            type = GL_UNSIGNED_INT;
            break;
    }

    glTextureSubImage3D(this->id, 0, ox, oy, oz, width,
            height, depth, format, type, data);
}

void Texture3D::copy(const Texture3D *source) const
{
    assert(id);

    glCopyImageSubData(source->get_id(), GL_TEXTURE_3D,
            0, 0, 0, 0, this->id, GL_TEXTURE_3D, 0,
            0, 0, 0, this->size.x, this->size.y, this->size.z);
}

void Texture3D::bind_to_unit(unsigned int unit) const
{
    assert(id);

    glBindImageTexture(unit, this->id, 0, false, 0,
            GL_READ_WRITE, this->internal_format);
}

unsigned int Texture3D::get_id() const
{
    return this->id;
}
