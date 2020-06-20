
#ifndef __CHUNKLANDS_MATH_H__
#define __CHUNKLANDS_MATH_H__

#include <boost/functional/hash.hpp>
#include <cmath>
#include <glm/ext/vector_float1.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <ostream>

namespace chunklands::math {

  template<int N, class T> using vec = glm::vec<N, T, glm::defaultp>;
  template<int N> using fvec = vec<N, float>;
  template<int N> using ivec = vec<N, int>;
  template<class T> using vec1 = vec<1, T>;
  template<class T> using vec2 = vec<2, T>;
  template<class T> using vec3 = vec<3, T>;

  using fvec1 = fvec<1>;
  using fvec2 = fvec<2>;
  using fvec3 = fvec<3>;

  using ivec1 = ivec<1>;
  using ivec2 = ivec<2>;
  using ivec3 = ivec<3>;

  struct ivec3_hasher {
    std::size_t operator()(const ivec3& v) const;
  };

  inline ivec3 get_center_chunk(const fvec3& pos, unsigned chunk_size);

  template<int N, class T>
  struct AABB {
    explicit AABB(vec<N, T> origin, vec<N, T> span) : origin(std::move(origin)), span(std::move(span)) {
#ifndef NDEBUG
      for (unsigned i = 0; i < N; i++) {
        assert(span[i] >= (T)0);
      }
#endif
    }

    explicit AABB() {
      span.x = (T)-1;
    }

    bool IsEmpty() const {
      return span.x == (T)-1;
    }

    bool operator!() const {
      return IsEmpty();
    }

    operator bool() const {
      return !IsEmpty();
    }

    bool operator==(const AABB<N, T>& rhs) const {
      return (IsEmpty() && rhs.IsEmpty()) || (origin == rhs.origin && span == rhs.span);
    }

    vec<N, T> origin,
              span;
  };

  template<class T>
  using AABB1 = AABB<1, T>;
  template<class T>
  using AABB2 = AABB<2, T>;
  template<class T>
  using AABB3 = AABB<3, T>;

  using fAABB1 = AABB<1, float>;
  using fAABB2 = AABB<2, float>;
  using fAABB3 = AABB<3, float>;

  using iAABB1 = AABB<1, int>;
  using iAABB2 = AABB<2, int>;
  using iAABB3 = AABB<3, int>;


  class box_iterator {
    friend std::ostream& operator<<(std::ostream& os, const box_iterator& it);

  public:
    explicit box_iterator() = default;
    explicit box_iterator(const ivec3& a, const ivec3& b)
      : a_(a), b_(b), current_(a) {

      assert(a_.x <= b_.x);
      assert(a_.y <= b_.y);
      assert(a_.z <= b_.z);
    }

    bool operator==(const box_iterator& rhs) const {
      return current_ == rhs.current_;
    }

    bool operator!=(const box_iterator& rhs) const {
      return !(*this == rhs);
    }

    box_iterator& operator++();

    const ivec3& operator*() const {
      return current_;
    }

    static box_iterator end(const ivec3& min, const ivec3& max);

  private:
    ivec3 a_,
          b_,
          current_;
  };

  std::ostream& operator<<(std::ostream& os, const box_iterator& it);

  class chunk_pos_in_box {
  public:
    explicit chunk_pos_in_box(const fAABB3& box, unsigned chunk_size);

  public:
    box_iterator begin() const {
      return box_iterator {chunk_min_, chunk_max_};
    }

    const box_iterator& end() const {
      return end_;
    }
  private:
    ivec3 chunk_min_,
          chunk_max_;

    box_iterator end_;
  };

  class block_pos_in_box {
  public:
    explicit block_pos_in_box(const fAABB3& box, const ivec3& chunk_pos, unsigned chunk_size);

  public:
    box_iterator begin() const {
      if (empty_) {
        return end();
      }

      return box_iterator {block_min_, block_max_};
    }

    const box_iterator& end() const {
      return end_;
    }
  private:
    bool  empty_;
    ivec3 block_min_,
          block_max_;

    box_iterator end_;
  };


  

  // vec<>
  template<class T> std::ostream& operator<<(std::ostream& os, const vec1<T>& v);
  template<class T> std::ostream& operator<<(std::ostream& os, const vec2<T>& v);
  template<class T> std::ostream& operator<<(std::ostream& os, const vec3<T>& v);

  template<int N, class T> inline vec1<T> x_vec(const vec<N, T>& v);
  template<int N, class T> inline vec1<T> y_vec(const vec<N, T>& v);
  template<int N, class T> inline vec1<T> z_vec(const vec<N, T>& v);

  // AABB<N, T>
  template<int N, class T> AABB<N, T> operator+(const AABB<N, T>& box, const vec<N, T>& v);
  template<int N, class T> std::ostream& operator<<(std::ostream& os, const AABB<N, T>& box);

  template<int N, class T> inline AABB<1, T> x_aabb(const AABB<N, T>& b);
  template<int N, class T> inline AABB<1, T> y_aabb(const AABB<N, T>& b);
  template<int N, class T> inline AABB<1, T> z_aabb(const AABB<N, T>& b);

  // AABB<1, T>
  template<class T> AABB<1, T> operator|(const AABB<1, T>& box, const vec1<T>& v);
  template<class T> AABB<1, T> operator&(const AABB<1, T>& a, const AABB<1, T>& b);

  // AABB<2, T>
  template<class T> AABB<2, T> operator|(const AABB<2, T>& b, const vec2<T>& v);
  template<class T> AABB<2, T> operator&(const AABB<2, T>& a, const AABB<2, T>& b);

  // AABB<3, T>
  template<class T> AABB<3, T> operator|(const AABB<3, T>& box, const vec3<T>& v);
  template<class T> AABB<3, T> operator&(const AABB<3, T>& a, const AABB<3, T>& b);

  // util
  template<class T> inline ivec3 floor(const vec3<T>& v);
  template<class T> inline ivec3 ceil(const vec3<T>& v);
  template<class T> iAABB3 bound(const AABB<3, T>& box);

  template<int N, class T> inline T chess_distance(const vec<N, T>& a, const vec<N, T>& b);

  template<class T>
  using collision_time = AABB1<T>;

  enum CollisionAxis {
    kNone = 0,
    kX    = 1,
    kY    = 2,
    kZ    = 4,
  };

  template<class T>
  struct axis_collision {
    int axis;
    collision_time<T> time;

    bool operator==(const axis_collision<T>& rhs) const {
      return axis == rhs.axis && time == rhs.time;
    }
  };

  template<class T>
  std::ostream& operator<<(std::ostream& os, const axis_collision<T>& c);

  template<class T> collision_time<T> collision(const AABB1<T>& moving, const vec1<T>& v, const AABB1<T>& fixed);
  template<class T> axis_collision<T> collision_3d(const AABB3<T>& moving, const vec3<T>& v, const AABB3<T>& fixed);

  template<class T> inline bool is_infinity(const T& value);
}

#include "math.inl"

#endif