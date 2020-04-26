#ifndef __CHUNKLANDS_GBUFFERPASSBASE_H__
#define __CHUNKLANDS_GBUFFERPASSBASE_H__

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "js.h"
#include "gl.h"
#include "GLProgramBase.h"
#include "RenderPass.h"

namespace chunklands {
  class GBufferPassBase : public JSObjectWrap<GBufferPassBase>, public RenderPass {
    JS_IMPL_WRAP(GBufferPassBase, ONE_ARG({
      JS_SETTER(Program)
    }))

  public:
    void UpdateProjection(const glm::mat4& proj) {
      glUniformMatrix4fv(uniforms_.proj, 1, GL_FALSE, glm::value_ptr(proj));
    }

    void UpdateView(const glm::mat4& view) {
      glUniformMatrix4fv(uniforms_.view, 1, GL_FALSE, glm::value_ptr(view));
    }

  protected:
    void InitializeProgram() override;

  private:
    struct {
      GLint proj    = -1,
            view    = -1;
    } uniforms_;
  };
}

#endif
