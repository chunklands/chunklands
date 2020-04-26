#include "GLUniform.h"

namespace chunklands {
  const GLProgramBase& operator<<(const GLProgramBase& program, GLUniform& uniform) {
    uniform.location_ = program.GetUniformLocation(uniform.name_.c_str());
    return program;
  }
}
