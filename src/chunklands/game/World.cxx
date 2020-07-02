
#include "World.hxx"
#include <chunklands/debug.hxx>
#include <chunklands/jsmath.hxx>
#include <chunklands/misc/Profiler.hxx>
#include <iostream>

namespace chunklands::game {

  constexpr int RENDER_DISTANCE   = 12;
  constexpr int PREFETCH_DISTANCE = RENDER_DISTANCE + 2;
  constexpr int RETAIN_DISTANCE   = RENDER_DISTANCE + 4;

  static_assert(PREFETCH_DISTANCE >  RENDER_DISTANCE,   "prefetch distance has to be greater than render distance");
  static_assert(RETAIN_DISTANCE   >= PREFETCH_DISTANCE, "retain distance has to be greater or equal than prefetch distance");
  
  constexpr unsigned MAX_CHUNK_VIEW_UPDATES = 6;
  constexpr unsigned MAX_CHUNK_MODEL_GENERATES = 6;
  constexpr unsigned MAX_CHUNK_MODEL_PROCESSES = 6;

  JS_DEF_WRAP(World)

  void World::Prepare() {
    CHECK_GL();

    { // calculate nearest chunks
      nearest_chunks_.clear();
      // 4/3 * PI * r*r*r, for r = PREFETCH_DISTANCE + 0.5
      nearest_chunks_.reserve(5 * PREFETCH_DISTANCE * PREFETCH_DISTANCE * PREFETCH_DISTANCE);

      for (int cx = -PREFETCH_DISTANCE; cx <= PREFETCH_DISTANCE; cx++) {
        for (int cy = -PREFETCH_DISTANCE; cy <= PREFETCH_DISTANCE; cy++) {
          for (int cz = -PREFETCH_DISTANCE; cz <= PREFETCH_DISTANCE; cz++) {
            if (glm::length(glm::vec3(cx, cy, cz)) <= PREFETCH_DISTANCE) {
              nearest_chunks_.push_back(glm::ivec3(cx, cy, cz));
            }
          }
        }
      }

      std::sort_heap(nearest_chunks_.begin(), nearest_chunks_.end(), [](const glm::ivec3& a, const glm::ivec3& b) {
        glm::vec3 af(a);
        glm::vec3 bf(b);

        // more important to draw chunk of the same y level first
        af.y *= af.y;
        bf.y *= bf.y;

        return glm::length(af) < glm::length(bf);
      });
    }
  }


