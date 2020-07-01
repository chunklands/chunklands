#ifndef __CHUNKLANDS_MATH_INL__
#define __CHUNKLANDS_MATH_INL__

namespace chunklands::math {
  ivec3 __WITH_BUG_get_center_chunk(const fvec3& pos, unsigned chunk_size) {
    return ivec3(
      pos.x >= 0 ? pos.x : pos.x - chunk_size,
      pos.y >= 0 ? pos.y : pos.y - chunk_size,
      pos.z >= 0 ? pos.z : pos.z - chunk_size
    ) / (int)chunk_size;
  }

  ivec3 get_center_chunk(const fvec3& pos, unsigned chunk_size) {
    return ivec3(
      pos.x >= 0 ? pos.x : pos.x - chunk_size + 1,
      pos.y >= 0 ? pos.y : pos.y - chunk_size + 1,
      pos.z >= 0 ? pos.z : pos.z - chunk_size + 1
    ) / (int)chunk_size;
  }

  ivec3 get_center_chunk(const ivec3& pos, unsigned chunk_size) {
    return ivec3(pos.x >= 0 ? pos.x : pos.x - chunk_size + 1,
                      pos.y >= 0 ? pos.y : pos.y - chunk_size + 1,
                      pos.z >= 0 ? pos.z : pos.z - chunk_size + 1
    ) / (int)chunk_size;
  }

  ivec3 get_pos_in_chunk(const fvec3& pos, unsigned chunk_size) {
    return ivec3(
      pos.x >= 0 ? (int)pos.x % chunk_size : ((int)pos.x - 1) % chunk_size,
      pos.y >= 0 ? (int)pos.y % chunk_size : ((int)pos.y - 1) % chunk_size,
      pos.z >= 0 ? (int)pos.z % chunk_size : ((int)pos.z - 1) % chunk_size
    );
  }

  ivec3 get_pos_in_chunk(const ivec3& pos, unsigned chunk_size) {
    return ivec3(
      pos.x % chunk_size,
      pos.y % chunk_size,
      pos.z % chunk_size
    );
  }

  template<int N, class T>
  inline vec1<T> x_vec(const vec<N, T>& v) {
    return vec1<T>{v.x};
  }

  template<int N, class T>
  inline vec1<T> y_vec(const vec<N, T>& v) {
    return vec1<T>{v.y};
  }

  template<int N, class T>
  inline vec1<T> z_vec(const vec<N, T>& v) {
    return vec1<T>{v.z};
  }

  template<int N, class T>
  AABB<N, T> operator+(const AABB<N, T>& box, const vec<N, T>& v) {
    if (box.IsEmpty()) {
      return AABB<N, T> {};
    }

    return AABB<N, T> {
      box.origin + v,
      box.span
    };
  }

  template<int N, class T>
  std::ostream& operator<<(std::ostream& os, const AABB<N, T>& box) {
    return os << "AABB{ origin={" << box.origin << "}, span={" << box.span << "} }";
  }

  template<int N, class T>
  inline AABB<1, T> x_aabb(const AABB<N, T>& b) {
    return AABB<1, T> { vec1<T>{b.origin.x}, vec1<T>{b.span.x} };
  }

  template<int N, class T>
  inline AABB<1, T> y_aabb(const AABB<N, T>& b) {
    return AABB<1, T> { vec1<T>{b.origin.y}, vec1<T>{b.span.y} };
  }

  template<int N, class T>
  inline AABB<1, T> z_aabb(const AABB<N, T>& b) {
    return AABB<1, T> { vec1<T>{b.origin.z}, vec1<T>{b.span.z} };
  }


  template<class T>
  AABB<1, T> operator|(const AABB<1, T>& box, const vec1<T>& v) {
    if (box.IsEmpty()) {
      return AABB<1, T> {};
    }

    if (v.x >= (T)0) {
      return AABB<1, T> {
        box.origin,
        box.span + v
      };
    } else {
      return AABB<1, T> {
        box.origin + v, // v < 0
        box.span   - v  // v < 0
      };
    }
  }

