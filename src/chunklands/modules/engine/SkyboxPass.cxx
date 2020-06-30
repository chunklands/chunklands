
#include "SkyboxPass.hxx"

namespace chunklands::modules::engine {

  JS_DEF_WRAP(SkyboxPass)
  
  void SkyboxPass::InitializeProgram() {
    CHECK_GL();

    gl::Uniform skybox_texture{"u_skybox_texture"};

    *js_Program
      >> skybox_texture
      >> uniforms_.proj
      >> uniforms_.view;

    skybox_texture.Update(0);
  }

} // namespace chunklands::modules::engine