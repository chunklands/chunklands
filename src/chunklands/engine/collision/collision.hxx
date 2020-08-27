#ifndef __CHUNKLANDS_ENGINE_COLLISION_COLLISION_HXX__
#define __CHUNKLANDS_ENGINE_COLLISION_COLLISION_HXX__

#include <cmath>
#include <glm/vec3.hpp>

namespace chunklands::engine::collision {

struct AABB3D {
    inline float x_min() const { return origin.x; }
    inline float x_max() const { return origin.x + span.x; }
    inline float y_min() const { return origin.y; }
    inline float y_max() const { return origin.y + span.y; }
    inline float z_min() const { return origin.z; }
    inline float z_max() const { return origin.z + span.z; }

    glm::vec3 origin, span;
};

struct collision_1D {
    float start = 0.f, end = 0.f;

    explicit collision_1D()
        : start(-1.f)
        , end(-2.f)
    {
    }
    explicit collision_1D(float start, float end)
        : start(start)
        , end(end)
    {
    }
    explicit collision_1D(float st_min, float st_max, float dy_min, float dy_max, float v);

    inline bool never() const { return end < start; }
    inline bool overlap() const { return start < 0.f && end > 0.f; }
    inline bool will() const { return start >= 0.f; }
    inline bool in_unittime() const { return start >= 0.f && start < 1.f; }

    inline collision_1D overlap(const collision_1D& other) const
    {
        return collision_1D(std::max(start, other.start), std::min(end, other.end));
    }

    inline bool starts_with(const collision_1D& other) const
    {
        return start == other.start;
    }

    inline bool earlier(const collision_1D& other) const
    {
        return start < other.start;
    }
};

namespace {
    constexpr float COLLISION_EPSILON = 0.00001f;

    inline float snuggle_movement(float st_min, float st_max, float dy_min, float dy_max, float v)
    {
        if (v >= 0.f) {
            return std::max(0.f, st_min - dy_max - COLLISION_EPSILON);
        } else {
            return std::min(0.f, st_max - dy_min + COLLISION_EPSILON);
        }
    }
}

struct collision_impulse {
    explicit collision_impulse() {}
    explicit collision_impulse(collision_1D collision)
        : collision(std::move(collision))
    {
    }
    explicit collision_impulse(const AABB3D& st, const AABB3D& dy, const glm::vec3& v);

    bool is_more_relevant_than(const collision_impulse& other) const;

    inline int axis_collisions() const
    {
        return (is_x_collision ? 1 : 0) + (is_y_collision ? 1 : 0) + (is_z_collision ? 1 : 0);
    }

    collision_1D collision;
    glm::vec3 outstanding { 0.f };
    glm::vec3 collision_free { 0.f };
    bool is_x_collision = false,
         is_y_collision = false,
         is_z_collision = false;
};

} // namespace chunklands::engine::collision

#endif