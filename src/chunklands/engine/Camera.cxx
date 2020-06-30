
#include "Camera.hxx"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>
#include <chunklands/jsmath.hxx>

namespace chunklands::engine {

  JS_DEF_WRAP(Camera)

  void Camera::AddLook(float yaw_rad, float pitch_rad) {
    constexpr float pitch_break = (.5f * M_PI) - .1;
    
    look_.x += yaw_rad;
    look_.x = fmodf(look_.x, 2.f * M_PI);

    look_.y += pitch_rad;
    look_.y = std::max(std::min(look_.y, pitch_break), -pitch_break);
  }

  void Camera::UpdateViewportRatio(int width, int height) {
    constexpr float fovy_degree = 75.f;
    
    proj_ = glm::perspective(glm::radians(fovy_degree), (float)width / height, 0.1f, 1000.0f);
  }

  void Camera::Update(double) {
    glm::vec3 look_center(-sinf(look_.x) * cosf(look_.y),
                          sinf(look_.y),
                          -cosf(look_.x) * cosf(look_.y));

    view_        = glm::lookAt(pos_, pos_ + look_center, glm::vec3(0.f, 1.f, 0.f));
    view_skybox_ = glm::lookAt(glm::vec3(0, 0, 0), look_center, glm::vec3(0.f, 1.f, 0.f));
  }

  void Camera::JSCall_SetPosition(JSCbi info) {
    pos_.x = info[0].ToNumber().FloatValue();
    pos_.y = info[1].ToNumber().FloatValue();
    pos_.z = info[2].ToNumber().FloatValue();
  }

  JSValue Camera::JSCall_GetPosition(JSCbi info) {
    return jsmath::vec3(info.Env(), pos_);
  }

  JSValue Camera::JSCall_GetLook(JSCbi info) {
    return jsmath::vec2(info.Env(), look_);
  }

} // namespace chunklands::engine