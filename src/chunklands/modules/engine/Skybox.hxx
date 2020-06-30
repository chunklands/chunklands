#ifndef __CHUNKLANDS_MODULES_ENGINE_SKYBOX_HXX__
#define __CHUNKLANDS_MODULES_ENGINE_SKYBOX_HXX__

#include <chunklands/js.hxx>
#include <chunklands/modules/gl/glfw.hxx>
#include "SkyboxTexture.hxx"

namespace chunklands::modules::engine {

  class Skybox : public JSObjectWrap<Skybox> {
    JS_IMPL_WRAP(Skybox, ONE_ARG({
      JS_CB(initialize)
    }))

    JS_DECL_CB_VOID(initialize)

  public:
    ~Skybox() {
      DeleteGLArrays();
    }

    void DeleteGLArrays() {
      glDeleteBuffers(1, &vbo_);
      glDeleteVertexArrays(1, &vao_);
    }

  public:
    void Render();

  private:
    GLuint vao_ = 0;
    GLuint vbo_ = 0;

    SkyboxTexture texture_;
  };

} // namespace chunklands::modules::engine

#endif