#include <gtest/gtest.h>
#include <chunklands/math.h>
#include <chunklands/modules/misc/misc_module.h>

namespace chunklands::math {

  TEST(chunklands__math, get_center_chunk) {
    EXPECT_EQ(get_center_chunk(vec3{ 0,   0,   0}, 16), ivec3({ 0,  0,  0}));
    EXPECT_EQ(get_center_chunk(vec3{25,   0,   3}, 16), ivec3({ 1,  0,  0}));
    EXPECT_EQ(get_center_chunk(vec3{ 0,  16,  31}, 16), ivec3({ 0,  1,  1}));
    EXPECT_EQ(get_center_chunk(vec3{-1,   0,   0}, 16), ivec3({-1,  0,  0}));
    EXPECT_EQ(get_center_chunk(vec3{20, -25, -60}, 16), ivec3({ 1, -2, -4}));
  }

  TEST(chunklands__math, AABB1_operator_plus) {
    EXPECT_EQ(
      (AABB1 {1.f,  2.f} + vec1{3.f}),
      (AABB1 {4.f,  2.f})
    );
  }

  TEST(chunklands__math, AABB1_operator_pipe) {
    EXPECT_EQ(
      (AABB1 {2.f, 5.f} | vec1 {2.f}),
      (AABB1 {2.f, 7.f})
    );
  }

  TEST(chunklands__math, AABB1_operator_pipe_negative) {
    EXPECT_EQ(
      (AABB1 {2.f, 5.f} | vec1 {-2.f}),
      (AABB1 {0.f, 7.f})
    );
  }

  TEST(chunklands__math, AABB1_operator_pipe_zero) {
    EXPECT_EQ(
      (AABB1 {2.f, 5.f} | vec1 {0.f}),
      (AABB1 {2.f, 5.f})
    );
  }

  TEST(chunklands__math, AABB2_operator_plus) {
    EXPECT_EQ(
      (AABB2 {-1.f, 3.f,  2.f, 1.f} + vec2{1.f, -4.f}),
      (AABB2 {0.f, -1.f,  2.f, 1.f})
    );
  }

  TEST(chunklands__math, AABB2_operator_pipe) {
    EXPECT_EQ(
      (AABB2 {1.f, -1.f, 3.f, 5.f} | vec2 {1.f, 2.f}),
      (AABB2 {1.f, -1.f, 4.f, 7.f})
    );
  }

  TEST(chunklands__math, AABB2_operator_pipe_negative) {
    EXPECT_EQ(
      (AABB2 {1.f, -1.f, 3.f, 5.f} | vec2 {-1.f, -2.f}),
      (AABB2 {0.f, -3.f, 4.f, 7.f})
    );
  }

  TEST(chunklands__math, AABB2_operator_pipe_zero) {
    EXPECT_EQ(
      (AABB2 {1.f, -1.f, 3.f, 5.f} | vec2 {0.f, 0.f}),
      (AABB2 {1.f, -1.f, 3.f, 5.f})
    );
  }

  TEST(chunklands__math, AABB3_operator_plus) {
    EXPECT_EQ(
      (AABB3 {-2.f,  0.f, -2.f,  3.f, 4.f, 4.f} + vec3{1.f, -4.f, -3.f}),
      (AABB3 {-1.f, -4.f, -5.f,  3.f, 4.f, 4.f})
    );
  }

  TEST(chunklands__math, AABB3_operator_pipe) {
    EXPECT_EQ(
      (AABB3 {1.f, -1.f, -2.f, 3.f, 5.f, 1.f} | vec3 {3.f, 1.f, 1.f}),
      (AABB3 {1.f, -1.f, -2.f, 6.f, 6.f, 2.f})
    );
  }

  TEST(chunklands__math, AABB3_operator_negative) {
    EXPECT_EQ(
      (AABB3 { 1.f, -1.f, -2.f, 3.f, 5.f, 1.f} | vec3 {-3.f, -1.f, -1.f}),
      (AABB3 {-2.f, -2.f, -3.f, 6.f, 6.f, 2.f})
    );
  }

  TEST(chunklands__math, AABB3_operator_zero) {
    EXPECT_EQ(
      (AABB3 {1.f, -1.f, -2.f, 3.f, 5.f, 1.f} | vec3 {0.f, 0.f, 0.f}),
      (AABB3 {1.f, -1.f, -2.f, 3.f, 5.f, 1.f})
    );
  }

  TEST(chunklands__math, chunk_pos_in_box) {
    chunk_pos_in_box c {AABB3{-4.01f, -43.73f, 193.3f, 52.7f, 23.3f, 3.f}, 16};

    auto&& it = std::begin(c);
    auto&& end = std::end(c);

    for (int z = 12; z <= 12; z++) {
      for (int y = -3; y <= -2; y++) {
        for (int x = -1; x <= 3; x++) {
          EXPECT_NE(it, end);
          ivec3 actual = *it;
          EXPECT_EQ(actual, ivec3(x, y, z));

          ++it;
        }
      }
    }

    EXPECT_EQ(it, end);
  }
}
