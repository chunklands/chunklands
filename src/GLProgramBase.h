#ifndef __CHUNKLANDS_GLPROGRAMBASE_H__
#define __CHUNKLANDS_GLPROGRAMBASE_H__

#include "js.h"
#include "gl.h"

namespace chunklands {
  class GLProgramBase : public JSWrap<GLProgramBase> {
    JS_DECL_WRAP(GLProgramBase)
    JS_DECL_CB_VOID(compile)

  public:
    void Use() const;
    void Unuse() const;

    GLint GetUniformLocation(const GLchar* name) const;

  private:
    GLuint program_ = 0;
  };
}

#endif