  void World::Update(double, const engine::Camera& camera) {
    DURATION_METRIC("world_update");

    // Calculation of the current chunk we are standing on:
    //   for n = ]-16;+16[, n / 16 = 0, but we want:
    //   - 1. for n = [0;+16[ => 0
    //   - 2. for n = ]-16;0[ => -1
    // thus for negative dimension values we need to subtract chunk size
    glm::ivec3 center_chunk_pos = chunklands::math::get_center_chunk(camera.GetPosition(), Chunk::SIZE);

    {
      DURATION_METRIC("world_update_retain");
      // TODO(daaitch): make retain parallel lazy
      // map cleanup: remove chunks outside prefetch distance
      for (auto&& it = chunk_map_.begin(); it != chunk_map_.end(); ) {
        auto&& pos = it->first;

        // check chunk inside retain distance
        if (glm::length(glm::vec3(pos - center_chunk_pos)) <= RETAIN_DISTANCE) {
          it++; // goto next chunk
        } else {
          // remove chunk
          it = chunk_map_.erase(it); // next it
        }
      }
    }

    unsigned chunk_view_updates = 0;
    unsigned chunk_model_updates = 0;

    {
      DURATION_METRIC("world_update_chunks");
      // TODO(daaitch): currently taking 8ms when settled => create update queues
      // map update: insert/update chunks
      for (auto&& nearest_chunk_it = nearest_chunks_.cbegin(); nearest_chunk_it != nearest_chunks_.cend(); nearest_chunk_it++) {
        glm::ivec3 pos(*nearest_chunk_it + center_chunk_pos);

        // find or create chunk
        auto&& chunk_it = chunk_map_.find(pos);
        if (chunk_it == chunk_map_.end()) {
          auto&& insert_it = chunk_map_.insert(std::make_pair(pos,
                                                              std::make_shared<Chunk>(pos)));
          chunk_it = insert_it.first;
        }

        assert(chunk_it != chunk_map_.end());
        assert(chunk_it->first == pos);
        auto&& chunk = chunk_it->second;

        // we will call `Generate*` in reverse order to not update too much in one iteration

        // 1. GenerateView
        if ( // check chunk inside render distance
          chunk_view_updates < MAX_CHUNK_VIEW_UPDATES &&
          chunk->GetState() == kModelPrepared
        ) {
          do { // to call "break;"
            auto&& left_chunk_it = chunk_map_.find(glm::ivec3(pos.x-1, pos.y, pos.z));
            if (left_chunk_it == chunk_map_.end() ||
                left_chunk_it->second->GetState() < kModelPrepared) { break; }
            auto&& right_chunk_it = chunk_map_.find(glm::ivec3(pos.x+1, pos.y, pos.z));
            if (right_chunk_it == chunk_map_.end() ||
                right_chunk_it->second->GetState() < kModelPrepared) { break; }
            auto&& top_chunk_it = chunk_map_.find(glm::ivec3(pos.x, pos.y+1, pos.z));
            if (top_chunk_it == chunk_map_.end() ||
                top_chunk_it->second->GetState() < kModelPrepared) { break; }
            auto&& bottom_chunk_it = chunk_map_.find(glm::ivec3(pos.x, pos.y-1, pos.z));
            if (bottom_chunk_it == chunk_map_.end() ||
                bottom_chunk_it->second->GetState() < kModelPrepared) { break; }
            auto&& front_chunk_it = chunk_map_.find(glm::ivec3(pos.x, pos.y, pos.z-1));
            if (front_chunk_it == chunk_map_.end() ||
                front_chunk_it->second->GetState() < kModelPrepared) { break; }
            auto&& back_chunk_it = chunk_map_.find(glm::ivec3(pos.x, pos.y, pos.z+1));
            if (back_chunk_it == chunk_map_.end() ||
                back_chunk_it->second->GetState() < kModelPrepared) { break; }
            
            const Chunk* neighbors[kNeighborCount] = {
              &*left_chunk_it->second,
              &*right_chunk_it->second,
              &*top_chunk_it->second,
              &*bottom_chunk_it->second,
              &*front_chunk_it->second,
              &*back_chunk_it->second
            };
            
            chunk_view_updates++;
            js_ChunkGenerator->GenerateView(*chunk, neighbors);
          } while (0);
        }

        // 2. GenerateModel
        // always inside prefetch distance
        if (chunk_model_updates < MAX_CHUNK_MODEL_GENERATES && chunk->GetState() == kEmpty) {
          chunk_model_updates++;
          js_ChunkGenerator->GenerateModel(chunk);
        }

      }
    }

    {
      DURATION_METRIC("world_update_process_models");
      for (unsigned i = 0; i < MAX_CHUNK_MODEL_PROCESSES; i++) {
        if (!js_ChunkGenerator->ProcessNextModel()) {
          break;
        }
      }
    }
  }

  void World::Render(double, const engine::Camera& camera) {
    DURATION_METRIC("world_render");
    CHECK_GL();

    glm::ivec3 center_chunk_pos = chunklands::math::get_center_chunk(camera.GetPosition(), Chunk::SIZE);

    // map render all chunks
    unsigned rendered_index_count = 0;
    unsigned rendered_chunk_count = 0;
    for (auto&& chunk_entry : chunk_map_) {
      auto&& chunk = chunk_entry.second;

      if (chunk->GetState() != kViewPrepared) {
        continue; // no view data
      }

      if (glm::length(glm::vec3(chunk->GetPos() - center_chunk_pos)) > RENDER_DISTANCE) {
        continue;
      }

      rendered_index_count += chunk->GetIndexCount();
      rendered_chunk_count++;
      chunk->Render();
    }

    misc::Profiler::SetGauge("rendered_chunk_count", rendered_chunk_count);
    misc::Profiler::SetGauge("rendered_index_count", rendered_index_count);
  }

  int World::GetRenderDistance() const {
    return RENDER_DISTANCE;
  }

