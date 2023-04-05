#include "texture.hpp"
#include <GL/glew.h>

Texture::Texture(const glm::ivec2 &size, unsigned int internal_format)
    : size(size), internal_format(internal_format)
{
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
  glDeleteTextures(1, &this->id);
}

void Texture::set_data(const void *data) const
{
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

    glTextureSubImage2D(this->id, 0, 0, 0, this->size.x,
            this->size.y, format, type, data);
}

unsigned int Texture::get_id() const
{
    return this->id;
}
