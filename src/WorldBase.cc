#include "WorldBase.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vector_relational.hpp>

#include "log.h"

namespace chunklands {
  DEFINE_OBJECT_WRAP(WorldBase, ONE_ARG({
    InstanceMethod("setChunkGenerator", &WorldBase::SetChunkGenerator)
  }))

  WorldBase::WorldBase(const Napi::CallbackInfo& info) : ObjectWrap<WorldBase>(info) {
    Napi::HandleScope scope(info.Env());
    
    if (!info[0].IsObject()) {
      Napi::Error::New(Env(), "expect object {vertexShader: ..., fragmentShader: ...}").ThrowAsJavaScriptException();
      return;
    }

    Napi::Object settings = info[0].ToObject();
    vsh_src_ = settings.Get("vertexShader").ToString();
    fsh_src_ = settings.Get("fragmentShader").ToString();
  }

  void WorldBase::SetChunkGenerator(const Napi::CallbackInfo& info) {
    chunk_generator_ = info[0].ToObject();
  }

  void WorldBase::Prepare() {
    { // vertex shader (duplicated code)
      vsh_ = glCreateShader(GL_VERTEX_SHADER);
      const char* src = vsh_src_.c_str();
      glShaderSource(vsh_, 1, &src, nullptr);
      glCompileShader(vsh_);
      GLint result = GL_FALSE;
      glGetShaderiv(vsh_, GL_COMPILE_STATUS, &result);
      if (result != GL_TRUE) {
        GLint length = 0;
        glGetShaderiv(vsh_, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> message(length + 1);
        glGetShaderInfoLog(vsh_, length, nullptr, message.data());
        Napi::Error::New(Env(), message.data()).ThrowAsJavaScriptException();
        return;
      }

      CHECK_GL();
    }

    { // fragment shader (duplicated code)
      fsh_ = glCreateShader(GL_FRAGMENT_SHADER);
      const char* src = fsh_src_.c_str();
      glShaderSource(fsh_, 1, &src, nullptr);
      glCompileShader(fsh_);
      GLint result = GL_FALSE;
      glGetShaderiv(fsh_, GL_COMPILE_STATUS, &result);
      if (result != GL_TRUE) {
        GLint length = 0;
        glGetShaderiv(fsh_, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> message(length + 1);
        glGetShaderInfoLog(fsh_, length, nullptr, message.data());
        Napi::Error::New(Env(), message.data()).ThrowAsJavaScriptException();
        return;
      }

      CHECK_GL();
    }

    { // program
      program_ = glCreateProgram();
      glAttachShader(program_, vsh_);
      glAttachShader(program_, fsh_);
      glLinkProgram(program_);

      GLint result = GL_FALSE;
      glGetProgramiv(program_, GL_LINK_STATUS, &result);
      if (result != GL_TRUE) {
        GLint length = 0;
        glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> message(length + 1);
        glGetProgramInfoLog(program_, length, nullptr, message.data());
        Napi::Error::New(Env(), message.data()).ThrowAsJavaScriptException();
        return;
      }

      CHECK_GL();
    }

    { // matrices
      view_uniform_location_ = glGetUniformLocation(program_, "u_view");
      proj_uniform_location_ = glGetUniformLocation(program_, "u_proj");
    }

    { // texture
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      texture_location_ = glGetUniformLocation(program_, "u_texture");
    }

    { // general
      glEnable(GL_DEPTH_TEST);
    }
  }

  constexpr int RENDER_DISTANCE   = 2;
  constexpr int PREFETCH_DISTANCE = 3;
  static_assert(PREFETCH_DISTANCE > RENDER_DISTANCE, "PREFETCH_DISTANCE must be bigger than RENDER_DISTANCE");

  void WorldBase::Update(double diff) {

    // Calculation of the current chunk we are standing on:
    //   for n = ]-16;+16[, n / 16 = 0, but we want:
    //   - 1. for n = [0;+16[ => 0
    //   - 2. for n = ]-16;0[ => -1
    // thus for negative dimension values we need to subtract chunk size
    glm::ivec3 center_chunk_pos = glm::ivec3(pos_.x >= 0 ? pos_.x : pos_.x - Chunk::SIZE,
                                             pos_.y >= 0 ? pos_.y : pos_.y - Chunk::SIZE,
                                             pos_.z >= 0 ? pos_.z : pos_.z - Chunk::SIZE
                                            ) / (int)Chunk::SIZE;

    glm::ivec3 render_distance(RENDER_DISTANCE, RENDER_DISTANCE, RENDER_DISTANCE);
    glm::ivec3 render_from = center_chunk_pos - render_distance;
    glm::ivec3 render_to   = center_chunk_pos + render_distance;

    glm::ivec3 prefetch_distance(PREFETCH_DISTANCE, PREFETCH_DISTANCE, PREFETCH_DISTANCE);
    glm::ivec3 prefetch_from = center_chunk_pos - prefetch_distance;
    glm::ivec3 prefetch_to   = center_chunk_pos + prefetch_distance;

    // map cleanup: remove chunks outside prefetch distance
    for (auto&& it = chunk_map_.begin(); it != chunk_map_.end(); ) {
      auto&& pos = it->first;

      if ( // check chunk inside prefetch distance
        glm::all(glm::lessThanEqual(prefetch_from, pos)) &&
        glm::all(glm::lessThanEqual(pos, prefetch_to))
      ) {
        it++; // goto next chunk
      } else {
        // remove chunk
        it = chunk_map_.erase(it);
      }
    }

    // map update: insert/update chunks
    for (int cx = prefetch_from.x; cx <= prefetch_to.x; cx++) {
      for (int cy = prefetch_from.y; cy <= prefetch_to.y; cy++) {
        for (int cz = prefetch_from.z; cz <= prefetch_to.z; cz++) {
          glm::ivec3 pos(cx, cy, cz);

          // find or create chunk
          auto&& it = chunk_map_.find(pos);
          if (it == chunk_map_.end()) {
            auto&& insert_it = chunk_map_.insert(std::make_pair(pos,
                                                                std::make_shared<Chunk>(pos)));
            it = insert_it.first;
          }

          assert(it != chunk_map_.end());
          assert(it->first == pos);
          auto&& chunk = it->second;

          // we will call `Generate*` in reverse order to not update too much in one iteration

          // 1. GenerateView
          if ( // check chunk inside render distance
            chunk->GetState() == kModelPrepared &&
            glm::all(glm::lessThanEqual(render_from, pos)) &&
            glm::all(glm::lessThanEqual(pos, render_to))
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
              
              chunk_generator_->GenerateView(*chunk, neighbors);
            } while (0);
          }

          // 2. GenerateModel
          // always inside prefetch distance
          if (chunk->GetState() == kEmpty) {
            chunk_generator_->GenerateModel(*chunk);
          }

        }
      }
    }
    
    
    glm::vec3 look_center(-sinf(look_.x) * cosf(look_.y),
                          sinf(look_.y),
                          -cosf(look_.x) * cosf(look_.y));
    view_ = glm::lookAt(pos_, pos_ + look_center, glm::vec3(0.f, 1.f, 0.f));
  }

  void WorldBase::Render(double diff) {
    glUseProgram(program_);
    glUniformMatrix4fv(view_uniform_location_, 1, GL_FALSE, glm::value_ptr(view_));
    glUniformMatrix4fv(proj_uniform_location_, 1, GL_FALSE, glm::value_ptr(proj_));
    glUniform1i(texture_location_, 0);

    chunk_generator_->BindTexture();

    CHECK_GL();

    // map render all chunks
    unsigned rendered_index_count = 0;
    for (auto&& chunk_entry : chunk_map_) {
      auto&& chunk = chunk_entry.second;

      if (chunk->GetState() != kViewPrepared) {
        continue; // no view data
      }

      rendered_index_count += chunk->GetIndexCount();
      chunk->Render();
    }

    std::cout << "Rendered index count: " << rendered_index_count << std::endl;

    glUseProgram(0);
  }

  void WorldBase::UpdateViewportRatio(int width, int height) {
    constexpr float fovy_degree = 75.f;
    
    proj_ = glm::perspective(glm::radians(fovy_degree), (float)width / height, 0.1f, 100.0f);
    CHECK_GL();
  }

  void WorldBase::AddLook(float yaw_rad, float pitch_rad) {
    constexpr float pitch_break = (.5f * M_PI) - .1;
    
    look_.x += yaw_rad;
    look_.x = fmodf(look_.x, 2.f * M_PI);

    look_.y += pitch_rad;
    look_.y = std::max(std::min(look_.y, pitch_break), -pitch_break);
  }

  void WorldBase::AddPos(const glm::vec3& v) {
    pos_ += v;
  }
}
