#ifndef __CHUNKLANDS_SCENEBASE_H__
#define __CHUNKLANDS_SCENEBASE_H__

#include <napi.h>
#include "gl.h"
#include "napi/object_wrap_util.h"
#include "napi/PersistentObjectWrap.h"
#include "WindowBase.h"
#include "WorldBase.h"

namespace chunklands {
  class SceneBase : public Napi::ObjectWrap<SceneBase> {
    DECLARE_OBJECT_WRAP(SceneBase)
    DECLARE_OBJECT_WRAP_CB(void SetWindow)
    DECLARE_OBJECT_WRAP_CB(void SetWorld)

  public:
    virtual ~SceneBase();
  
  public:
    void Prepare();
    void Update(double diff);
    void Render(double diff);

    void UpdateViewport();
    void UpdateViewport(int width, int height);

  private:
    void InitializeGLBuffers(int width, int height);
    void DeleteGLBuffers();

  private:
    NapiExt::PersistentObjectWrap<WindowBase> window_;
    boost::signals2::scoped_connection window_on_resize_conn_;
    boost::signals2::scoped_connection window_on_cursor_move_conn_;
    glm::ivec2 last_cursor_pos_;

    NapiExt::PersistentObjectWrap<WorldBase> world_;

    GLuint renderbuffer_;
    GLuint framebuffer_;
    GLuint position_texture_;
    GLuint normal_texture_;
    GLuint color_texture_;

    glm::ivec2 buffer_size_;
  };
}

#endif