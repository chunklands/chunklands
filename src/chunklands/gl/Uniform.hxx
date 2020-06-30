#ifndef __CHUNKLANDS_GL_UNIFORM_HXX__
#define __CHUNKLANDS_GL_UNIFORM_HXX__

#include <string>
#include "glfw.hxx"
#include <chunklands/modules/misc/misc_module.hxx>
#include <glm/gtc/type_ptr.hpp>
#include "ProgramBase.hxx"

namespace chunklands::gl {

  class Uniform {
    friend const ProgramBase& operator>>(const ProgramBase& program, Uniform& uniform);

  public:
    Uniform(std::string name) : name_(std::move(name)) {}
    Uniform(const std::string& name, signed i) : name_(name + "[" + std::to_string(i) + "]") {}

    operator GLint() const {
      return location_;
    }

    void Update(GLint value) {
      CC_ASSERT(location_ != -1);
      glUniform1i(location_, value);
    }

    void Update(const glm::vec3& value) {
      CC_ASSERT(location_ != -1);
      glUniform3fv(location_, 1, glm::value_ptr(value));
    }

    void Update(const glm::mat4& value) {
      CC_ASSERT(location_ != -1);
      glUniformMatrix4fv(location_, 1, GL_FALSE, glm::value_ptr(value));
    }

    void FetchLocation(const ProgramBase& program) {
      location_ = program.GetUniformLocation(name_.c_str());
    }

  private:
    std::string name_;
    GLint location_ = -1;
  };

  const ProgramBase& operator>>(const ProgramBase& program, Uniform& uniform);

} // namespace chunklands::gl

#endif