#ifndef __CHUNKLANDS_ENGINE_MATH_MATH_HXX__
#define __CHUNKLANDS_ENGINE_MATH_MATH_HXX__

#include <boost/functional/hash.hpp>
#include <cmath>
#include <glm/ext/vector_float1.hpp>
#include <glm/ext/vector_relational.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <ostream>

namespace chunklands::engine::math {

template <int N, class T>
using vec = glm::vec<N, T, glm::defaultp>;
template <int N>
using fvec = vec<N, float>;
template <int N>
using dvec = vec<N, double>;
template <int N>
using ivec = vec<N, int>;
template <class T>
using vec1 = vec<1, T>;
template <class T>
using vec2 = vec<2, T>;
template <class T>
using vec3 = vec<3, T>;
template <class T>
using vec4 = vec<4, T>;

using fvec1 = fvec<1>;
using fvec2 = fvec<2>;
using fvec3 = fvec<3>;
using fvec4 = fvec<4>;

using dvec1 = dvec<1>;
using dvec2 = dvec<2>;
using dvec3 = dvec<3>;
using dvec4 = dvec<4>;

using ivec1 = ivec<1>;
using ivec2 = ivec<2>;
using ivec3 = ivec<3>;

struct ivec3_hasher {
    std::size_t operator()(const ivec3& v) const;
};

inline ivec3 get_center_chunk(const fvec3& pos, unsigned chunk_size);
inline ivec3 get_center_chunk(const ivec3& pos, unsigned chunk_size);

inline ivec3 get_pos_in_chunk(const fvec3& pos, unsigned chunk_size);
inline ivec3 get_pos_in_chunk(const ivec3& pos, unsigned chunk_size);

template <int N, class T>
struct AABB {
    explicit AABB(vec<N, T> origin, vec<N, T> span)
        : origin(std::move(origin))
        , span(std::move(span))
    {
    }

    explicit AABB()
    {
        origin = vec<N, T>(static_cast<T>(0));
        span = vec<N, T>(static_cast<T>(-1));
    }

    template <class R>
    explicit AABB(const AABB<N, R>& other)
        : origin(other.origin)
        , span(other.span)
    {
    }

    static AABB<N, T> from_range(const vec<N, T>& a, const vec<N, T>& b)
    {
        vec<N, T> origin, span;
        for (int i = 0; i < N; i++) {
            if (b[i] >= a[i]) {
                origin[i] = a[i];
                span[i] = b[i] - a[i];
            } else {
                origin[i] = b[i];
                span[i] = a[i] - b[i];
            }
        }

        return AABB<N, T> { origin, span };
    }

    static AABB<N, T> from_points(const std::initializer_list<vec<N, T>>& list)
    {
        if (list.size() == 0) {
            return AABB<N, T>();
        }

        auto&& it = list.begin();
        vec<N, T> min = *it, max = *it;
        it++;

        for (; it != list.end(); it++) {
            auto&& current = *it;
            for (int i = 0; i < N; i++) {
                if (current[i] < min[i]) {
                    min[i] = current[i];
                }

                if (current[i] > max[i]) {
                    max[i] = current[i];
                }
            }
        }

        return AABB<N, T>(min, max - min);
    }

    bool operator==(const AABB<N, T>& rhs) const
    {
        return origin == rhs.origin && span == rhs.span;
    }

    vec<N, T> max() const
    {
        return origin + span;
    }

    vec<N, T> min() const
    {
        return origin;
    }

    bool empty() const
    {
        return glm::any(glm::lessThan(span, vec<N, T>(0)));
    }

    vec<N, T> origin,
        span;
};

template <class T>
using AABB1 = AABB<1, T>;
template <class T>
using AABB2 = AABB<2, T>;
template <class T>
using AABB3 = AABB<3, T>;

using fAABB1 = AABB<1, float>;
using fAABB2 = AABB<2, float>;
using fAABB3 = AABB<3, float>;

using dAABB1 = AABB<1, double>;
using dAABB2 = AABB<2, double>;
using dAABB3 = AABB<3, double>;

using iAABB1 = AABB<1, int>;
using iAABB2 = AABB<2, int>;
using iAABB3 = AABB<3, int>;

template <int N, class T>
struct Line {
    explicit Line(vec<N, T> origin, vec<N, T> span)
        : origin(std::move(origin))
        , span(std::move(span))
    {
    }

