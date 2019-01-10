#ifndef __CHUNKLANDS_SCENEBASE_H__
#define __CHUNKLANDS_SCENEBASE_H__

#include <napi.h>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include "napi/UnwrappedObject.h"
#include "gl.h"
#include "WindowBase.h"
#include "Chunk.h"

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
    
    Chunk chunk_;

    GLuint vsh_;
    GLuint fsh_;
    GLuint program_;

    glm::vec3 pos_ = glm::vec3(8.f, 8.f, 20.f);

    glm::mat4 view_;
    GLint view_uniform_location_;

    glm::mat4 proj_;
    GLint proj_uniform_location_;
  };
}

#endif