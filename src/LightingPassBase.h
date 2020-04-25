#ifndef __CHUNKLANDS_LIGHTINGPASSBASE_H__
#define __CHUNKLANDS_LIGHTINGPASSBASE_H__

#include "gl.h"
#include "js.h"
#include <glm/vec3.hpp>
#include "ARenderPass.h"

namespace chunklands {
  class LightingPassBase : public JSWrap<LightingPassBase>, public ARenderPass {
    JS_DECL_WRAP(LightingPassBase)

  public:
    void UpdateRenderDistance(GLfloat value);
    void UpdateSunPosition(const glm::vec3& value);

    void BindPositionTexture(GLuint texture);
    void BindNormalTexture(GLuint texture);
    void BindColorTexture(GLuint texture);
    void BindSSAOTexture(GLuint texture);

  protected:
    void InitializeProgram() override;

  private:
    struct {
      GLint render_distance = -1,
            sun_position    = -1;
    } uniforms_;
  };
}

#endif