  engine::collision_result World::ProcessNextCollision(const math::fAABB3 &box, const math::fvec3 &movement) {
    math::fAABB3 movement_box {box | movement};

    engine::collision_result result {
      .prio = std::numeric_limits<int>::max(),
      .axis = math::CollisionAxis::kNone,
      .ctime = std::numeric_limits<float>::max(),
      .collisionfree_movement = movement,
      .outstanding_movement = math::fvec3(0.f, 0.f, 0.f)
    };

    int collision_index = 0;

    for (auto&& chunk_pos : math::chunk_pos_in_box {movement_box, Chunk::SIZE}) {
      const auto&& chunk_result = chunk_map_.find(chunk_pos);
      if (chunk_result == chunk_map_.cend()) {
        // TODO(daaitch): make collision with chunk
        std::cout << "not loaded: " << chunk_pos << std::endl;
        continue;
      }

      auto&& chunk = chunk_result->second;
      math::ivec3 chunk_coord{ chunk_pos * (int)Chunk::SIZE };

      for (auto&& block_pos : math::block_pos_in_box {movement_box, chunk_pos, Chunk::SIZE}) {
        auto&& block_def = chunk->BlockAt(block_pos);
        assert(block_def != nullptr);

        math::ivec3 block_coord{ chunk_coord + block_pos };
        
        if (DEBUG_COLLISION) {
          std::cout << "collision #" << collision_index << std::endl;
        }

        auto&& block_collision = block_def->ProcessCollision(block_coord, box, movement);
        if (block_collision.ctime < result.ctime
          || (block_collision.ctime == result.ctime && block_collision.prio < result.prio)
        ) {
          result = block_collision;
        }

        ++collision_index;
      }
    }

    return result;
  }

  std::optional<math::ivec3> World::FindPointingBlock(const math::fLine3& look) {
    // math::fAABB3 box {look.origin - math::fvec3(.1f, .1f, .1f), math::fvec3(.2f, .2f, .2f)};
    math::fAABB3 box {look.origin, math::fvec3(0.f, 0.f, 0.f)};
    math::fAABB3 movement_box {box | look.span};

    float ctime = std::numeric_limits<float>::infinity();
    math::ivec3 result;

    int collision_index = 0;

    for (auto&& chunk_pos : math::chunk_pos_in_box {movement_box, Chunk::SIZE}) {
      const auto&& chunk_result = chunk_map_.find(chunk_pos);
      if (chunk_result == chunk_map_.cend()) {
        // TODO(daaitch): make collision with chunk
        std::cout << "not loaded: " << chunk_pos << std::endl;
        continue;
      }

      auto&& chunk = chunk_result->second;
      math::ivec3 chunk_coord{ chunk_pos * (int)Chunk::SIZE };

      for (auto&& block_pos : math::block_pos_in_box {movement_box, chunk_pos, Chunk::SIZE}) {
        auto&& block_def = chunk->BlockAt(block_pos);
        assert(block_def != nullptr);

        math::ivec3 block_coord{ chunk_coord + block_pos };

        if (DEBUG_COLLISION) {
          std::cout << "collision #" << collision_index << std::endl;
        }

        auto&& block_collision = block_def->ProcessCollision(block_coord, box, look.span);
        if (block_collision.ctime < ctime) {
          ctime = block_collision.ctime;
          result = block_coord;
        }

        ++collision_index;
      }
    }

    if (ctime >= 1.f) {
      return {};
    }

    return {result};
  }

  const BlockDefinition* World::GetBlock(const glm::ivec3& coord) const {
    auto&& it = chunk_map_.find(math::get_center_chunk(coord, Chunk::SIZE));
    if (it == chunk_map_.cend()) {
      return nullptr;
    }

    return it->second->BlockAt(math::get_pos_in_chunk(coord, Chunk::SIZE));
  }

  JSValue World::JSCall_findPointingBlock(JSCbi info) {
    auto&& pos = jsmath::vec3<float>(info[0]);
    auto&& look = jsmath::vec2<float>(info[1]);

    glm::vec3 look_center(-sinf(look.x) * cosf(look.y),
                           sinf(look.y),
                          -cosf(look.x) * cosf(look.y));

    std::cout << "pos: " << pos << ", look: " << look << ", look_center: " << look_center << std::endl;

    auto&& result = FindPointingBlock(math::fLine3::from_range(pos, pos + look_center * 2.f));
    if (!result) {
      return info.Env().Null();
    }

    return jsmath::vec3(info.Env(), result.value());
  }

  void World::JSCall_replaceBlock(JSCbi info) {
    auto&& coord = jsmath::vec3<int>(info[0]);
    int block_id = info[1].ToNumber().Int32Value();
    auto&& chunk_pos = math::get_center_chunk(coord, Chunk::SIZE);
    auto&& chunk_result = chunk_map_.find(chunk_pos);
    if (chunk_result == chunk_map_.end()) {
      // TODO(daaitch): public "warn", "error", "info" logger API in C++
      std::cerr << "WARN: cannot find chunk for position" << std::endl;
      return;
    }

    auto&& block_def = js_BlockRegistrar->GetByIndex(block_id);
    
    auto&& chunk = chunk_result->second;
    auto&& pos_in_chunk = math::get_pos_in_chunk(coord, Chunk::SIZE);
    chunk->UpdateBlock(pos_in_chunk, block_def);
  }

} // namespace chunklands::game