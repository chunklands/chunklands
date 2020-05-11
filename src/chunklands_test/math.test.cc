#include <gtest/gtest.h>
#include "math.h"

namespace chunklands::math {

  TEST(chunklands__math, get_center_chunk) {
    EXPECT_EQ(get_center_chunk(vec3{ 0,   0,   0}, 16), ivec3({ 0,  0,  0}));
    EXPECT_EQ(get_center_chunk(vec3{25,   0,   3}, 16), ivec3({ 1,  0,  0}));
    EXPECT_EQ(get_center_chunk(vec3{ 0,  16,  31}, 16), ivec3({ 0,  1,  1}));
    EXPECT_EQ(get_center_chunk(vec3{-1,   0,   0}, 16), ivec3({-1,  0,  0}));
    EXPECT_EQ(get_center_chunk(vec3{20, -25, -60}, 16), ivec3({ 1, -2, -4}));
  }

  TEST(chunklands__math, AABB_1_) {
    EXPECT_EQ(
      (AABB1 {1.f,  2.f} + vec1{3.f}),
      (AABB1 {4.f,  2.f})
    );

    EXPECT_EQ(
      (AABB1 {2.f, 5.f} | vec1 {2.f}),
      (AABB1 {2.f, 7.f})
    );
  }

  TEST(chunklands__math, AABB_2_) {
    EXPECT_EQ(
      (AABB2 {-1.f, 3.f,  2.f, 1.f} + vec2{1.f, -4.f}),
      (AABB2 {0.f, -1.f,  2.f, 1.f})
    );
  }

  TEST(chunklands__math, AABB_3_) {
    EXPECT_EQ(
      (AABB3 {-2.f,  0.f, -2.f,  3.f, 4.f, 4.f} + vec3{1.f, -4.f, -3.f}),
      (AABB3 {-1.f, -4.f, -5.f,  3.f, 4.f, 4.f})
    );
  }
}