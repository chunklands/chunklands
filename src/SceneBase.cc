#include "SceneBase.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace chunklands {
  Napi::FunctionReference SceneBase::constructor;

  void SceneBase::Initialize(Napi::Env env) {
    constructor = Napi::Persistent(
      DefineClass(env, "SceneBase", {
        InstanceMethod("setWindow", &SceneBase::SetWindow)
      })
    );

    constructor.SuppressDestruct();
  }

  SceneBase::SceneBase(const Napi::CallbackInfo& info) : Napi::ObjectWrap<SceneBase>(info) {
    Napi::HandleScope scope(info.Env());
    
    if (!info[0].IsObject()) {
      Napi::Error::New(Env(), "expect object").ThrowAsJavaScriptException();
      return;
    }

    Napi::Object settings = info[0].ToObject();
    vsh_src_ = settings.Get("vertexShader").ToString();
    fsh_src_ = settings.Get("fragmentShader").ToString();
  }

  void SceneBase::SetWindow(const Napi::CallbackInfo& info) {
    window_ = info[0].ToObject();
  }

  void SceneBase::Prepare() {

    { // vertex shader (duplicated code)
      vsh_ = glCreateShader(GL_VERTEX_SHADER);
      const char* src = vsh_src_.c_str();
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

    { // fragment shader (duplicated code)
      fsh_ = glCreateShader(GL_FRAGMENT_SHADER);
      const char* src = fsh_src_.c_str();
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

    proj_ = glm::perspective(glm::radians(45.f), 640.f / 480.f, 0.1f, 100.0f); 

    view_uniform_location_ = glGetUniformLocation(program_, "u_view");
    proj_uniform_location_ = glGetUniformLocation(program_, "u_proj");

    glUseProgram(program_);
    glUniformMatrix4fv(proj_uniform_location_, 1, GL_FALSE, glm::value_ptr(proj_));
    glUseProgram(0);

    CHECK_GL();

    chunk_.Prepare();
  }

  void SceneBase::Render(double diff) {

    glfwPollEvents();
    window_->Clear();

    if (window_->GetKey(GLFW_KEY_W)) {
      pos_.z -= diff;
    }

    if (window_->GetKey(GLFW_KEY_S)) {
      pos_.z += diff;
    }

    if (window_->GetKey(GLFW_KEY_A)) {
      pos_.x -= diff;
    }

    if (window_->GetKey(GLFW_KEY_D)) {
      pos_.x += diff;
    }
    
    glUseProgram(program_);

    view_ = glm::lookAt(pos_, pos_ + glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, 1.f, 0.f));
    glUniformMatrix4fv(view_uniform_location_, 1, GL_FALSE, glm::value_ptr(view_));
    CHECK_GL();

    chunk_.Render();

    glUseProgram(program_);

    window_->SwapBuffers();
  }
}