#ifndef __CHUNKLANDS_GLPROGRAMBASE_H__
#define __CHUNKLANDS_GLPROGRAMBASE_H__

#include <napi.h>
#include "napi/object_wrap_util.h"
#include "gl.h"

namespace chunklands {
  class GLProgramBase : public Napi::ObjectWrap<GLProgramBase> {
    DECLARE_OBJECT_WRAP(GLProgramBase)

    DECLARE_OBJECT_WRAP_CB(void Compile)

  public:
    void Use() const;
    GLint GetUniformLocation(const GLchar* name) const;

  private:
    GLuint vsh_ = 0;
    GLuint fsh_ = 0;
    GLuint program_ = 0;
  };
}

#endif
