#ifndef __CHUNKLANDS_ENGINE_GL_GL_CHECK_HXX__
#define __CHUNKLANDS_ENGINE_GL_GL_CHECK_HXX__

#include <chunklands/libcxx/glfw.hxx>
#include "gl_exception.hxx"

#ifndef NDEBUG
  #define GL_CHECK_DEBUG() ::chunklands::engine::gl::gl_check()
#else
  #define GL_CHECK_DEBUG() do {} while(0)
#endif

namespace chunklands::engine::gl {
  
  inline void gl_check() {
    const GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
      std::stringstream ss;
      ss << "Error: " << error;
      throw_gl_exception("glGetError", ss.str());
    }
  }

} // namespace chunklands::engine::gl

#endif