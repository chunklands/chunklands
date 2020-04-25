#ifndef __CHUNKLANDS_GBUFFERPASSBASE_H__
#define __CHUNKLANDS_GBUFFERPASSBASE_H__

#include <glm/mat4x4.hpp>

#include "js.h"
#include "gl.h"
#include "GLProgramBase.h"

namespace chunklands {
  class GBufferPassBase : public JSWrap<GBufferPassBase> {
    JS_DECL_WRAP(GBufferPassBase)
    JS_DECL_SETTER_REF(GLProgramBase, Program)

  public:
    void Begin() {
      js_Program->Use();
    }

    void End() {
      js_Program->Unuse();
    }

    void UpdateProjection(const glm::mat4& proj);
    void UpdateView(const glm::mat4& view);

  private:
    struct {
      GLint proj    = -1,
            view    = -1;
    } uniforms_;
  };
}

#endif
