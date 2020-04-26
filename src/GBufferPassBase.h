#ifndef __CHUNKLANDS_GBUFFERPASSBASE_H__
#define __CHUNKLANDS_GBUFFERPASSBASE_H__

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "js.h"
#include "gl.h"
#include "GLProgramBase.h"

namespace chunklands {
  class GBufferPassBase : public JSObjectWrap<GBufferPassBase> {
    JS_IMPL_WRAP(GBufferPassBase, ONE_ARG({
      JS_SETTER(Program)
    }))

    JS_DECL_SETTER_WRAP(GLProgramBase, Program)

  public:
    void Begin() {
      js_Program->Use();
    }

    void End() {
      js_Program->Unuse();
    }

    void UpdateProjection(const glm::mat4& proj) {
      glUniformMatrix4fv(uniforms_.proj, 1, GL_FALSE, glm::value_ptr(proj));
    }

    void UpdateView(const glm::mat4& view) {
      glUniformMatrix4fv(uniforms_.view, 1, GL_FALSE, glm::value_ptr(view));
    }

  private:
    struct {
      GLint proj    = -1,
            view    = -1;
    } uniforms_;
  };
}

#endif