  template<class T>
  AABB<1, T> operator&(const AABB<1, T>& a, const AABB<1, T>& b) {
    if(a.IsEmpty() || b.IsEmpty()) {
      return AABB<1, T> {};
    }

    if (std::numeric_limits<T>::has_infinity) {
      if (std::isinf(a.origin.x) && std::isinf(a.span.x)) { // NOLINT (clang-analyzer-core.CallAndMessage)
        return b;
      } else if (std::isinf(b.origin.x) && std::isinf(b.span.x)) { // NOLINT (clang-analyzer-core.CallAndMessage)
        return a;
      }
    }

    if (a.origin.x + a.span.x <= b.origin.x || b.origin.x + b.span.x <= a.origin.x) {
      return AABB<1, T> {};
    }

    T origin = std::max(a.origin.x           , b.origin.x),
      span   = std::min(a.origin.x + a.span.x, b.origin.x + b.span.x) - origin;

    return AABB<1, T> { vec1<T>{origin}, vec1<T>{span} };
  }

  template<class T>
  AABB<2, T> operator|(const AABB<2, T>& b, const vec2<T>& v) {
    if (b.IsEmpty()) {
      return AABB<2, T>{};
    }

    AABB<1, T> b_x = x_aabb(b) | x_vec(v);
    AABB<1, T> b_y = y_aabb(b) | y_vec(v);

    return AABB<2, T> {
      vec2<T> {b_x.origin.x, b_y.origin.x},
      vec2<T> {b_x.span.x,   b_y.span.x}
    };
  }

  template<class T>
  AABB<2, T> operator&(const AABB<2, T>& a, const AABB<2, T>& b) {
    if(a.IsEmpty() || b.IsEmpty()) {
      return AABB<2, T>{};
    }

    AABB<1, T> x = x_aabb(a) & x_aabb(b);
    if (!x) {
      return AABB<2, T>{};
    }

    AABB<1, T> y = y_aabb(a) & y_aabb(b);
    if (!y) {
      return AABB<2, T>{};
    }

    return AABB<2, T>{
      vec2<T>{x.origin.x, y.origin.x},
      vec2<T>{x.span.x,   y.span.x},
    };
  }

  template<class T>
  AABB<3, T> operator|(const AABB<3, T>& box, const vec3<T>& v) {
    if (box.IsEmpty()) {
      return AABB<3, T> {};
    }

    AABB<1, T> b_x = x_aabb(box) | x_vec(v);
    AABB<1, T> b_y = y_aabb(box) | y_vec(v);
    AABB<1, T> b_z = z_aabb(box) | z_vec(v);

    // we checked for IsEmpty()
    return AABB<3, T> {
      vec3<T> {b_x.origin.x, b_y.origin.x, b_z.origin.x}, // NOLINT (clang-analyzer-core.CallAndMessage)
      vec3<T> {b_x.span.x,   b_y.span.x  , b_z.span.x}    // NOLINT (clang-analyzer-core.CallAndMessage)
    };
  }


  template<class T>
  AABB<3, T> operator&(const AABB<3, T>& a, const AABB<3, T>& b) {
    if(a.IsEmpty() || b.IsEmpty()) {
      return AABB<3, T> {};
    }

    AABB<1, T> x = x_aabb(a) & x_aabb(b);
    if (!x) {
      return AABB<3, T> {};
    }

    AABB<1, T> y = y_aabb(a) & y_aabb(b);
    if (!y) {
      return AABB<3, T> {};
    }

    AABB<1, T> z = z_aabb(a) & z_aabb(b);
    if (!z) {
      return AABB<3, T> {};
    }

    return AABB<3, T>{
      vec3<T>{x.origin.x, y.origin.x, z.origin.x},
      vec3<T>{x.span.x,   y.span.x,   z.span.x}
    };
  }

  template<class T>
  inline ivec3 floor(const vec3<T>& v) {
    return ivec3{
      (int)std::floor(v.x),
      (int)std::floor(v.y),
      (int)std::floor(v.z)
    };
  }

  template<class T>
  inline ivec3 ceil(const vec3<T>& v) {
    return ivec3{
      (int)std::ceil(v.x),
      (int)std::ceil(v.y),
      (int)std::ceil(v.z)
    };
  }


  template<class T>
  iAABB3 bound(const AABB<3, T>& box) {
    ivec3 origin = floor(box.origin);
    ivec3 span   = ceil(box.origin + box.span) - origin;

    return iAABB3{origin, span};
  }

  template<class T>
  inline T chess_distance(const vec1<T>& a, const vec1<T>& b) {
    return std::abs(a.x - b.x);
  }

  template<class T>
  inline T chess_distance(const vec2<T>& a, const vec2<T>& b) {
    return std::abs((a.x - b.x) + (a.y - b.y));
  }

  template<class T>
  inline T chess_distance(const vec3<T>& a, const vec3<T>& b) {
    return std::abs((a.x - b.x) + (a.y - b.y) + (a.z - b.z));
  }

