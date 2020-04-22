#ifndef __CHUNKLANDS_SKYBOXPASSBASE_H__
#define __CHUNKLANDS_SKYBOXPASSBASE_H__

#include <glm/mat4x4.hpp>
#include "js.h"
#include "GLProgramBase.h"

namespace chunklands {
  class SkyboxPassBase : public JSWrap<SkyboxPassBase> {
    JS_DECL_WRAP(SkyboxPassBase)
    JS_CB_DECL(useProgram)

  public:
    void Begin() {
      program_->Use();
    }

    void End() {
      program_->Unuse();
    }

    void UpdateProjection(const glm::mat4& matrix);
    void UpdateView(const glm::mat4& matrix);
    void BindSkyboxTexture(GLuint texture);
    
  private:
    JSRef<GLProgramBase> program_;

    struct {
      GLint proj = -1;
      GLint view = -1;
    } uniforms_;
  };
}

#endif
