
#include "MovementController.hxx"
#include <chunklands/modules/misc/misc_module.hxx>
#include <chunklands/debug.hxx>

namespace chunklands::engine {

  JS_DEF_WRAP(MovementController)

  int MovementController::AddMovement(math::fvec3 outstanding_movement) {
    DURATION_METRIC("movement_time");

    int axis = math::CollisionAxis::kNone;

    int next_collision_index = 0;
    while(math::chess_distance(outstanding_movement, math::fvec3(0, 0, 0)) > 0.f) {

      if (DEBUG_COLLISION) {
        std::cout << "Next collision #" << next_collision_index << std::endl;
      }

      collision_result result = js_CollisionSystem->ProcessNextCollision(player_box_ + js_Camera->GetPosition(), outstanding_movement);
      js_Camera->AddPos(result.collisionfree_movement);
      outstanding_movement = result.outstanding_movement;
      axis |= result.axis;

      ++next_collision_index;
    }

    return axis;
  }

  std::ostream& operator<<(std::ostream& os, const collision_result& c) {
    return os << "collision_result { ctime=" << c.ctime << ", collisionfree_movement=" << c.collisionfree_movement
      << ", outstanding_movement=" << c.outstanding_movement << " }";
  }

} // namespace chunklands::engine