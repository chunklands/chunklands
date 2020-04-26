#ifndef __CHUNKLANDS_GLPROGRAMBASE_H__
#define __CHUNKLANDS_GLPROGRAMBASE_H__

#include "js.h"
#include "gl.h"

namespace chunklands {
  class GLProgramBase : public JSObjectWrap<GLProgramBase> {
    JS_IMPL_WRAP(GLProgramBase, ONE_ARG({
      JS_CB(compile)
    }))

    JS_DECL_CB_VOID(compile)

  public:
    void Use() const {
      glUseProgram(program_);
    }

    void Unuse() const {
      glUseProgram(0);
    }

    GLint GetUniformLocation(const GLchar* name) const;

  private:
    GLuint program_ = 0;
  };
}

#endif
