#ifndef __CHUNKLANDS_ENGINE_GL_GL_CHECK_HXX__
#define __CHUNKLANDS_ENGINE_GL_GL_CHECK_HXX__

#include <chunklands/engine/engine_exception.hxx>
#include <chunklands/libcxx/ThreadGuard.hxx>
#include <chunklands/libcxx/glfw.hxx>

#ifndef NDEBUG
#define GL_CHECK_DEBUG() ::chunklands::engine::gl::gl_check()
#else
#define GL_CHECK_DEBUG() \
    do {                 \
    } while (0)
#endif

namespace chunklands::engine::gl {

inline void gl_check()
{
    assert(libcxx::ThreadGuard::IsOpenGLThread());

    const GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::stringstream ss;
        ss << "Error: " << error;
        throw create_engine_exception("glGetError", ss.str());
    }
}

} // namespace chunklands::engine::gl

#endif