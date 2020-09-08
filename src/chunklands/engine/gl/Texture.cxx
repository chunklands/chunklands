
#include "Texture.hxx"

namespace chunklands::engine::gl {

GLenum channels_to_format(int channels)
{
    switch (channels) {
    case 3:
        return GL_RGB;
    case 4:
        return GL_RGBA;
    }

    throw std::runtime_error("unknown amount of channels");
}

Texture::~Texture()
{
    if (texture_ != 0) {
        glDeleteTextures(1, &texture_);
        texture_ = 0;
    }
}

Texture::Texture(const image::image& image)
{
    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, channels_to_format(image.channels), GL_UNSIGNED_BYTE, image.data.get());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace chunklands::engine::gl