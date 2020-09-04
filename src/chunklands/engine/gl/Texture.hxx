#ifndef __CHUNKLANDS_ENGINE_GL_TEXTURE_HXX__
#define __CHUNKLANDS_ENGINE_GL_TEXTURE_HXX__

#include <chunklands/engine/image/image_loader.hxx>
#include <chunklands/libcxx/glfw.hxx>
#include <glm/vec2.hpp>

namespace chunklands::engine::gl {

class Texture {
public:
    Texture(const image::image& image);
    ~Texture();

    GLuint GetTexture() const
    {
        return texture_;
    }

private:
    GLuint texture_ = 0;
};

} // namespace chunklands::engine::gl

#endif