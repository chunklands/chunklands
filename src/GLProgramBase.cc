#include "GLProgramBase.h"

#include "check.h"

namespace chunklands {
  JS_DEF_WRAP(GLProgramBase, ONE_ARG({
    JS_CB(compile)
  }))

  void GLProgramBase::JSCall_compile(const Napi::CallbackInfo& info) {
    std::string vsh_src = info[0].ToString(),
                fsh_src = info[1].ToString();

    GLuint vsh, fsh;

    { // vertex shader
      CHECK_GL();

      vsh = glCreateShader(GL_VERTEX_SHADER);
      const char* src = vsh_src.c_str();
      glShaderSource(vsh, 1, &src, nullptr);
      glCompileShader(vsh);
      GLint result = GL_FALSE;
      glGetShaderiv(vsh, GL_COMPILE_STATUS, &result);
      if (result != GL_TRUE) {
        GLint length = 0;
        glGetShaderiv(vsh, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> message(length + 1);
        glGetShaderInfoLog(vsh, length, nullptr, message.data());
        throw Napi::Error::New(Env(), message.data());
      }
    }

    { // fragment shader
      CHECK_GL();

      fsh = glCreateShader(GL_FRAGMENT_SHADER);
      const char* src = fsh_src.c_str();
      glShaderSource(fsh, 1, &src, nullptr);
      glCompileShader(fsh);
      GLint result = GL_FALSE;
      glGetShaderiv(fsh, GL_COMPILE_STATUS, &result);
      if (result != GL_TRUE) {
        GLint length = 0;
        glGetShaderiv(fsh, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> message(length + 1);
        glGetShaderInfoLog(fsh, length, nullptr, message.data());
        throw Napi::Error::New(Env(), message.data());
      }
    }

    { // program
      CHECK_GL();

      program_ = glCreateProgram();
      glAttachShader(program_, vsh);
      glAttachShader(program_, fsh);
      glLinkProgram(program_);

      GLint result = GL_FALSE;
      glGetProgramiv(program_, GL_LINK_STATUS, &result);
      if (result != GL_TRUE) {
        GLint length = 0;
        glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> message(length + 1);
        glGetProgramInfoLog(program_, length, nullptr, message.data());
        throw Napi::Error::New(Env(), message.data());
      }

      glDeleteShader(vsh);
      vsh = 0;
      glDeleteShader(fsh);
      fsh = 0;
    }
  }

  GLint GLProgramBase::GetUniformLocation(const GLchar* name) const {
    GLint location = glGetUniformLocation(program_, name);
    CHECK_MSG(location != -1, std::string("for ") + name);

    return location;
  }

  void GLProgramBase::Use() const {
    glUseProgram(program_);
  }

  void GLProgramBase::Unuse() const {
    glUseProgram(0);
  }
}
