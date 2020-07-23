
#include "Vao3fv3fn2ft.hxx"
#include <stdexcept>
#include <assert.h>

namespace chunklands::gl {

  void Vao3fv3fn2ft::Cleanup() {
    if (vao_ != 0) {
      glDeleteVertexArrays(1, &vao_);
      vao_ = 0;
    }

    if (vbo_ != 0) {
      glDeleteBuffers(1, &vbo_);
      vbo_ = 0;
    }
  }

  void Vao3fv3fn2ft::Init(GLenum mode, const GLfloat* data, GLsizeiptr size) {
    Cleanup();

    mode_ = mode;

    if (size % 8 != 0) {
      throw std::runtime_error("data needs 8 * n items");
    }

    CHECK_GL();

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * size, data, GL_STATIC_DRAW);
    vertex_count_ = size / 8;

    glBindVertexArray(vao_);

    constexpr GLsizei stride = (3 + 3 + 2) * sizeof(GLfloat);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)(0 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // uv attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    CHECK_GL_HERE();
  }

  void Vao3fv3fn2ft::Render() const {
    CHECK_GL();

    assert(vao_ != 0);

    glBindVertexArray(vao_);
    glDrawArrays(mode_, 0, vertex_count_);
    glBindVertexArray(0);

    CHECK_GL_HERE();
  }

} // namespace chunklands::gl