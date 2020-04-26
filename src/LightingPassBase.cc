#include "LightingPassBase.h"

namespace chunklands {
  JS_DEF_WRAP(LightingPassBase)

  void LightingPassBase::InitializeProgram() {
    uniforms_ = {
      .render_distance = js_Program->GetUniformLocation("u_render_distance"),
      .sun_position     = js_Program->GetUniformLocation("u_sun_position"),
    };

    glUniform1i(js_Program->GetUniformLocation("u_position_texture"), 0);
    glUniform1i(js_Program->GetUniformLocation("u_normal_texture"),   1);
    glUniform1i(js_Program->GetUniformLocation("u_color_texture"),    2);
    glUniform1i(js_Program->GetUniformLocation("u_ssao_texture"),     3);
  }
}
