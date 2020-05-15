#ifndef __CHUNKLANDS_MATH_INL__
#define __CHUNKLANDS_MATH_INL__

namespace chunklands::math {
  ivec3 get_center_chunk(const fvec3& pos, unsigned chunk_size) {
    return ivec3(pos.x >= 0 ? pos.x : pos.x - chunk_size,
                      pos.y >= 0 ? pos.y : pos.y - chunk_size,
                      pos.z >= 0 ? pos.z : pos.z - chunk_size
    ) / (int)chunk_size;
  }

  template<class T>
  std::ostream& operator<<(std::ostream& os, const vec1<T>& v) {
    return os << "vec{ x=" << v.x << " }";
  }

  template<class T>
  std::ostream& operator<<(std::ostream& os, const vec2<T>& v) {
    return os << "vec{ x=" << v.x << ", y=" << v.y << " }";
  }

  template<class T>
  std::ostream& operator<<(std::ostream& os, const vec3<T>& v) {
    return os << "vec{ x=" << v.x << ", y=" << v.y << ", z=" << v.z << " }";
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
      return {};
    }

    return AABB<N, T> {
      .origin = box.origin + v,
      .span   = box.span
    };
  }

  template<int N, class T>
  std::ostream& operator<<(std::ostream& os, const AABB<N, T>& box) {
    return os << "AABB{ origin={" << box.origin << "}, span={" << box.span << "} }";
  }

  template<int N, class T>
  inline AABB<1, T> x_aabb(const AABB<N, T>& b) {
    return { vec1<T>{b.origin.x}, vec1<T>{b.span.x} };
  }

  template<int N, class T>
  inline AABB<1, T> y_aabb(const AABB<N, T>& b) {
    return { vec1<T>{b.origin.y}, vec1<T>{b.span.y} };
  }

  template<int N, class T>
  inline AABB<1, T> z_aabb(const AABB<N, T>& b) {
    return { vec1<T>{b.origin.z}, vec1<T>{b.span.z} };
  }


  template<class T>
  AABB<1, T> operator|(const AABB<1, T>& box, const vec1<T>& v) {
    if (box.IsEmpty()) {
      return {};
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
      return {};
    }

    if (a.origin.x + a.span.x <= b.origin.x || b.origin.x + b.span.x <= a.origin.x) {
      return {};
    }

    T origin = std::max(a.origin.x           , b.origin.x),
      span   = std::min(a.origin.x + a.span.x, b.origin.x + b.span.x) - origin;

    return { vec1<T>{origin}, vec1<T>{span} };
  }

  template<class T>
  AABB<2, T> operator|(const AABB<2, T>& b, const vec2<T>& v) {
    if (b.IsEmpty()) {
      return {};
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
      return {};
    }

    AABB<1, T> x = x_aabb(a) & x_aabb(b);
    if (!x) {
      return {};
    }

    AABB<1, T> y = y_aabb(a) & y_aabb(b);
    if (!y) {
      return {};
    }

    return AABB<2, T>{
      vec2<T>{x.origin.x, y.origin.x},
      vec2<T>{x.span.x,   y.span.x},
    };
  }

  template<class T>
  AABB<3, T> operator|(const AABB<3, T>& box, const vec3<T>& v) {
    if (box.IsEmpty()) {
      return {};
    }

    AABB<1, T> b_x = x_aabb(box) | x_vec(v);
    AABB<1, T> b_y = y_aabb(box) | y_vec(v);
    AABB<1, T> b_z = z_aabb(box) | z_vec(v);

    return AABB<3, T> {
      vec3<T> {b_x.origin.x, b_y.origin.x, b_z.origin.x},
      vec3<T> {b_x.span.x,   b_y.span.x  , b_z.span.x}
    };
  }


  template<class T>
  AABB<3, T> operator&(const AABB<3, T>& a, const AABB<3, T>& b) {
    if(a.IsEmpty() || b.IsEmpty()) {
      return {};
    }

    AABB<1, T> x = x_aabb(a) & x_aabb(b);
    if (!x) {
      return {};
    }

    AABB<1, T> y = y_aabb(a) & y_aabb(b);
    if (!y) {
      return {};
    }

    AABB<1, T> z = z_aabb(a) & z_aabb(b);
    if (!z) {
      return {};
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
}

#endif