  template<class T>
  std::ostream& operator<<(std::ostream& os, const axis_collision<T>& c) {
    os << "axis_collision { axis=";
    if (c.axis == kNone) {
      os << "None";
    } else {
      if (c.axis & kX) {
        os << "X";
      }

      if (c.axis & kY) {
        os << "Y";
      }

      if (c.axis & kZ) {
        os << "Z";
      }
    }

    return os << ", time=" << c.time << " }";
  }

  template<class T>
  collision_time<T> collision(const AABB1<T>& moving, const vec1<T>& v, const AABB1<T>& fixed) {
    if (v.x > 0) {
      
      if (moving.origin.x >= fixed.origin.x + fixed.span.x) {
        // [fixed]
        //          [moving]--->
        return collision_time<T> {};
      }

      // A)      [fixed]
      //    [moving]--->
      //
      // B)               [fixed]
      //    [moving]--->
      T dist_collision_start =  fixed.origin.x                 - (moving.origin.x + moving.span.x);
      T dist_collision_end   = (fixed.origin.x + fixed.span.x) -  moving.origin.x;
      assert(dist_collision_end >= dist_collision_start);

      return collision_time<T>{
        vec1<T>{  dist_collision_start                       / v.x },
        vec1<T>{ (dist_collision_end - dist_collision_start) / v.x }
      };
    }
    
    if (v.x < 0) {
      
      if (moving.origin.x + moving.span.x <= fixed.origin.x) {
        // <---[moving]
        //               [fixed]
        return collision_time<T> {};
      }

      // C)  <---[moving]
      //      [fixed]
      //
      // D)           <---[moving]
      //      [fixed]
      T dist_collision_start =  moving.origin.x                  - (fixed.origin.x + fixed.span.x);
      T dist_collision_end   = (moving.origin.x + moving.span.x) - fixed.origin.x;
      assert(dist_collision_end >= dist_collision_start);

      return collision_time<T>{
        vec1<T>{  dist_collision_start                         / -v.x },
        vec1<T>{ (dist_collision_end   - dist_collision_start) / -v.x }
      };
    }

    // v = 0

    if (fixed & moving) {
      return collision_time<T>{
        vec1<T>{-std::numeric_limits<T>::infinity()},
        vec1<T>{+std::numeric_limits<T>::infinity()}
      };
    }

    return collision_time<T> {};
  }

  template<class T>
  axis_collision<T> collision_3d(const AABB3<T>& moving, const vec3<T>& v, const AABB3<T>& fixed) {
    auto&& x_result = collision(x_aabb(moving), x_vec(v), x_aabb(fixed));
    auto&& y_result = collision(y_aabb(moving), y_vec(v), y_aabb(fixed));
    auto&& z_result = collision(z_aabb(moving), z_vec(v), z_aabb(fixed));

    axis_collision<T> result{
      .axis = CollisionAxis::kNone,
      .time = x_result & y_result & z_result
    };

    if (!result.time.IsEmpty()) {
      if (!x_result.IsEmpty() &&
        x_result.origin.x >= result.time.origin.x // NOLINT (clang-analyzer-core.UndefinedBinaryOperatorResult)
      ) {
        result.axis |= CollisionAxis::kX;
      }

      if (!y_result.IsEmpty() &&
        y_result.origin.x >= result.time.origin.x // NOLINT (clang-analyzer-core.UndefinedBinaryOperatorResult)
      ) {
        result.axis |= CollisionAxis::kY;
      }

      if (!z_result.IsEmpty() &&
        z_result.origin.x >= result.time.origin.x // NOLINT (clang-analyzer-core.UndefinedBinaryOperatorResult)
      ) {
        result.axis |= CollisionAxis::kZ;
      }
    }

    return result;
  }
}

namespace std {
  template<class T>
  std::ostream& operator<<(std::ostream& os, const chunklands::math::vec1<T>& v) {
    return os << "vec{ x=" << v.x << " }";
  }

  template<class T>
  std::ostream& operator<<(std::ostream& os, const chunklands::math::vec2<T>& v) {
    return os << "vec{ x=" << v.x << ", y=" << v.y << " }";
  }

  template<class T>
  std::ostream& operator<<(std::ostream& os, const chunklands::math::vec3<T>& v) {
    return os << "vec{ x=" << v.x << ", y=" << v.y << ", z=" << v.z << " }";
  }

}

#endif