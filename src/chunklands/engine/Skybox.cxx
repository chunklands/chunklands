
#include "Skybox.hxx"

namespace chunklands::engine {

  GLfloat skybox_vertices[] = {
    -1.f,  1.f, -1.f,
    -1.f, -1.f, -1.f,
     1.f, -1.f, -1.f,
     1.f, -1.f, -1.f,
     1.f,  1.f, -1.f,
    -1.f,  1.f, -1.f,

    -1.f, -1.f,  1.f,
    -1.f, -1.f, -1.f,
    -1.f,  1.f, -1.f,
    -1.f,  1.f, -1.f,
    -1.f,  1.f,  1.f,
    -1.f, -1.f,  1.f,

     1.f, -1.f, -1.f,
     1.f, -1.f,  1.f,
     1.f,  1.f,  1.f,
     1.f,  1.f,  1.f,
     1.f,  1.f, -1.f,
     1.f, -1.f, -1.f,

    -1.f, -1.f,  1.f,
    -1.f,  1.f,  1.f,
     1.f,  1.f,  1.f,
     1.f,  1.f,  1.f,
     1.f, -1.f,  1.f,
    -1.f, -1.f,  1.f,

    -1.f,  1.f, -1.f,
     1.f,  1.f, -1.f,
     1.f,  1.f,  1.f,
     1.f,  1.f,  1.f,
    -1.f,  1.f,  1.f,
    -1.f,  1.f, -1.f,

    -1.f, -1.f, -1.f,
    -1.f, -1.f,  1.f,
     1.f, -1.f, -1.f,
     1.f, -1.f, -1.f,
    -1.f, -1.f,  1.f,
     1.f, -1.f,  1.f
  };

  JS_DEF_WRAP(Skybox)

  Skybox::~Skybox() {
    DeleteGLArrays();
  }

  void Skybox::DeleteGLArrays() {
    glDeleteBuffers(1, &vbo_);
    glDeleteVertexArrays(1, &vao_);
  }

  void Skybox::JSCall_initialize(JSCbi info) {
    CHECK_GL();
    DeleteGLArrays();

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *)0);

    assert(info[0].IsString());
    texture_.LoadTexture(info[0].ToString().Utf8Value());
  }

  void Skybox::Render() {
    texture_.ActiveAndBind(GL_TEXTURE0);

    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
  }

} // namespace chunklands::engine