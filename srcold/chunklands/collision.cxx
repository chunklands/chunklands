
#include "collision.hxx"

namespace chunklands::collision {

  collision_1D::collision_1D(float st_min, float st_max, float dy_min, float dy_max, float v) {
    assert(st_min <= st_max);
    assert(dy_min <= dy_max);

    if (v < 0.f) {
      v = -v;
      std::swap(st_min, dy_min);
      std::swap(st_max, dy_max);
    }

    assert(v >= 0.f);

    // 1       2     4       8 (axis)
    // [ fixed ]     [dynamic> : start = 1 - 8 = -7, end = 2 - 4 = -4
    // [dynamic>     [ fixed ] : start = 4 - 2 = +2, end = 8 - 1 = +7
    // [ fixed [     ]dynamic> : start = 1 - 8 = -7, end = 4 - 2 = +2
    // [dynamic[     > fixed ] : start = 2 - 4 = -2, end = 8 - 1 = +7
    const float start_len = st_min - dy_max;
    const float end_len   = st_max - dy_min;
    assert(start_len <= end_len);

    start = start_len / v;
    end   = end_len   / v;
  }

  collision_impulse::collision_impulse(const AABB3D& st, const AABB3D& dy, const glm::vec3& v) {
    const collision_1D x_collision(st.x_min(), st.x_max(), dy.x_min(), dy.x_max(), v.x);
    const collision_1D y_collision(st.y_min(), st.y_max(), dy.y_min(), dy.y_max(), v.y);
    const collision_1D z_collision(st.z_min(), st.z_max(), dy.z_min(), dy.z_max(), v.z);

    collision = x_collision.overlap(y_collision).overlap(z_collision);

    if (collision.starts_with(x_collision)) {
      is_x_collision = true;
      collision_free.x = snuggle_movement(st.x_min(), st.x_max(), dy.x_min(), dy.x_max(), v.x);
    } else {
      collision_free.x = v.x * collision.start;
      outstanding.x = v.x - collision_free.x;
    }

    if (collision.starts_with(y_collision)) {
      is_y_collision = true;
      collision_free.y = snuggle_movement(st.y_min(), st.y_max(), dy.y_min(), dy.y_max(), v.y);
    } else {
      collision_free.y = v.y * collision.start;
      outstanding.y = v.y - collision_free.y;
    }

    if (collision.starts_with(z_collision)) {
      is_z_collision = true;
      collision_free.z = snuggle_movement(st.z_min(), st.z_max(), dy.z_min(), dy.z_max(), v.z);
    } else {
      collision_free.z = v.z * collision.start;
      outstanding.z = v.z - collision_free.z;
    }
  }

  bool collision_impulse::is_more_relevant_than(const collision_impulse& other) const {
    if (!collision.in_unittime()) {
      return false;
    }

    if (other.collision.never()) {
      return true;
    }

    if (collision.earlier(other.collision)) {
      return true;
    }

    if (collision.starts_with(other.collision)) {
      return axis_collisions() < other.axis_collisions();
    }

    

    return false;
  }

} // namespace chunklands::collision