#ifndef __CHUNKLANDS_SCENEBASE_H__
#define __CHUNKLANDS_SCENEBASE_H__

#include <napi.h>
#include "napi/UnwrappedObject.h"
#include "gl.h"
#include "WindowBase.h"
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace chunklands {
  class SceneBase : public Napi::ObjectWrap<SceneBase> {
  public:
    static Napi::FunctionReference constructor;
    static void Initialize(Napi::Env env);

  public:
    SceneBase(const Napi::CallbackInfo& info);

  public: // JS
    void SetWindow(const Napi::CallbackInfo& info);
  
  public: // Native
    void Prepare();
    void Render(double diff);

  private:
    NapiExt::PersistentObjectWrap<WindowBase> window_;

    std::string vsh_src_;
    std::string fsh_src_;
    
    GLuint vao_;
    GLuint vb_;

    GLuint vsh_;
    GLuint fsh_;
    GLuint program_;

    glm::vec3 pos_ = glm::vec3(0.f, 2.f, 4.f);

    glm::mat4 view_;
    GLint view_uniform_location_;

    glm::mat4 proj_;
    GLint proj_uniform_location_;
  };
}

#endif