#include "texture.hpp"
#include <GL/glew.h>

Texture::Texture(const glm::ivec2 &size)
    : size(size)
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
    glTextureStorage2D(this->id, 1, GL_RGBA32F,
            size.x, size.y);
}

Texture::~Texture()
{
  glDeleteTextures(1, &this->id);
}

void Texture::set_data(const glm::vec4 *data) const
{
    glTextureSubImage2D(this->id, 0, 0, 0, this->size.x,
            this->size.y, GL_RGBA, GL_FLOAT, data);
}

unsigned int Texture::get_id() const
{
    return this->id;
}
