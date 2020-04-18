
#include "RenderQuad.h"

namespace chunklands {
  const GLfloat quad_vertices[] = {
    -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,   0.0f, 1.0f,
     1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
     1.0f,  1.0f, 0.0f,   1.0f, 1.0f,
  };

  RenderQuad::RenderQuad() {
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
  }

  RenderQuad::~RenderQuad() {
    if (vbo_ != 0) {
      glDeleteBuffers(1, &vbo_);
      vbo_ = 0;
    }

    if (vao_ != 0) {
      glDeleteVertexArrays(1, &vao_);
      vao_ = 0;
    }
  }

  void RenderQuad::Render() {
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
  }
}
