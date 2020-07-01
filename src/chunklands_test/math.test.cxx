#include <gtest/gtest.h>
#include <chunklands/math.hxx>

namespace chunklands::math {

  TEST(chunklands__math, get_center_chunk) {

    // int
    ASSERT_EQ((get_center_chunk(ivec3 {  0,   0,   0}, 16)), (ivec3{  0,  0,  0}));
    ASSERT_EQ((get_center_chunk(ivec3 { 25,   0,   3}, 16)), (ivec3{  1,  0,  0}));
    ASSERT_EQ((get_center_chunk(ivec3 {  0,  16,  31}, 16)), (ivec3{  0,  1,  1}));
    ASSERT_EQ((get_center_chunk(ivec3 { -1,   0,   0}, 16)), (ivec3{ -1,  0,  0}));
    ASSERT_EQ((get_center_chunk(ivec3 { 20, -25, -60}, 16)), (ivec3{  1, -2, -4}));
    ASSERT_EQ((get_center_chunk(ivec3 {-15, -15, -15}, 16)), (ivec3{ -1, -1, -1}));
    ASSERT_EQ((get_center_chunk(ivec3 {-16, -16, -16}, 16)), (ivec3{ -1, -1, -1}));
    ASSERT_EQ((get_center_chunk(ivec3 {-17, -17, -17}, 16)), (ivec3{ -2, -2, -2}));

    // float
    ASSERT_EQ((get_center_chunk(fvec3 {  0,   0,   0}, 16)), (ivec3{  0,  0,  0}));
    ASSERT_EQ((get_center_chunk(fvec3 { 25,   0,   3}, 16)), (ivec3{  1,  0,  0}));
    ASSERT_EQ((get_center_chunk(fvec3 {  0,  16,  31}, 16)), (ivec3{  0,  1,  1}));
    ASSERT_EQ((get_center_chunk(fvec3 { -1,   0,   0}, 16)), (ivec3{ -1,  0,  0}));
    ASSERT_EQ((get_center_chunk(fvec3 { 20, -25, -60}, 16)), (ivec3{  1, -2, -4}));
    ASSERT_EQ((get_center_chunk(fvec3 {-15, -15, -15}, 16)), (ivec3{ -1, -1, -1}));
    ASSERT_EQ((get_center_chunk(fvec3 {-16, -16, -16}, 16)), (ivec3{ -1, -1, -1}));
    ASSERT_EQ((get_center_chunk(fvec3 {-17, -17, -17}, 16)), (ivec3{ -2, -2, -2}));

  }

  TEST(chunklands__math, get_pos_in_chunk) {

    // int
    ASSERT_EQ((get_pos_in_chunk(ivec3 { 0,    0,    0}, 16)), (ivec3{ 0,  0,  0}));
    ASSERT_EQ((get_pos_in_chunk(ivec3 { 1,    1,    1}, 16)), (ivec3{ 1,  1,  1}));
    ASSERT_EQ((get_pos_in_chunk(ivec3 {-1,  -16,  -15}, 16)), (ivec3{15,  0,  1}));
    ASSERT_EQ((get_pos_in_chunk(ivec3 {-15,  -1,  -16}, 16)), (ivec3{ 1, 15,  0}));
    ASSERT_EQ((get_pos_in_chunk(ivec3 {-16, -15,   -1}, 16)), (ivec3{ 0,  1, 15}));

    // float
    ASSERT_EQ((get_pos_in_chunk(fvec3 {  0,       0,       0   }, 16)), (ivec3{ 0,  0,  0}));
    ASSERT_EQ((get_pos_in_chunk(fvec3 {  1,       1,       1   }, 16)), (ivec3{ 1,  1,  1}));
    ASSERT_EQ((get_pos_in_chunk(fvec3 {  0.1f,   14.1f,   15.1f}, 16)), (ivec3{ 0, 14, 15}));
    ASSERT_EQ((get_pos_in_chunk(fvec3 { 16.1f,   17.1f,   15.9f}, 16)), (ivec3{ 0,  1, 15}));
    ASSERT_EQ((get_pos_in_chunk(fvec3 { -0.1f,  -14.1f,  -15.1f}, 16)), (ivec3{15,  1,  0}));
    ASSERT_EQ((get_pos_in_chunk(fvec3 {-16.1f,  -17.1f,  -15.9f}, 16)), (ivec3{15, 14,  0}));

  }

