
#include "MovementController.hxx"
#include <chunklands/debug.hxx>
#include <chunklands/misc/Profiler.hxx>
#include <iostream>

namespace chunklands::engine {

  JS_DEF_WRAP(MovementController)

  int MovementController::AddMovement(math::fvec3 outstanding_movement) {
    DURATION_METRIC("movement_time");

    int axis = math::CollisionAxis::kNone;

    int next_collision_index = 0;
    while(math::chess_distance(outstanding_movement, math::fvec3(0, 0, 0)) > 0.f) {
      // infinite loop?
      assert(next_collision_index < 100);

      if (DEBUG_COLLISION) {
        std::cout << "Next collision #" << next_collision_index << std::endl;
      }

      auto&& impulse = js_CollisionSystem->ProcessNextCollision(player_box_ + js_Camera->GetPosition(), outstanding_movement);
      if (impulse.collision.never() || !impulse.collision.in_unittime()) {
        js_Camera->AddPos(outstanding_movement);
        break;
      }

      outstanding_movement = impulse.outstanding;
      js_Camera->AddPos(impulse.collision_free);

      axis |= impulse.is_x_collision ? math::CollisionAxis::kX : math::CollisionAxis::kNone;
      axis |= impulse.is_y_collision ? math::CollisionAxis::kY : math::CollisionAxis::kNone;
      axis |= impulse.is_z_collision ? math::CollisionAxis::kZ : math::CollisionAxis::kNone;

      ++next_collision_index;
    }

    return axis;
  }

} // namespace chunklands::engine