#ifndef __CHUNKLANDS_SCENEBASE_H__
#define __CHUNKLANDS_SCENEBASE_H__

#include <napi.h>
#include "gl.h"

namespace chunklands {
  class SceneBase : public Napi::ObjectWrap<SceneBase> {
  public:
    static Napi::FunctionReference constructor;
    static void Initialize(Napi::Env env);

  public:
    SceneBase(const Napi::CallbackInfo& info);

  public:
    void Prepare(const Napi::CallbackInfo& info);
    void Render(const Napi::CallbackInfo& info);

  private:
    std::string vsh_src_;
    std::string fsh_src_;
    
    GLuint vao_;
    GLuint vb_;

    GLuint vsh_;
    GLuint fsh_;
    GLuint program_;
  };
}

#endif