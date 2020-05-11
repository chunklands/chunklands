#include "math.h"

namespace chunklands::math {

  template<>
  AABB<1> AABB<1>::operator|(const vec<1>& v) {
    if (v.x >= 0) {
      return AABB<1> {
        origin,
        span + v
      };
    } else {
      return AABB<1> {
        origin + v, // v < 0
        span   - v  // v < 0
      };
    }
  }

  template<>
  AABB<2> AABB<2>::operator|(const vec<2>& v) {
    vec<1> v_x = vec<1> {v.x};
    vec<1> v_y = vec<1> {v.y};

    AABB<1> b_x = AABB<1> {
      vec<1> {origin.x},
      vec<1> {span.x}
    } | v_x;

    AABB<1> b_y = AABB<1> {
      vec<1> {origin.y},
      vec<1> {span.y}
    } | v_y;

    return AABB<2> {
      vec<2> {b_x.origin.x, b_y.origin.x},
      vec<2> {b_x.span.x,   b_y.span.x}
    };
  }

  template<>
  AABB<3> AABB<3>::operator|(const vec<3>& v) {
    vec<1> v_x = vec<1> {v.x};
    vec<1> v_y = vec<1> {v.y};
    vec<1> v_z = vec<1> {v.z};

    AABB<1> b_x = AABB<1> {
      vec<1> {origin.x},
      vec<1> {span.x}
    } | v_x;

    AABB<1> b_y = AABB<1> {
      vec<1> {origin.y},
      vec<1> {span.y}
    } | v_y;

    AABB<1> b_z = AABB<1> {
      vec<1> {origin.z},
      vec<1> {span.z}
    } | v_z;

    return AABB<3> {
      vec<3> {b_x.origin.x, b_y.origin.x, b_z.origin.x},
      vec<3> {b_x.span.x,   b_y.span.x  , b_z.span.x}
    };
  }
}