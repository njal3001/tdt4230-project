#include "texture.hpp"
#include <GL/glew.h>

Texture::Texture()
    : id(0), size(glm::ivec2(0)), internal_format(0)
{}

void Texture::initialize(const glm::ivec2 &size, unsigned int internal_format)
{
    this->size = size;
    this->internal_format = internal_format;

    glCreateTextures(GL_TEXTURE_2D, 1, &this->id);
    glTextureParameteri(this->id,
            GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(this->id,
            GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(this->id,
            GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(this->id,
            GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureStorage2D(this->id, 1, this->internal_format,
            size.x, size.y);
}

Texture::~Texture()
{
    if (this->id)
    {
        glDeleteTextures(1, &this->id);
    }
}

void Texture::set_data(const void *data) const
{
    this->set_sub_data(data, 0, 0, this->size.x, this->size.y);
}

void Texture::set_sub_data(const void *data,
        int ox, int oy, int width, int height) const
{
    assert(this->id);

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

    glTextureSubImage2D(this->id, 0, ox, oy, width,
            height, format, type, data);
}

void Texture::copy(const Texture *source) const
{
    assert(this->id);

    glCopyImageSubData(source->get_id(), GL_TEXTURE_2D,
            0, 0, 0, 0, this->id, GL_TEXTURE_2D, 0,
            0, 0, 0, this->size.x, this->size.y, 1);
}

void Texture::bind_to_unit(unsigned int unit) const
{
    assert(this->id);

    glBindImageTexture(unit, this->id, 0, false, 0,
            GL_READ_WRITE, this->internal_format);
}

unsigned int Texture::get_id() const
{
    return this->id;
}
