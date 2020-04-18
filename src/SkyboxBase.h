#ifndef __CHUNKLANDS_SKYBOXBASE_H__
#define __CHUNKLANDS_SKYBOXBASE_H__

#include <napi.h>
#include "napi/object_wrap_util.h"

#include "gl.h"
#include "GLTexture.h"

namespace chunklands {
  class SkyboxBase : public Napi::ObjectWrap<SkyboxBase> {
    DECLARE_OBJECT_WRAP(SkyboxBase)
    DECLARE_OBJECT_WRAP_CB(void Initialize)

  public:
    ~SkyboxBase();

  public:
    void Render();

  private:
    GLuint vao_ = 0;
    GLuint vbo_ = 0;

    GLTexture texture_;
  };
}

#endif
