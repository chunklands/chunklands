#ifndef __CHUNKLANDS_GL_PROGRAMBASE_HXX__
#define __CHUNKLANDS_GL_PROGRAMBASE_HXX__

#include <chunklands/js.hxx>
#include "glfw.hxx"

namespace chunklands::gl {

  class ProgramBase : public JSObjectWrap<ProgramBase> {
    JS_IMPL_WRAP(ProgramBase, ONE_ARG({
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

} // namespace chunklands::gl

#endif