  TEST(chunklands__math, fAABB1) {
    // translate
    ASSERT_EQ(
      (fAABB1{ fvec1{1}, fvec1{2} } + fvec1{3}),
      (fAABB1{ fvec1{4}, fvec1{2} })
    );

    // movingbox
    ASSERT_EQ(
      (fAABB1{ fvec1{2}, fvec1{5} } | fvec1{2}),
      (fAABB1{ fvec1{2}, fvec1{7} })
    );

    // movingbox negative
    ASSERT_EQ(
      (fAABB1{ fvec1{2}, fvec1{5} } | fvec1{-2}),
      (fAABB1{ fvec1{0}, fvec1{7} })
    );

    // movingbox zero
    ASSERT_EQ(
      (fAABB1{ fvec1{2}, fvec1{5} } | fvec1{0}),
      (fAABB1{ fvec1{2}, fvec1{5} })
    );

    // intersection empty
    // a===a
    //       b===b
    ASSERT_TRUE((fAABB1{ fvec1{1}, fvec1{1} } & fAABB1{ fvec1{3}, fvec1{1} }).IsEmpty());

    // intersection empty
    //       a===a
    // b===b
    ASSERT_TRUE((fAABB1{ fvec1{3}, fvec1{1} } & fAABB1{ fvec1{1}, fvec1{1} }).IsEmpty());

    // intersection left contains right
    // a=====a
    //   b=b
    ASSERT_EQ(
      (fAABB1{ fvec1{1}, fvec1{3} } & fAABB1{ fvec1{2}, fvec1{1} }),
      (fAABB1{ fvec1{2}, fvec1{1} })
    );

    // intersection right contains left
    //   a=a
    // b=====b
    ASSERT_EQ(
      (fAABB1{ fvec1{4}, fvec1{3} } & fAABB1{ fvec1{-2}, fvec1{10} }),
      (fAABB1{ fvec1{4}, fvec1{3} })
    );

    // intersection right intersects left
    // a=====a
    //    b=====b
    ASSERT_EQ(
      (fAABB1{ fvec1{0}, fvec1{4} } & fAABB1{ fvec1{2}, fvec1{4} }),
      (fAABB1{ fvec1{2}, fvec1{2} })
    );

    // intersection left intersects right
    //    a=====a
    // b=====b
    ASSERT_EQ(
      (fAABB1{ fvec1{3}, fvec1{5} } & fAABB1{ fvec1{-3}, fvec1{7} }),
      (fAABB1{ fvec1{3}, fvec1{1} })
    );
  }

  TEST(chunklands__math, fAABB2) {
    // translate
    ASSERT_EQ(
      (fAABB2{ fvec2{-1, 3}, fvec2{2, 1} } + fvec2{1, -4}),
      (fAABB2{ fvec2{0, -1}, fvec2{2, 1} })
    );

    // movingbox
    ASSERT_EQ(
      (fAABB2{ fvec2{1, -1}, fvec2{3, 5} } | fvec2{1, 2}),
      (fAABB2{ fvec2{1, -1}, fvec2{4, 7} })
    );

    // movingbox negative
    ASSERT_EQ(
      (fAABB2{ fvec2{1, -1}, fvec2{3, 5} } | fvec2{-1, -2}),
      (fAABB2{ fvec2{0, -3}, fvec2{4, 7} })
    );

    // movingbox zero
    ASSERT_EQ(
      (fAABB2{ fvec2{1, -1}, fvec2{3, 5} } | fvec2{0, 0}),
      (fAABB2{ fvec2{1, -1}, fvec2{3, 5} })
    );

    // intersection empty
    ASSERT_TRUE((fAABB2{ fvec2{0, 0}, fvec2{4, 4} } & fAABB2{ fvec2{5, 2}, fvec2{1, 1} }).IsEmpty());

    // intersection
    ASSERT_EQ(
      (fAABB2{ fvec2{0, 0}, fvec2{4, 4} } & fAABB2{ fvec2{2, 2}, fvec2{4, 4} }),
      (fAABB2{ fvec2{2, 2}, fvec2{2, 2} })
    );

    // intersection
    ASSERT_EQ(
      (fAABB2{ fvec2{2, 2}, fvec2{4, 4} } & fAABB2{ fvec2{0, 0}, fvec2{4, 4} }),
      (fAABB2{ fvec2{2, 2}, fvec2{2, 2} })
    );
  }

