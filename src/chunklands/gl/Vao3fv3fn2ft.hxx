#ifndef __CHUNKLANDS_GL_VAO3FV3FN2FT_HXX__
#define __CHUNKLANDS_GL_VAO3FV3FN2FT_HXX__

#include <vector>
#include "glfw.hxx"

namespace chunklands::gl {

class Vao3fv3fn2ft {
public:
  ~Vao3fv3fn2ft() { Cleanup(); }
  void Init(GLenum mode, const GLfloat* data, GLsizeiptr size);
  void Render() const;

private:
  void Cleanup();

private:
  GLenum mode_;
  GLuint vao_ = 0;
  GLuint vbo_ = 0;
  GLsizei vertex_count_ = 0;
};

} // namespace chunklands::gl

#endif