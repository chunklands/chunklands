#ifndef __CHUNKLANDS_ENGINE_CAMERA_HXX__
#define __CHUNKLANDS_ENGINE_CAMERA_HXX__

#include <chunklands/js.hxx>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace chunklands::engine {

  class Camera : public JSObjectWrap<Camera> {
    JS_IMPL_WRAP(Camera, ONE_ARG({
      JS_SETTER(Position),
      JS_GETTER(Position),
      JS_GETTER(Look),
    }))

    JS_DECL_CB_VOID(SetPosition)
    JS_DECL_CB(GetPosition)
    JS_DECL_CB(GetLook)

  public:
    void UpdateViewportRatio(int width, int height);

    void Update(double diff);
    
    void AddLook(float yaw_rad, float pitch_rad);
    void AddPos(const glm::vec3& v) {
      pos_ += v;
    }

    const glm::vec3& GetPosition() const {
      return pos_;
    }

    const glm::vec2& GetLook() const {
      return look_;
    }

    const glm::mat4& GetProjection() const {
      return proj_;
    }

    const glm::mat4& GetView() const {
      return view_;
    }

    const glm::mat4& GetViewSkybox() const {
      return view_skybox_;
    }
  private:

    glm::vec3 pos_ {0.f, 0.f, 0.f};
    glm::vec2 look_ {0.f, 0.f};

    glm::mat4 view_;
    glm::mat4 proj_;
    
    glm::mat4 view_skybox_;
  };

} // namespace chunklands::engine

#endif