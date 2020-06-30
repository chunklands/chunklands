#ifndef __CHUNKLANDS_MODULES_ENGINE_ICOLLISIONSYSTEM_HXX__
#define __CHUNKLANDS_MODULES_ENGINE_ICOLLISIONSYSTEM_HXX__

#include <ostream>
#include <chunklands/math.hxx>

namespace chunklands::modules::engine {

  struct collision_result {
    int prio;
    int axis;
    float ctime;
    math::fvec3 collisionfree_movement;
    math::fvec3 outstanding_movement;
  };

  std::ostream& operator<<(std::ostream& os, const collision_result& c);

  class ICollisionSystem {
  public:
    virtual collision_result ProcessNextCollision(const math::fAABB3 &box, const math::fvec3 &movement) = 0;
  };

} // namespace chunklands::modules::engine

#endif