  TEST(chunklands__math, fAABB3) {
    // translate
    ASSERT_EQ(
      (fAABB3{ fvec3{-2,  0, -2}, fvec3{3, 4, 4} } + fvec3{1, -4, -3}),
      (fAABB3{ fvec3{-1, -4, -5}, fvec3{3, 4, 4} })
    );

    // movingbox
    ASSERT_EQ(
      (fAABB3{ fvec3{1, -1, -2}, fvec3{3, 5, 1}} | fvec3{3, 1, 1}),
      (fAABB3{ fvec3{1, -1, -2}, fvec3{6, 6, 2}})
    );

    // movingbox negative
    ASSERT_EQ(
      (fAABB3{ fvec3{ 1, -1, -2}, fvec3{3, 5, 1} } | fvec3{-3, -1, -1}),
      (fAABB3{ fvec3{-2, -2, -3}, fvec3{6, 6, 2} })
    );

    // movingbox zero
    ASSERT_EQ(
      (fAABB3{ fvec3{1, -1, -2}, fvec3{3, 5, 1} } | fvec3{0, 0, 0}),
      (fAABB3{ fvec3{1, -1, -2}, fvec3{3, 5, 1} })
    );

    // intersection empty
    ASSERT_TRUE((fAABB3{ fvec3{0, 0, 0}, fvec3{4, 4, 4} } & fAABB3{ fvec3{5, 2, 1}, fvec3{1, 1, 1} }).IsEmpty());

    // intersection
    ASSERT_EQ(
      (fAABB3{ fvec3{0, 0, 0}, fvec3{4, 4, 4} } & fAABB3{ fvec3{2, 2, 2}, fvec3{4, 4, 4} }),
      (fAABB3{ fvec3{2, 2, 2}, fvec3{2, 2, 2} })
    );

    // intersection
    ASSERT_EQ(
      (fAABB3{ fvec3{2, 2, 2}, fvec3{4, 4, 4} } & fAABB3{ fvec3{0, 0, 0}, fvec3{4, 4, 4} }),
      (fAABB3{ fvec3{2, 2, 2}, fvec3{2, 2, 2} })
    );
  }

  TEST(chunklands__math, chunk_pos_in_box) {
    chunk_pos_in_box c { fAABB3{ fvec3{-4.01f, -43.73f, 193.3f}, fvec3{52.7f, 23.3f, 3} }, 16};

    auto&& it = std::begin(c);
    auto&& end = std::end(c);

    for (int z = 12; z <= 12; z++) {
      for (int y = -3; y <= -2; y++) {
        for (int x = -1; x <= 3; x++) {
          ASSERT_NE(it, end);
          ivec3 actual = *it;
          ASSERT_EQ(actual, ivec3(x, y, z));

          ++it;
        }
      }
    }

    ASSERT_EQ(it, end);
  }

  TEST(chunklands__math, bound) {
    ASSERT_EQ((bound(fAABB3{ fvec3{  .2,   .5,   .7}, fvec3{ .4, 1.6,    .31} })), (iAABB3{ ivec3{ 0,  0,  0}, ivec3{1, 3, 2} }));
    ASSERT_EQ((bound(fAABB3{ fvec3{ -.2,  -.5,  -.7}, fvec3{ .8, 0.51,   .6 } })), (iAABB3{ ivec3{-1, -1, -1}, ivec3{2, 2, 1} }));
    ASSERT_EQ((bound(fAABB3{ fvec3{-0.9, -0.9, -0.9}, fvec3{1.8, 1.8  , 1.8 } })), (iAABB3{ ivec3{-1, -1, -1}, ivec3{2, 2, 2} }));
  }

  TEST(chunklands__math, block_pos_in_box) {

    {
      fAABB3 box{ fvec3{-0.9, -0.9, -0.9}, fvec3{1.8, 1.8, 1.8} };

      {
        block_pos_in_box b { box, ivec3{0, 0, 0}, 16};

        auto&& it = std::begin(b);
        auto&& end = std::end(b);

        ASSERT_NE(it, end);
        ASSERT_EQ(*it, ivec3(0, 0, 0));

        ++it;

        ASSERT_EQ(it, end);
      }

      {
        block_pos_in_box b { box, ivec3{-1, -1, -1}, 16};

        auto&& it = std::begin(b);
        auto&& end = std::end(b);

        ASSERT_NE(it, end);
        ASSERT_EQ(*it, ivec3(15, 15, 15));

        ++it;

        ASSERT_EQ(it, end);
      }

      {
        block_pos_in_box b { box, ivec3{-1, 0, 0}, 16};

        auto&& it = std::begin(b);
        auto&& end = std::end(b);

        ASSERT_NE(it, end);
        ASSERT_EQ(*it, ivec3(15, 0, 0));

        ++it;

        ASSERT_EQ(it, end);
      }

      {
        block_pos_in_box b { box, ivec3{0, -1, 0}, 16};

        auto&& it = std::begin(b);
        auto&& end = std::end(b);

        ASSERT_NE(it, end);
        ASSERT_EQ(*it, ivec3(0, 15, 0));

        ++it;

        ASSERT_EQ(it, end);
      }

      {
        block_pos_in_box b { box, ivec3{0, 0, -1}, 16};

        auto&& it = std::begin(b);
        auto&& end = std::end(b);

        ASSERT_NE(it, end);
        ASSERT_EQ(*it, ivec3(0, 0, 15));

        ++it;

        ASSERT_EQ(it, end);
      }

      {
        block_pos_in_box b { box, ivec3{-1, -1, 0}, 16};

        auto&& it = std::begin(b);
        auto&& end = std::end(b);

        ASSERT_NE(it, end);
        ASSERT_EQ(*it, ivec3(15, 15, 0));

        ++it;

        ASSERT_EQ(it, end);
      }

      {
        block_pos_in_box b { box, ivec3{-1, 0, -1}, 16};

        auto&& it = std::begin(b);
        auto&& end = std::end(b);

        ASSERT_NE(it, end);
        ASSERT_EQ(*it, ivec3(15, 0, 15));

        ++it;

        ASSERT_EQ(it, end);
      }

      {
        block_pos_in_box b { box, ivec3{0, -1, -1}, 16};

        auto&& it = std::begin(b);
        auto&& end = std::end(b);

        ASSERT_NE(it, end);
        ASSERT_EQ(*it, ivec3(0, 15, 15));

        ++it;

        ASSERT_EQ(it, end);
      }
    }

    {
      fAABB3 box{ fvec3{23.4f, 41.1f, -28.f}, fvec3{4.8f, 7.91f, 27.9f} };

      {
        block_pos_in_box b { box, ivec3{1, 2, -1}, 16};

        auto&& it = std::begin(b);
        auto&& end = std::end(b);

        for (int z = 0; z <= 15; z++) {
          for (int y = 9; y <= 15; y++) {
            for (int x = 7; x <= 12; x++) {
              ASSERT_NE(it, end);
              ivec3 actual = *it;
              ASSERT_EQ(actual, ivec3(x, y, z));

              ++it;
            }
          }
        }

        ASSERT_EQ(it, end);
      }
    }
  }

