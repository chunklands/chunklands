#ifndef __CHUNKLANDS_ENGINE_ICOLLISIONSYSTEM_HXX__
#define __CHUNKLANDS_ENGINE_ICOLLISIONSYSTEM_HXX__

#include <ostream>
#include <chunklands/math.hxx>
#include <chunklands/collision.hxx>

namespace chunklands::engine {

  class ICollisionSystem {
  public:
    virtual collision::collision_impulse ProcessNextCollision(const math::fAABB3& box, const math::fvec3& movement) = 0;
  };

} // namespace chunklands::engine

#endif