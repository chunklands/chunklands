#include "LightingPassBase.h"

namespace chunklands {
  JS_DEF_WRAP(LightingPassBase)

  void LightingPassBase::InitializeProgram() {
    GLUniform position_texture{"u_position_texture"},
              normal_texture{"u_normal_texture"},
              color_texture{"u_color_texture"},
              ssao_texture{"u_ssao_texture"};

    *js_Program
      >> uniforms_.render_distance
      >> uniforms_.sun_position
      >> position_texture
      >> normal_texture
      >> color_texture
      >> ssao_texture;

    position_texture.Update(0);
    normal_texture.Update(1);
    color_texture.Update(2);
    ssao_texture.Update(3);
  }
}