  TEST(chunklands__math, collision_time) {
    // 1D
    ASSERT_EQ(
      (collision(fAABB1{ fvec1{4}, fvec1{2} }, fvec1{2}, fAABB1{ fvec1{0}, fvec1{2} })),
      (collision_time<float>{})
    );

    ASSERT_EQ(
      (collision(fAABB1{ fvec1{0}, fvec1{2} }, fvec1{-2}, fAABB1{ fvec1{4}, fvec1{2} })),
      (collision_time<float>{})
    );

    ASSERT_EQ(
      (collision(fAABB1{ fvec1{4}, fvec1{2} }, fvec1{2}, fAABB1{ fvec1{2}, fvec1{2} })),
      (collision_time<float>{})
    );

    ASSERT_EQ(
      (collision(fAABB1{ fvec1{0}, fvec1{4} }, fvec1{-2}, fAABB1{ fvec1{4}, fvec1{2} })),
      (collision_time<float>{})
    );

    ASSERT_EQ(
      (collision(fAABB1{ fvec1{0}, fvec1{2} }, fvec1{2}, fAABB1{ fvec1{4}, fvec1{2} })),
      (collision_time<float>{ fvec1{1}, fvec1{2} })
    );

    ASSERT_EQ(
      (collision(fAABB1{ fvec1{0}, fvec1{2} }, fvec1{1}, fAABB1{ fvec1{1}, fvec1{2} })),
      (collision_time<float>{ fvec1{-1}, fvec1{4} })
    );

    ASSERT_EQ(
      (collision(fAABB1{ fvec1{4}, fvec1{2} }, fvec1{-2}, fAABB1{ fvec1{0}, fvec1{2} })),
      (collision_time<float>{ fvec1{1}, fvec1{2} })
    );

    ASSERT_EQ(
      (collision(fAABB1{ fvec1{1}, fvec1{2} }, fvec1{-1}, fAABB1{ fvec1{0}, fvec1{2} })),
      (collision_time<float>{ fvec1{-1}, fvec1{4} })
    );

    // special cases

    ASSERT_EQ(
      (collision(fAABB1{ fvec1{0}, fvec1{2} }, fvec1{0}, fAABB1{ fvec1{1}, fvec1{2} })),
      (collision_time<float>{
        fvec1{-std::numeric_limits<float>::infinity()},
        fvec1{std::numeric_limits<float>::infinity()}
      })
    );

    ASSERT_EQ(
      (collision(fAABB1{ fvec1{0}, fvec1{2} }, fvec1{0}, fAABB1{ fvec1{3}, fvec1{2} })),
      (collision_time<float>{})
    );

    ASSERT_EQ(
      (collision(fAABB1{ fvec1{0}, fvec1{2} }, fvec1{0}, fAABB1{ fvec1{2}, fvec1{2} })),
      (collision_time<float>{})
    );

    // 3D
    ASSERT_EQ(
      (collision_3d(fAABB3{ fvec3{0, 0, 0}, fvec3{1, 1, 1} }, fvec3{1, 2, -1}, fAABB3{ fvec3{-2, -2, -2}, fvec3{4, 4, 4} })),
      (axis_collision<float>{
        .axis = CollisionAxis::kY,
        .time = collision_time<float>{ fvec1{-1.5}, fvec1{2.5} }
      })
    );
  }
}
