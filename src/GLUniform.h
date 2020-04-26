#ifndef __CHUNKLANDS_GLUNIFORM_H__
#define __CHUNKLANDS_GLUNIFORM_H__

#include <string>
#include "gl.h"
#include "GLProgramBase.h"
#include "check.h"

namespace chunklands {
  class GLUniform {
    friend const GLProgramBase& operator<<(const GLProgramBase& program, GLUniform& uniform);

  public:
    GLUniform(std::string name) : name_(std::move(name)) {}

    operator GLint() const {
      return location_;
    }

    void operator()(GLint value) {
      CC_ASSERT(location_ != -1);
      glUniform1i(location_, value);
    }

    void FetchLocation(const GLProgramBase& program) {
      location_ = program.GetUniformLocation(name_.c_str());
    }

  private:
    std::string name_;
    GLint location_ = -1;
  };

  const GLProgramBase& operator<<(const GLProgramBase& program, GLUniform& uniform);
}

#endif
