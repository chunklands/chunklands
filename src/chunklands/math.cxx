#include "math.hxx"

namespace chunklands::math {
  std::size_t ivec3_hasher::operator()(const ivec3& v) const {
    std::size_t seed = 0;
    boost::hash_combine(seed, boost::hash_value(v.x));
    boost::hash_combine(seed, boost::hash_value(v.y));
    boost::hash_combine(seed, boost::hash_value(v.z));

    return seed;
  }

  box_iterator& box_iterator::operator++() {
    assert(current_.z <= b_.z);

    current_.x++;
    if (current_.x > b_.x) {
      current_.x = a_.x;
      current_.y++;
      if (current_.y > b_.y) {
        current_.y = a_.y;
        current_.z++;
      }
    }

    return *this;
  }

  box_iterator box_iterator::end(const ivec3& min, const ivec3& max) {
    ivec3 end{min.x, min.y, max.z + 1};
    return box_iterator{end, end};
  }

  chunk_pos_in_box::chunk_pos_in_box(const fAABB3& box, unsigned chunk_size) {
    chunk_min_ = __WITH_BUG_get_center_chunk(box.origin           , chunk_size);
    chunk_max_ = __WITH_BUG_get_center_chunk(box.origin + box.span, chunk_size);
    end_       = box_iterator::end(chunk_min_, chunk_max_);
  }

  block_pos_in_box::block_pos_in_box(const fAABB3& box, const ivec3& chunk_pos, unsigned chunk_size) {
    iAABB3 bound_box = bound(box);
    iAABB3 chunk_box{
      chunk_pos * (int)chunk_size,
      ivec3{chunk_size, chunk_size, chunk_size}
    };

    iAABB3 intersection = bound_box & chunk_box;
    if (!intersection) {
      empty_ = true;
      block_min_ = ivec3{0, 0, 0};
      block_max_ = ivec3{0, 0, 0};
    } else {
      empty_ = false;
      block_min_ = intersection.origin - chunk_box.origin;
      block_max_ = block_min_ + intersection.span - ivec3(1, 1, 1);
    }

    end_ = box_iterator::end(block_min_, block_max_);
  }

  std::ostream& operator<<(std::ostream& os, const box_iterator& it) {
    return os << "box_iterator{ a=" << it.a_ << ", b=" << it.b_ << ", current=" << it.current_ << " }";
  }
}