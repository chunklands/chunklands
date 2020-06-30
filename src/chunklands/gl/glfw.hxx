#ifndef __CHUNKLANDS_GL_GLFW_HXX__
#define __CHUNKLANDS_GL_GLFW_HXX__

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <boost/preprocessor/stringize.hpp>

#define POSITION __FILE__ ":" BOOST_PP_STRINGIZE(__LINE__)

#ifdef NDEBUG

#define CHECK_GL() \
  do {} while (0)

#define CHECK_GL_HERE() \
  do {} while (0)

#else // NDEBUG

#define CHECK_GL() \
  ::chunklands::gl::GLCheck gl_check(POSITION);

#define CHECK_GL_HERE() \
  gl_check.Check(POSITION)
#endif // NDEBUG

namespace chunklands::gl {

  class GLCheck {
  public:
    GLCheck(const char* hint);
    ~GLCheck();

  public:
    void Check(const char* hint);
  };

} // namespace chunklands::gl

#endif