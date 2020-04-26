#ifndef __CHUNKLANDS_SKYBOXPASSBASE_H__
#define __CHUNKLANDS_SKYBOXPASSBASE_H__

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "js.h"
#include "GLProgramBase.h"
#include "RenderPass.h"

namespace chunklands {
  class SkyboxPassBase : public JSObjectWrap<SkyboxPassBase>, public RenderPass {
    JS_IMPL_WRAP(SkyboxPassBase, ONE_ARG({
      JS_SETTER(Program)
    }))

  public:
    void UpdateProjection(const glm::mat4& matrix) {
      glUniformMatrix4fv(uniforms_.proj, 1, GL_FALSE, glm::value_ptr(matrix));
    }
    
    void UpdateView(const glm::mat4& matrix) {
      glUniformMatrix4fv(uniforms_.view, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void BindSkyboxTexture(GLuint texture) {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture);
    }

  protected:
    void InitializeProgram() override;
    
  private:

    struct {
      GLint proj = -1;
      GLint view = -1;
    } uniforms_;
  };
}

#endif
