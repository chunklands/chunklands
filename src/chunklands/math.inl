#ifndef __CHUNKLANDS_MATH_INL__
#define __CHUNKLANDS_MATH_INL__

namespace chunklands::math {
  ivec3 get_center_chunk(const fvec3& pos, unsigned chunk_size) {
    return ivec3(
      pos.x >= 0.f ? int(pos.x) : floor<int>(pos.x) - int(chunk_size) + 1,
      pos.y >= 0.f ? int(pos.y) : floor<int>(pos.y) - int(chunk_size) + 1,
      pos.z >= 0.f ? int(pos.z) : floor<int>(pos.z) - int(chunk_size) + 1
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
      pos.x >= 0.f ? int(pos.x) % chunk_size : (floor<int>(pos.x)) % chunk_size,
      pos.y >= 0.f ? int(pos.y) % chunk_size : (floor<int>(pos.y)) % chunk_size,
      pos.z >= 0.f ? int(pos.z) % chunk_size : (floor<int>(pos.z)) % chunk_size
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
    return AABB<N, T> {
      box.origin + v,
      box.span
    };
  }

  template<int N, class T>
  std::ostream& operator<<(std::ostream& os, const AABB<N, T>& box) {
    return os << "AABB(" << box.origin << ", " << box.span << ")";
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
    if (v.x >= (T)0) {
      return AABB<1, T> {
        box.origin,
        box.span + v
      };
    } else {
      const vec1<T> nv = -v;
      return AABB<1, T> {
        box.origin - nv, // v < 0
        box.span   + nv  // v < 0
      };
    }
  }

  template<class T>
  AABB<1, T> operator&(const AABB<1, T>& a, const AABB<1, T>& b) {
    if (a.empty() || b.empty()) {
      return AABB<1, T> {};
    }

    if (a.max().x <= b.min().x || b.max().x <= a.min().x) {
      return AABB<1, T> {};
    }

    const T origin = std::max(a.min().x, b.min().x),
            span   = std::min(a.max().x, b.max().x) - origin;
    
    return AABB<1, T> { vec1<T>{origin}, vec1<T>{span} };
  }

  template<class T>
  AABB<2, T> operator|(const AABB<2, T>& b, const vec2<T>& v) {
    const AABB<1, T> b_x = x_aabb(b) | x_vec(v);
    const AABB<1, T> b_y = y_aabb(b) | y_vec(v);

    return AABB<2, T> {
      vec2<T> {b_x.origin.x, b_y.origin.x},
      vec2<T> {b_x.span.x,   b_y.span.x}
    };
  }

  template<class T>
  AABB<2, T> operator&(const AABB<2, T>& a, const AABB<2, T>& b) {
    const AABB<1, T> x = x_aabb(a) & x_aabb(b);
    const AABB<1, T> y = y_aabb(a) & y_aabb(b);

    return AABB<2, T>{
      vec2<T>{x.origin.x, y.origin.x},
      vec2<T>{x.span.x,   y.span.x},
    };
  }

  template<class T>
  AABB<3, T> operator|(const AABB<3, T>& box, const vec3<T>& v) {
    const AABB<1, T> b_x = x_aabb(box) | x_vec(v);
    const AABB<1, T> b_y = y_aabb(box) | y_vec(v);
    const AABB<1, T> b_z = z_aabb(box) | z_vec(v);

    // we checked for IsEmpty()
    return AABB<3, T> {
      vec3<T> {b_x.origin.x, b_y.origin.x, b_z.origin.x},
      vec3<T> {b_x.span.x,   b_y.span.x  , b_z.span.x}
    };
  }


  template<class T>
  AABB<3, T> operator&(const AABB<3, T>& a, const AABB<3, T>& b) {
    const AABB<1, T> x = x_aabb(a) & x_aabb(b);
    const AABB<1, T> y = y_aabb(a) & y_aabb(b);
    const AABB<1, T> z = z_aabb(a) & z_aabb(b);

    return AABB<3, T>{
      vec3<T>{x.origin.x, y.origin.x, z.origin.x},
      vec3<T>{x.span.x,   y.span.x,   z.span.x}
    };
  }

  template<int N, class T>
  std::ostream& operator<<(std::ostream& os, const Line<N, T>& line) {
    return os << "Line(" << line.origin << ", " << line.span << ")";
  }

  template<class T>
  inline ivec3 floor(const vec3<T>& v) {
    return ivec3{
      floor<int>(v.x),
      floor<int>(v.y),
      floor<int>(v.z)
    };
  }

  template<class T>
  inline ivec3 ceil(const vec3<T>& v) {
    return ivec3{
      ceil<int>(v.x),
      ceil<int>(v.y),
      ceil<int>(v.z)
    };
  }


  template<class T>
  iAABB3 bound(const AABB<3, T>& box) {
    const ivec3 origin = floor(box.min());
    const ivec3 span   = ceil(box.max()) - origin;

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

  template<class R, class T> R floor(T x) {
    static_assert(std::numeric_limits<R>::is_integer, "return value must be integer");
    static_assert(!std::numeric_limits<T>::is_integer, "param must be fp number");

    const R v = static_cast<R>(x);
    return static_cast<T>(v) <= x ? v : v - static_cast<R>(1);
  }

  template<class R, class T> R ceil(T x) {
    static_assert(std::numeric_limits<R>::is_integer, "return value must be integer");
    static_assert(!std::numeric_limits<T>::is_integer, "param must be fp number");

    const R v = static_cast<R>(x);
    return static_cast<T>(v) >= x ? v : v + static_cast<R>(1);
  }
}

namespace std {
  template<class T>
  std::ostream& operator<<(std::ostream& os, const chunklands::math::vec1<T>& v) {
    return os << "vec(" << v.x << ")";
  }

  template<class T>
  std::ostream& operator<<(std::ostream& os, const chunklands::math::vec2<T>& v) {
    return os << "vec(" << v.x << ", " << v.y << ")";
  }

  template<class T>
  std::ostream& operator<<(std::ostream& os, const chunklands::math::vec3<T>& v) {
    return os << "vec(" << v.x << ", " << v.y << ", " << v.z << ")";
  }
}

#endif