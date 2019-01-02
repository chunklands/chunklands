#ifndef __CHUNKLANDS_GL_H__
#define __CHUNKLANDS_GL_H__

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cassert>

#ifdef NDEBUG

#define CHECK_GL() \
  do {} while (0)

#else // NDEBUG

#define CHECK_GL()              \
  do {                          \
    GLenum err = glGetError();  \
    assert(err == GL_NONE);     \
  } while (0)
#endif // NDEBUG

#endif