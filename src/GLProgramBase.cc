#include "GLProgramBase.h"

namespace chunklands {
  DEFINE_OBJECT_WRAP_DEFAULT_CTOR(GLProgramBase, ONE_ARG({
    InstanceMethod("_compile", &GLProgramBase::Compile)
  }))

  void GLProgramBase::Compile(const Napi::CallbackInfo& info) {
    std::string vsh_src = info[0].ToString();
    std::string fsh_src = info[1].ToString();

    { // vertex shader
      vsh_ = glCreateShader(GL_VERTEX_SHADER);
      const char* src = vsh_src.c_str();
      glShaderSource(vsh_, 1, &src, nullptr);
      glCompileShader(vsh_);
      GLint result = GL_FALSE;
      glGetShaderiv(vsh_, GL_COMPILE_STATUS, &result);
      if (result != GL_TRUE) {
        GLint length = 0;
        glGetShaderiv(vsh_, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> message(length + 1);
        glGetShaderInfoLog(vsh_, length, nullptr, message.data());
        Napi::Error::New(Env(), message.data()).ThrowAsJavaScriptException();
        return;
      }

      CHECK_GL();
    }

    { // fragment shader
      fsh_ = glCreateShader(GL_FRAGMENT_SHADER);
      const char* src = fsh_src.c_str();
      glShaderSource(fsh_, 1, &src, nullptr);
      glCompileShader(fsh_);
      GLint result = GL_FALSE;
      glGetShaderiv(fsh_, GL_COMPILE_STATUS, &result);
      if (result != GL_TRUE) {
        GLint length = 0;
        glGetShaderiv(fsh_, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> message(length + 1);
        glGetShaderInfoLog(fsh_, length, nullptr, message.data());
        Napi::Error::New(Env(), message.data()).ThrowAsJavaScriptException();
        return;
      }

      CHECK_GL();
    }

    { // program
      program_ = glCreateProgram();
      glAttachShader(program_, vsh_);
      glAttachShader(program_, fsh_);
      glLinkProgram(program_);

      GLint result = GL_FALSE;
      glGetProgramiv(program_, GL_LINK_STATUS, &result);
      if (result != GL_TRUE) {
        GLint length = 0;
        glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> message(length + 1);
        glGetProgramInfoLog(program_, length, nullptr, message.data());
        Napi::Error::New(Env(), message.data()).ThrowAsJavaScriptException();
        return;
      }

      CHECK_GL();
    }
  }

  GLint GLProgramBase::GetUniformLocation(const GLchar* name) const {
    return glGetUniformLocation(program_, name);
  }

  void GLProgramBase::Use() const {
    glUseProgram(program_);
  }
}
