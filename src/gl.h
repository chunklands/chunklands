#ifndef __CHUNKLANDS_GL_H__
#define __CHUNKLANDS_GL_H__

#include <cassert>
#include <glad/glad.h>
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
  ::chunklands::GLCheck gl_check(POSITION);

#define CHECK_GL_HERE() \
  gl_check.Check(POSITION)
#endif // NDEBUG

namespace chunklands {
  class GLCheck {
  public:
    GLCheck(const char* hint);
    ~GLCheck();

  public:
    void Check(const char* hint);
  };
}

#endif