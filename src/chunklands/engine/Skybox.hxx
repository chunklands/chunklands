#ifndef __CHUNKLANDS_ENGINE_SKYBOX_HXX__
#define __CHUNKLANDS_ENGINE_SKYBOX_HXX__

#include <chunklands/js.hxx>
#include <chunklands/gl/glfw.hxx>
#include "SkyboxTexture.hxx"

namespace chunklands::engine {

  class Skybox : public JSObjectWrap<Skybox> {
    JS_IMPL_WRAP(Skybox, ONE_ARG({
      JS_CB(initialize)
    }))

    JS_DECL_CB_VOID(initialize)

  public:
    ~Skybox();
    void DeleteGLArrays();

  public:
    void Render();

  private:
    GLuint vao_ = 0;
    GLuint vbo_ = 0;

    SkyboxTexture texture_;
  };

} // namespace chunklands::engine

#endif