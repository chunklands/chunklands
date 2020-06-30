#ifndef __CHUNKLANDS_MODULES_ENGINE_MOVEMENTCONTROLLER_HXX__
#define __CHUNKLANDS_MODULES_ENGINE_MOVEMENTCONTROLLER_HXX__

#include <chunklands/js.hxx>
#include "ICollisionSystem.hxx"
#include <chunklands/math.hxx>
#include "Camera.hxx"

namespace chunklands::modules::engine {

  class MovementController : public JSObjectWrap<MovementController> {
    JS_IMPL_WRAP(MovementController, ONE_ARG({
      JS_SETTER(CollisionSystem),
      JS_SETTER(Camera),
    }))

    JS_IMPL_ABSTRACT_WRAP_SETTER(ICollisionSystem, CollisionSystem)
    JS_IMPL_SETTER_WRAP(Camera, Camera)

  public:
    int AddMovement(math::fvec3 outstanding_movement);

  private:
    math::fAABB3 player_box_ {
      {-.4f, -1.4f, -.4f},
      { .8f,  1.6f,  .8f}
    };
  };

} // namespace chunklands::modules::engine

#endif