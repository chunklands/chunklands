
#ifndef __CHUNKLANDS_MATH_H__
#define __CHUNKLANDS_MATH_H__

#include <boost/functional/hash.hpp>
#include <cmath>
#include <glm/ext/vector_float1.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace chunklands::math {

  template<int N>
  using vec = glm::vec<N, float, glm::defaultp>;
  template<int N>
  using ivec = glm::vec<N, int, glm::defaultp>;

  using vec1 = vec<1>;
  using vec2 = vec<2>;
  using vec3 = vec<3>;

  using ivec1 = ivec<1>;
  using ivec2 = ivec<2>;
  using ivec3 = ivec<3>;

  struct ivec3_hasher {
    std::size_t operator()(const ivec3& v) const {
      std::size_t seed = 0;
      boost::hash_combine(seed, boost::hash_value(v.x));
      boost::hash_combine(seed, boost::hash_value(v.y));
      boost::hash_combine(seed, boost::hash_value(v.z));

      return seed;
    }
  };

  inline ivec3 get_center_chunk(const vec3& pos, unsigned chunk_size) {
    return ivec3(pos.x >= 0 ? pos.x : pos.x - chunk_size,
                      pos.y >= 0 ? pos.y : pos.y - chunk_size,
                      pos.z >= 0 ? pos.z : pos.z - chunk_size
    ) / (int)chunk_size;
  }

  template<int N>
  struct AABB {
    AABB(float ox, float sx) : AABB(vec1 {ox}, vec1 {sx}) {}
    AABB(float ox, float oy, float sx, float sy) : AABB(vec2 {ox, oy}, vec2 {sx, sy}) {}
    AABB(float ox, float oy, float oz, float sx, float sy, float sz) : AABB(vec3 {ox, oy, oz}, vec3 {sx, sy, sz}) {}
    AABB(vec<N> origin, vec<N> span) : origin(std::move(origin)), span(std::move(span)) {
#ifndef NDEBUG
      for (unsigned i = 0; i < N; i++) {
        assert(span[i] >= 0.f);
      }
#endif
    }

    bool operator==(const AABB<N>& rhs) const {
      return origin == rhs.origin && span == rhs.span;
    }

    AABB<N> operator+(const vec<N>& v) {
      return AABB<N> {
        .origin = origin + v,
        .span = span
      };
    }

    AABB<N> operator|(const vec<N>& v) const;    

    vec<N>  origin,
            span;
  };

  using AABB1 = AABB<1>;
  using AABB2 = AABB<2>;
  using AABB3 = AABB<3>;


  // enum CollisionType {
  //   kNoCollision,
  //   kIsColliding,
  //   kWillCollide,
  // };

  // struct collision {
  //   CollisionType collision;
  //   float distance;
  // };

  // constexpr float REL_DISTANCE_COLLISION = 0.f;

  // inline float rel_distance(const AABB<1>& moving, const AABB<1>& fixed) {
  //   float a = moving.origin.x,
  //         b = moving.origin.x + moving.span.x,
  //         c = fixed.origin.x,
  //         d = fixed.origin.x + fixed.span.x;

  //   //  a-------b
  //   //               c-----d
  //   float cb = c - b;
  //   if (cb > 0) {
  //     return cb; // + distance
  //   }

  //   //               a-----b
  //   //  c-------d
  //   float da = d - a;
  //   if (da < 0) {
  //     return da; // - distance
  //   }

  //   return REL_DISTANCE_COLLISION;
  // }

  // inline collision calculate_collision_1d(const AABB<1>& moving, const vec<1>& velo, const AABB<1>& fixed) {
  //   float dist = rel_distance(moving, fixed);
  //   if (dist == REL_DISTANCE_COLLISION) {
  //     return {
  //       .collision = kIsColliding,
  //       .distance  = 0.f
  //     };
  //   }
  //   // CHECK moving => fixed -neg, +pos values is this correct? I'm too drunk right now :(
  //   if (velo.x >= 0) {
  //     if (dist < velo.x) {
  //       return {
  //         .collision = kWillCollide,
  //         .distance  = dist,
  //       };
  //     } else {
  //       return {
  //         .collision = kNoCollision,
  //         .distance  = velo.x,
  //       };
  //     }
  //   } else {
  //     if (dist > velo.x) {
  //       return {
  //         .collision = kWillCollide,
  //         .distance  = dist,
  //       };
  //     } else {
  //       return {
  //         .collision = kNoCollision,
  //         .distance  = velo.x,
  //       };
  //     }
  //   }
  // }

  // inline collision calculate_collision_2d(const AABB<2>& moving, const vec<2>& velo, const AABB<2>& fixed) {
  //   auto&& x_coll = calculate_collision_1d(
  //     AABB<1> {
  //       vec<1> {moving.origin.x},
  //       vec<1> {moving.span.x}
  //     },
  //     vec<1>{velo.x},
  //     AABB<1>{
  //       vec<1> {fixed.origin.x},
  //       vec<1> {fixed.span.x}
  //     }
  //   );

  //   if (x_coll.collision == kIsColliding) {
  //     return x_coll;
  //   }

  //   auto&& y_coll = calculate_collision_1d(
  //     AABB<1> {
  //       vec<1> {moving.origin.y},
  //       vec<1> {moving.span.y}
  //     },
  //     vec<1>{velo.y},
  //     AABB<1>{
  //       vec<1> {fixed.origin.y},
  //       vec<1> {fixed.span.y}
  //     }
  //   );

  //   if (y_coll.collision == kIsColliding) {
  //     return y_coll;
  //   }

  //   if (x_coll.collision == kWillCollide) {
  //     if (y_coll.collision == kNoCollision) {
  //       return x_coll;
  //     } else {
  //       if (x_coll.distance <)
  //     }
  //   }

  //   if (x_coll.collision == kNoCollision && y_coll.collision == )
  // }

  // inline float calculate_collision_3d(const AABB3& moving, const glm::vec3& velo, const AABB3& fixed) {
    
  // }

  class chunk_pos_in_box_iterator {
  public:
    chunk_pos_in_box_iterator() = default;
    chunk_pos_in_box_iterator(const ivec3& chunk_min, const ivec3& chunk_max)
      : chunk_min_(chunk_min), chunk_max_(chunk_max), current_(chunk_min) {

      assert(chunk_min_.x <= chunk_max_.x);
      assert(chunk_min_.y <= chunk_max_.y);
      assert(chunk_min_.z <= chunk_max_.z);
    }

    bool operator==(const chunk_pos_in_box_iterator& rhs) const {
      return current_ == rhs.current_;
    }

    bool operator!=(const chunk_pos_in_box_iterator& rhs) const {
      return !(*this == rhs);
    }

    chunk_pos_in_box_iterator& operator++() {
      assert(current_.z <= chunk_max_.z);

      current_.x++;
      if (current_.x > chunk_max_.x) {
        current_.x = chunk_min_.x;
        current_.y++;
        if (current_.y > chunk_max_.y) {
          current_.y = chunk_min_.y;
          current_.z++;
        }
      }

      return *this;
    }

    const ivec3& operator*() const {
      return current_;
    }

  private:
    ivec3 chunk_min_,
          chunk_max_,
          current_;
  };

  class chunk_pos_in_box {
  public:
    chunk_pos_in_box(const AABB3& box, unsigned chunk_size) {
      chunk_min_ = get_center_chunk(box.origin           , chunk_size);
      chunk_max_ = get_center_chunk(box.origin + box.span, chunk_size);

      ivec3 chunk_end = {chunk_min_.x, chunk_min_.y, chunk_max_.z + 1};
      end_ = {chunk_end, chunk_end};
    }

  public:
    chunk_pos_in_box_iterator begin() const {
      return {chunk_min_, chunk_max_};
    }

    const chunk_pos_in_box_iterator& end() const {
      return end_;
    }
  private:
    ivec3 chunk_min_,
          chunk_max_;

    chunk_pos_in_box_iterator end_;
  };

  inline float chess_distance(const vec1& a, const vec1& b) {
    return std::fabs(a.x - b.x);
  }

  inline float chess_distance(const vec2& a, const vec2& b) {
    return std::fabs(a.x - b.x + a.y - b.y);
  }

  inline float chess_distance(const vec3& a, const vec3& b) {
    return std::fabs(a.x - b.x + a.y - b.y + a.z - b.z);
  }
}

#endif