    vec<N, T> origin,
        span;
};

template <class T>
using Line1 = Line<1, T>;
template <class T>
using Line2 = Line<2, T>;
template <class T>
using Line3 = Line<3, T>;

using fLine1 = Line<1, float>;
using fLine2 = Line<2, float>;
using fLine3 = Line<3, float>;

using iLine1 = Line<1, int>;
using iLine2 = Line<2, int>;
using iLine3 = Line<3, int>;

class box_iterator {
    friend std::ostream& operator<<(std::ostream& os, const box_iterator& it);

public:
    explicit box_iterator() = default;
    explicit box_iterator(const ivec3& a, const ivec3& b)
        : a_(a)
        , b_(b)
        , current_(a)
    {

        assert(a_.x <= b_.x);
        assert(a_.y <= b_.y);
        assert(a_.z <= b_.z);
    }

    bool operator==(const box_iterator& rhs) const
    {
        return current_ == rhs.current_;
    }

    bool operator!=(const box_iterator& rhs) const
    {
        return !(*this == rhs);
    }

    box_iterator& operator++();

    const ivec3& operator*() const
    {
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
    box_iterator begin() const
    {
        return box_iterator { chunk_min_, chunk_max_ };
    }

    const box_iterator& end() const
    {
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
    box_iterator begin() const
    {
        if (empty_) {
            return end();
        }

        return box_iterator { block_min_, block_max_ };
    }

    const box_iterator& end() const
    {
        return end_;
    }

private:
    bool empty_;
    ivec3 block_min_,
        block_max_;

    box_iterator end_;
};

// vec<>
template <int N, class T>
inline vec1<T> x_vec(const vec<N, T>& v);
template <int N, class T>
inline vec1<T> y_vec(const vec<N, T>& v);
template <int N, class T>
inline vec1<T> z_vec(const vec<N, T>& v);

// AABB<N, T>
template <int N, class T>
AABB<N, T> operator+(const AABB<N, T>& box, const vec<N, T>& v);
template <int N, class T>
std::ostream& operator<<(std::ostream& os, const AABB<N, T>& box);

template <int N, class T>
inline AABB<1, T> x_aabb(const AABB<N, T>& b);
template <int N, class T>
inline AABB<1, T> y_aabb(const AABB<N, T>& b);
template <int N, class T>
inline AABB<1, T> z_aabb(const AABB<N, T>& b);

// AABB<1, T>
template <class T>
AABB<1, T> operator|(const AABB<1, T>& box, const vec1<T>& v);
template <class T>
AABB<1, T> operator&(const AABB<1, T>& a, const AABB<1, T>& b);

// AABB<2, T>
template <class T>
AABB<2, T> operator|(const AABB<2, T>& b, const vec2<T>& v);
template <class T>
AABB<2, T> operator&(const AABB<2, T>& a, const AABB<2, T>& b);

// AABB<3, T>
template <class T>
AABB<3, T> operator|(const AABB<3, T>& box, const vec3<T>& v);
template <class T>
AABB<3, T> operator&(const AABB<3, T>& a, const AABB<3, T>& b);

// Line
template <int N, class T>
std::ostream& operator<<(std::ostream& os, const Line<N, T>& line);

// util
template <class T>
inline ivec3 floor(const vec3<T>& v);
template <class T>
inline ivec3 ceil(const vec3<T>& v);
template <class T>
iAABB3 bound(const AABB<3, T>& box);

template <int N, class T>
inline T chess_distance(const vec<N, T>& a, const vec<N, T>& b);

enum CollisionAxis {
    kNone = 0,
    kX = 1,
    kY = 2,
    kZ = 4,
};

template <class R, class T>
R floor(T x);
template <class R, class T>
R ceil(T x);
}

namespace std {
template <class T>
ostream& operator<<(ostream& os, const chunklands::engine::math::vec1<T>& v);
template <class T>
ostream& operator<<(ostream& os, const chunklands::engine::math::vec2<T>& v);
template <class T>
ostream& operator<<(ostream& os, const chunklands::engine::math::vec3<T>& v);
}

#include "math.inl"

#endif