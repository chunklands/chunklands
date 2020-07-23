
#include "Uniform.hxx"

namespace chunklands::gl {

  const ProgramBase& operator>>(const ProgramBase& program, Uniform& uniform) {
    uniform.location_ = program.GetUniformLocation(uniform.name_.c_str());
    return program;
  }

} // namespace chunklands::gl