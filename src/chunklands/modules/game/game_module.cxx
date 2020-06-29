#include "game_module.hxx"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/vector_relational.hpp>
#include <glm/geometric.hpp>

#include <cmath>
#include <math.h>
#include <chunklands/modules/misc/misc_module.hxx>
#include <iostream>
#include <chunklands/math.hxx>
#include <chunklands/jsmath.hxx>

namespace chunklands::modules::game {

  /////////////////////////////////////////////////////////////////////////////
  // BlockDefinition //////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  BlockDefinition::BlockDefinition(std::string id, bool opaque, std::unordered_map<std::string, std::vector<GLfloat>> faces_vertex_data)
    : id_(std::move(id)), opaque_(opaque), faces_vertex_data_(std::move(faces_vertex_data)) {
  }

  engine::collision_result BlockDefinition::ProcessCollision(const math::ivec3& block_coord, const math::fAABB3& box, const math::fvec3& movement) const {
    if (!opaque_) {
      return {
        .prio = 0,
        .axis = math::CollisionAxis::kNone,
        .ctime = 1,
        .collisionfree_movement{movement},
        .outstanding_movement{0,0,0}
      };
    }

    math::fAABB3 block_box{ block_coord, math::fvec3{1, 1, 1} };
    auto&& collision = math::collision_3d(box, movement, block_box);

    if (!collision.time) {
      return {
        .prio = 0,
        .axis = math::CollisionAxis::kNone,
        .ctime = 1,
        .collisionfree_movement{movement},
        .outstanding_movement{0,0,0}
      };
    }
    
    if (collision.time.origin.x < 0) {
      return {
        .prio = 0,
        .axis = collision.axis,
        .ctime = 0,
        .collisionfree_movement{0, 0, 0},
        .outstanding_movement{0, 0, 0}
      };
    }

    if (collision.time.origin.x > 1) {
      return {
        .prio = 0,
        .axis = collision.axis,
        .ctime = 1,
        .collisionfree_movement{movement},
        .outstanding_movement{0,0,0}
      };
    }

    if (DEBUG_COLLISION) {
      std::cout << collision << std::endl;
    }

    float ctime = collision.time.origin.x;
    math::fvec3 good_movement = ctime * movement;
    math::fvec3 bad_movement = movement - good_movement;

    int prio = 0;

    if (collision.axis & math::CollisionAxis::kX) {
      bad_movement.x = 0;
      prio++;
    }

    if (collision.axis & math::CollisionAxis::kY) {
      bad_movement.y = 0;
      prio++;
    }

    if (collision.axis & math::CollisionAxis::kZ) {
      bad_movement.z = 0;
      prio++;
    }

    return {
      .prio = prio,
      .axis = collision.axis,
      .ctime = ctime,
      .collisionfree_movement{good_movement},
      .outstanding_movement{bad_movement}
    };
  }


  /////////////////////////////////////////////////////////////////////////////
  // BlockRegistrar ///////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  JS_DEF_WRAP(BlockRegistrar)

  JSValue BlockRegistrar::JSCall_addBlock(JSCbi info) {
    JS_ASSERT(info.Env(), info[0].IsObject());

    auto&& js_block_definition = info[0].ToObject();

    auto&& js_id = js_block_definition.Get("id");
    JS_ASSERT(info.Env(), js_id.IsString());
    std::string id = js_id.ToString();

    auto&& js_opaque = js_block_definition.Get("opaque");
    JS_ASSERT(info.Env(), js_opaque.IsBoolean());
    bool opaque = js_opaque.ToBoolean();

    auto&& js_faces = js_block_definition.Get("faces");
    JS_ASSERT(info.Env(), js_faces.IsObject());

    auto&& js_faces_obj = js_faces.As<JSObject>();
    auto&& js_facenames_arr = js_faces_obj.GetPropertyNames();
    std::unordered_map<std::string, std::vector<GLfloat>> faces;

    for (unsigned i = 0; i < js_facenames_arr.Length(); i++) {
      auto&& js_facename = js_facenames_arr.Get(i);
      assert(js_facename.IsString());
      
      auto&& js_facename_str = js_facename.ToString();
      auto&& js_face = js_faces_obj.Get(js_facename_str);
      JS_ASSERT(info.Env(), js_face.IsArray());
      auto&& js_face_arr = js_face.As<JSArray>();

      std::vector<GLfloat> face;
      face.reserve(js_face_arr.Length());

      for(unsigned i = 0; i < js_face_arr.Length(); i++) {
        auto&& js_vertex_value = js_face_arr.Get(i);
        JS_ASSERT(info.Env(), js_vertex_value.IsNumber());

        face.push_back(js_vertex_value.ToNumber().FloatValue());
      }

      auto&& facename = js_facename_str.Utf8Value();
      faces.insert(std::make_pair(facename, std::move(face)));
    }

    auto&& block_definition = std::make_unique<BlockDefinition>(id,
                                                                opaque,
                                                                std::move(faces));

    block_definitions_.push_back(std::move(block_definition));
    return JSNumber::New(info.Env(), block_definitions_.size() - 1);
  }

  void BlockRegistrar::JSCall_loadTexture(JSCbi info) {
    JS_ASSERT(info.Env(), info[0].IsString());

    std::string filepath = info[0].ToString();
    texture_.LoadTexture(filepath.c_str());
  }

  BlockDefinition* BlockRegistrar::GetByIndex(int index) {
    return block_definitions_[index].get();
  }

  const BlockDefinition* BlockRegistrar::GetByIndex(int index) const {
    return block_definitions_[index].get();
  }

  void BlockRegistrar::BindTexture() {
    texture_.ActiveAndBind(GL_TEXTURE0);
  }



  /////////////////////////////////////////////////////////////////////////////
  // SpriteRegistrar //////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  JS_DEF_WRAP(SpriteRegistrar)

  void SpriteRegistrar::JSCall_addSprite(JSCbi info) {
    auto&& sprite = std::make_unique<sprite_definition>();

    auto&& js_model = info[0].ToObject();
    sprite->id = js_model.Get("id").ToString();

    auto&& js_faces = js_model.Get("faces").ToObject();
    auto&& js_keys = js_faces.GetPropertyNames();

    for (uint32_t i = 0; i < js_keys.Length(); i++) {
      auto&& js_key = js_keys.Get(i);
      auto&& js_face = js_faces.Get(js_key).As<JSArray>();
      std::vector<GLfloat> face;
      face.reserve(js_face.Length());

      for (uint32_t i = 0; i < js_face.Length(); i++) {
        face.push_back(js_face.Get(i).ToNumber());
      }

      sprite->faces.insert(std::make_pair(js_key.ToString(), std::move(face)));
    }

    sprites_.insert(std::make_pair(sprite->id, std::move(sprite)));
  }

  const sprite_definition* SpriteRegistrar::GetSprite(const std::string& id) const {
    auto&& it = sprites_.find(id);
    if (it == sprites_.end()) {
      return nullptr;
    }

    return it->second.get();
  }



  /////////////////////////////////////////////////////////////////////////////
  // Chunk ////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  Chunk::Chunk(glm::ivec3 pos) : pos_(std::move(pos)) {
  }

  Chunk::~Chunk() {
    Cleanup();
  }

  void Chunk::InitializeGL() {
    CC_ASSERT(state_ != kViewPrepared);

    Cleanup();

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vb_);
  }

  void Chunk::Cleanup() {
    if (vb_) {
      glDeleteBuffers(1, &vb_);
      vb_ = 0;
    }

    if (vao_) {
      glDeleteVertexArrays(1, &vao_);
      vao_ = 0;
    }

    // blocks will not be reset, simply overwrite them if chunks should be reused
    if (state_ == kViewPrepared) {
      state_ = kModelPrepared;
    }
  }

  void Chunk::Render() {
    CC_ASSERT(state_ == kViewPrepared);
    CHECK_GL();

    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, vb_index_count_);
  }



  /////////////////////////////////////////////////////////////////////////////
  // ChunkGenerator ///////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  JS_DEF_WRAP(ChunkGenerator)

  struct generate_model_callback_data {
    JSWrapRef<ChunkGenerator> js_generator;
    std::shared_ptr<Chunk> chunk;
    misc::prof profiler;
  };

  void ChunkGenerator::GenerateModel(std::shared_ptr<Chunk>& chunk) {
    DURATION_METRIC("chunkgenerator_generate_model");

    assert(chunk->GetState() == kEmpty);

    auto&& pos = chunk->GetPos();
    {
      JSEnv env = js_WorldGenerator.Env();
      JSHandleScope scope(env);

      chunk->state_ = kModelIsPreparing;

      std::unique_ptr<generate_model_callback_data> data(new generate_model_callback_data{
        .js_generator {*this},
        .chunk = chunk,
        .profiler = PROF_MOVE()
      });

      auto&& js_callback = JSFunction::New(
        env, [](JSCbi info) {
          auto&& data = reinterpret_cast<generate_model_callback_data *>(info.Data());
          
          auto&& js_err = info[0];
          auto&& js_chunk = info[1];

          if (!js_err.IsUndefined() && !js_err.IsNull()) {
            throw js_err.As<JSError>();
            return;
          }

          assert(js_chunk.IsTypedArray());
          auto&& js_chunk_arr = js_chunk.As<JSInt32Array>();
          assert(js_chunk_arr.ElementLength() == Chunk::BLOCK_COUNT);
          data->js_generator->loaded_chunks_.push({
            .chunk = data->chunk,
            .js_chunk_data = Napi::Persistent(js_chunk_arr)
          });
        },
        "generateChunkCallback",
        data.get()
      );

      js_callback.AddFinalizer([](JSEnv, generate_model_callback_data *data) {
        delete data;
      }, data.release());
      
      js_WorldGenerator.Get("generateChunk").As<JSFunction>().Call(js_WorldGenerator.Value(), {
        JSNumber::New(env, pos.x),
        JSNumber::New(env, pos.y),
        JSNumber::New(env, pos.z),
        JSNumber::New(env, Chunk::SIZE),
        js_callback
      });
    }
  }

  bool ChunkGenerator::ProcessNextModel() {
    if (loaded_chunks_.empty()) {
      return false;
    }

    DURATION_METRIC("chunkgenerator_process_next_model");

    auto&& item = loaded_chunks_.front();
    auto&& data = item.js_chunk_data.Value().Data();

    int i = 0;
    item.chunk->ForEachBlock([&](Chunk::BlockType& block_type, int, int, int) {
      block_type = js_BlockRegistrar->GetByIndex(data[i]);
      i++;
    });

    item.chunk->state_ = kModelPrepared;
    loaded_chunks_.pop();

    VALUE_METRIC("chunkgenerator_models_queue", loaded_chunks_.size());

    return !loaded_chunks_.empty();
  }

  // for now just an estimation: allocation vs. growing vector
  // faces * triangle_per_face * vertices_per_triangle * floats_per_vertex * 0.5 load
  constexpr int estimated_avg_floats_per_block = (6 * 2 * 3 * 8) / 2;
  constexpr int estimated_floats_in_buffer = Chunk::SIZE * Chunk::SIZE * Chunk::SIZE * estimated_avg_floats_per_block;

  void ChunkGenerator::GenerateView(Chunk& chunk, const Chunk* neighbors[kNeighborCount]) {
    DURATION_METRIC("chunkgenerator_generate_view");

    assert(chunk.GetState() == kModelPrepared);
    assert(neighbors[kLeft  ] != nullptr && neighbors[kLeft  ]->GetState() >= kModelPrepared);
    assert(neighbors[kRight ] != nullptr && neighbors[kRight ]->GetState() >= kModelPrepared);
    assert(neighbors[kTop   ] != nullptr && neighbors[kTop   ]->GetState() >= kModelPrepared);
    assert(neighbors[kBottom] != nullptr && neighbors[kBottom]->GetState() >= kModelPrepared);
    assert(neighbors[kFront ] != nullptr && neighbors[kFront ]->GetState() >= kModelPrepared);
    assert(neighbors[kBack  ] != nullptr && neighbors[kBack  ]->GetState() >= kModelPrepared);

    // allocate client side vertex buffer
    std::vector<GLfloat> vertex_buffer_data;
    vertex_buffer_data.reserve(estimated_floats_in_buffer);

    glm::vec3 chunk_offset(glm::vec3(chunk.GetPos()) * (float)Chunk::SIZE);

    chunk.ForEachBlock([&](const Chunk::BlockType& block_type, unsigned x, unsigned y, unsigned z) {
      assert(block_type != nullptr);

      //                                                              << inside of chunk || check neighbor >>
      bool isLeftOpaque   = ((x >= 1            && chunk.blocks_[z][y][x-1]->IsOpaque()) || (x == 0             && neighbors[kLeft  ]->blocks_[z][y][Chunk::SIZE-1]->IsOpaque()));
      bool isRightOpaque  = ((x < Chunk::SIZE-1 && chunk.blocks_[z][y][x+1]->IsOpaque()) || (x == Chunk::SIZE-1 && neighbors[kRight ]->blocks_[z][y][0]            ->IsOpaque()));
      bool isBottomOpaque = ((y >= 1            && chunk.blocks_[z][y-1][x]->IsOpaque()) || (y == 0             && neighbors[kBottom]->blocks_[z][Chunk::SIZE-1][x]->IsOpaque()));
      bool isTopOpaque    = ((y < Chunk::SIZE-1 && chunk.blocks_[z][y+1][x]->IsOpaque()) || (y == Chunk::SIZE-1 && neighbors[kTop   ]->blocks_[z][0][x]            ->IsOpaque()));
      bool isFrontOpaque  = ((z >= 1            && chunk.blocks_[z-1][y][x]->IsOpaque()) || (z == 0             && neighbors[kFront ]->blocks_[Chunk::SIZE-1][y][x]->IsOpaque()));
      bool isBackOpaque   = ((z < Chunk::SIZE-1 && chunk.blocks_[z+1][y][x]->IsOpaque()) || (z == Chunk::SIZE-1 && neighbors[kBack  ]->blocks_[0][y][x]            ->IsOpaque()));

      if (isLeftOpaque && isRightOpaque && isBottomOpaque && isTopOpaque && isFrontOpaque && isBackOpaque) {
        // optimization: block is fully hidden
        return;
      }

      auto&& vertex_data = block_type->GetFacesVertexData();
      for (auto &&it = vertex_data.cbegin(); it != vertex_data.cend(); it++) {
        if (
          (it->first == "left"   && isLeftOpaque  ) ||
          (it->first == "right"  && isRightOpaque ) ||
          (it->first == "top"    && isTopOpaque   ) ||
          (it->first == "bottom" && isBottomOpaque) ||
          (it->first == "front"  && isFrontOpaque ) ||
          (it->first == "back"   && isBackOpaque )
        ) {
          // optimization: face is hidden
          continue;
        }

        auto&& vertex_data = it->second;
        assert(vertex_data.size() % 8 == 0);
        for (unsigned fi = 0; fi < vertex_data.size(); ) { // float index

          // position vertices
          vertex_buffer_data.push_back(vertex_data[fi++] + (GLfloat)x + chunk_offset.x);
          vertex_buffer_data.push_back(vertex_data[fi++] + (GLfloat)y + chunk_offset.y);
          vertex_buffer_data.push_back(vertex_data[fi++] + (GLfloat)z + chunk_offset.z);

          // normal vertices
          vertex_buffer_data.push_back(vertex_data[fi++]);
          vertex_buffer_data.push_back(vertex_data[fi++]);
          vertex_buffer_data.push_back(vertex_data[fi++]);

          // uv vertices
          vertex_buffer_data.push_back(vertex_data[fi++]);
          vertex_buffer_data.push_back(vertex_data[fi++]);
        }
      }
    });

    
    chunk.vb_index_count_ = vertex_buffer_data.size();

    // copy vector to graphic card
    chunk.InitializeGL();

    glBindBuffer(GL_ARRAY_BUFFER, chunk.vb_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertex_buffer_data.size(),
                 vertex_buffer_data.data(), GL_STATIC_DRAW);

    glBindVertexArray(chunk.vao_);

    constexpr GLsizei stride = (3 + 3 + 2) * sizeof(GLfloat);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)(0 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // uv attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    CHECK_GL();

    chunk.state_ = kViewPrepared;
  }



  /////////////////////////////////////////////////////////////////////////////
  // GameOverlay //////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  JS_DEF_WRAP(GameOverlay)

  void GameOverlay::Prepare() {
    CHECK_GL();
    auto&& crosshair = js_SpriteRegistrar->GetSprite("sprite.crosshair");
    assert(crosshair);

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);

    auto&& all = crosshair->faces.at("all");
    vb_index_count_ = all.size() / 8;

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * all.size(),
                 all.data(), GL_STATIC_DRAW);

    glBindVertexArray(vao_);

    constexpr GLsizei stride = (3 + 3 + 2) * sizeof(GLfloat);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)(0 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // uv attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    CHECK_GL_HERE();
  }

  void GameOverlay::Update(double) {

  }

  void GameOverlay::Render(double ) {
    assert(vao_ > 0 && vb_index_count_ > 0);
    CHECK_GL();

    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, vb_index_count_);
  }



  /////////////////////////////////////////////////////////////////////////////
  // Scene ////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  JS_DEF_WRAP(Scene)

  void Scene::JSCall_SetWindow(JSCbi info) {
    js_Window = info[0].ToObject();
    UpdateViewport();

    window_on_resize_conn_ = js_Window->on_resize.connect([this](int width, int height) {
      UpdateViewport(width, height);
    });

    window_on_cursor_move_conn_ = js_Window->on_game_control_look.connect([this](double xdiff, double ydiff) {
      const float yaw_rad   = .002 * xdiff;
      const float pitch_rad = .002 * ydiff;

      js_Camera->AddLook(yaw_rad, pitch_rad);
    });
  }

  void Scene::JSCall_SetWorld(JSCbi info) {
    js_World = info[0].ToObject();
    UpdateViewport();
  }

  void Scene::Prepare() {
    // depth test
    glEnable(GL_DEPTH_TEST);

    // culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);

    // TODO(daaitch): delete
    glGenQueries(1, &render_gbuffer_query_);
    glGenQueries(1, &render_ssao_query_);
    glGenQueries(1, &render_ssaoblur_query_);
    glGenQueries(1, &render_lighting_query_);
    glGenQueries(1, &render_skybox_query_);
    glGenQueries(1, &render_text_query_);

    // world
    js_World->Prepare();
    js_GameOverlay->Prepare();

    js_Events = Napi::Persistent(Value().Get("events").ToObject());
  }

  void Scene::Update(double diff) {
    auto&& look = js_Camera->GetLook();

    math::fvec3 movement {0.f, 0.f, 0.f};

    if (flight_mode_) {
      constexpr float move_factor = 20.f;

      if (js_Window->GetKey(GLFW_KEY_W) == GLFW_PRESS) {
        glm::vec3 move(-sinf(look.x) * cosf(look.y),
                      sinf(look.y),
                      -cosf(look.x) * cosf(look.y));
        movement += move_factor * (float)diff * move;
      }

      if (js_Window->GetKey(GLFW_KEY_S) == GLFW_PRESS) {
        glm::vec3 move(-sinf(look.x) * cosf(look.y),
                      sinf(look.y),
                      -cosf(look.x) * cosf(look.y));
        movement += -move_factor * (float)diff * move;
      }

      if (js_Window->GetKey(GLFW_KEY_A) == GLFW_PRESS) {
        glm::vec3 move(-cosf(look.x) * cosf(look.y),
                      0.f,
                      sinf(look.x) * cosf(look.y));
        movement += move_factor * (float)diff * move;
      }

      if (js_Window->GetKey(GLFW_KEY_D) == GLFW_PRESS) {
        glm::vec3 move(-cosf(look.x) * cosf(look.y),
                      0.f,
                      sinf(look.x) * cosf(look.y));
        movement += -move_factor * (float)diff * move;
      }
    } else {
      constexpr float move_factor = 5.f;

      vy_ += 25.f * diff;
      movement.y -= vy_ * diff;

      if (js_Window->GetKey(GLFW_KEY_W) == GLFW_PRESS) {
        glm::vec3 move(-sinf(look.x),
                      0,
                      -cosf(look.x));
        movement += move_factor * (float)diff * move;
      }

      if (js_Window->GetKey(GLFW_KEY_S) == GLFW_PRESS) {
        glm::vec3 move(-sinf(look.x),
                      0,
                      -cosf(look.x));
        movement += -move_factor * (float)diff * move;
      }

      if (js_Window->GetKey(GLFW_KEY_A) == GLFW_PRESS) {
        glm::vec3 move(-cosf(look.x),
                      0.f,
                      sinf(look.x));
        movement += move_factor * (float)diff * move;
      }

      if (js_Window->GetKey(GLFW_KEY_D) == GLFW_PRESS) {
        glm::vec3 move(-cosf(look.x),
                      0.f,
                      sinf(look.x));
        movement += -move_factor * (float)diff * move;
      }

    }

    int axis = js_MovementController->AddMovement(movement);
    if (axis & math::CollisionAxis::kY) {
      vy_ = 0.f;
    }

    js_Camera->Update(diff);
    js_World->Update(diff, *js_Camera);
    js_GameOverlay->Update(diff);

    {
      auto&& pos = js_Camera->GetPosition();
      auto&& look = js_Camera->GetLook();

      glm::vec3 look_center(-sinf(look.x) * cosf(look.y),
                             sinf(look.y),
                            -cosf(look.x) * cosf(look.y));

      auto&& new_pointing_block = js_World->FindPointingBlock(math::fLine3::from_range(pos, pos + look_center * 2.f));
      if (new_pointing_block != pointing_block_) {
        pointing_block_ = new_pointing_block;

        JSValue value = pointing_block_ ? jsmath::vec3(js_Events.Env(), pointing_block_.value()) : Env().Undefined();

        js_Events.Get("emit").As<JSFunction>().Call(js_Events.Value(), { JSString::New(Env(), "point"), value });
        
        if (pointing_block_) {
          auto&& def = js_World->GetBlock(pointing_block_.value());
          if (def) {
            js_BlockSelectPass->UpdateBlock(def->GetFacesVertexData());
          }
        }
      }
    }
  }

  void Scene::Render(double diff) {
    CHECK_GL();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    { // g-buffer pass
      CHECK_GL_HERE();
      glBeginQuery(GL_TIME_ELAPSED, render_gbuffer_query_);
      js_GBufferPass->Begin();
      js_GBufferPass->UpdateProjection(js_Camera->GetProjection());
      js_GBufferPass->UpdateView(js_Camera->GetView());
      js_BlockRegistrar->BindTexture();
      js_World->Render(diff, *js_Camera);
      js_GBufferPass->End();
      glEndQuery(GL_TIME_ELAPSED);
      CHECK_GL_HERE();
    }

    { // SSAO
      CHECK_GL_HERE();
      glBeginQuery(GL_TIME_ELAPSED, render_ssao_query_);
      js_SSAOPass->Begin();
      js_SSAOPass->UpdateProjection(js_Camera->GetProjection());
      js_SSAOPass->BindPositionTexture(js_GBufferPass->textures_.position);
      js_SSAOPass->BindNormalTexture(js_GBufferPass->textures_.normal);
      render_quad_.Render();
      js_SSAOPass->End();
      glEndQuery(GL_TIME_ELAPSED);
      CHECK_GL_HERE();
    }

    { // SSAO blur
      CHECK_GL_HERE();
      glBeginQuery(GL_TIME_ELAPSED, render_ssaoblur_query_);
      js_SSAOBlurPass->Begin();
      js_SSAOBlurPass->BindSSAOTexture(js_SSAOPass->textures_.color);
      render_quad_.Render();
      js_SSAOBlurPass->End();
      glEndQuery(GL_TIME_ELAPSED);
      CHECK_GL_HERE();
    }

    { // deferred lighting pass
      CHECK_GL_HERE();
      glBeginQuery(GL_TIME_ELAPSED, render_lighting_query_);
      js_LightingPass->Begin();
      js_LightingPass->BindPositionTexture(js_GBufferPass->textures_.position);
      js_LightingPass->BindNormalTexture(js_GBufferPass->textures_.normal);
      js_LightingPass->BindColorTexture(js_GBufferPass->textures_.color);
      js_LightingPass->BindSSAOTexture(js_SSAOBlurPass->textures_.color);

      js_LightingPass->UpdateRenderDistance(((float)js_World->GetRenderDistance() - 0.5f) * Chunk::SIZE);

      glm::vec3 sun_position = glm::normalize(glm::mat3(js_Camera->GetView()) * glm::vec3(-3, 10, 3));
      js_LightingPass->UpdateSunPosition(sun_position);
      render_quad_.Render();
      js_LightingPass->End();
      glEndQuery(GL_TIME_ELAPSED);
      CHECK_GL_HERE();
    }

    { // skybox
      CHECK_GL_HERE();
      glBeginQuery(GL_TIME_ELAPSED, render_skybox_query_);
      js_SkyboxPass->Begin();
      js_SkyboxPass->BindSkyboxTexture(js_GBufferPass->textures_.position);
      js_SkyboxPass->UpdateProjection(js_Camera->GetProjection());
      js_SkyboxPass->UpdateView(js_Camera->GetViewSkybox());
      js_Skybox->Render();
      js_SkyboxPass->End();
      glEndQuery(GL_TIME_ELAPSED);
      CHECK_GL_HERE();
    }

    if (pointing_block_) {
      CHECK_GL_HERE();
      js_BlockSelectPass->Begin();
      js_BlockSelectPass->UpdateProjection(js_Camera->GetProjection());
      js_BlockSelectPass->UpdateView(js_Camera->GetView() * glm::translate(glm::mat4(1.f), glm::vec3(pointing_block_.value())));
      js_BlockSelectPass->Render();
      js_BlockSelectPass->End();
      CHECK_GL_HERE();
    }

    { // text
      CHECK_GL_HERE();
      glBeginQuery(GL_TIME_ELAPSED, render_text_query_);
      js_TextRenderer->Begin();
      js_TextRenderer->Render();
      js_TextRenderer->End();
      glEndQuery(GL_TIME_ELAPSED);
      CHECK_GL_HERE();
    }

    { // game overlay
      CHECK_GL_HERE();
      glDisable(GL_CULL_FACE); // TODO(daaitch): culling problem
      js_GameOverlayRenderer->Begin();
      js_BlockRegistrar->BindTexture();
      js_GameOverlayRenderer->UpdateProjection();
      js_GameOverlay->Render(diff);
      js_GameOverlayRenderer->End();
      glEnable(GL_CULL_FACE);
      CHECK_GL_HERE();
    }

    {
      js_Window->SwapBuffers();

      // queries
      GLuint64 result = 0;

      glGetQueryObjectui64v(render_gbuffer_query_, GL_QUERY_RESULT, &result);
      HISTOGRAM_METRIC("render_gbuffer", result / 1000);

      glGetQueryObjectui64v(render_ssao_query_, GL_QUERY_RESULT, &result);
      HISTOGRAM_METRIC("render_ssao", result / 1000);

      glGetQueryObjectui64v(render_ssaoblur_query_, GL_QUERY_RESULT, &result);
      HISTOGRAM_METRIC("render_ssaoblur", result / 1000);

      glGetQueryObjectui64v(render_lighting_query_, GL_QUERY_RESULT, &result);
      HISTOGRAM_METRIC("render_lighting", result / 1000);

      glGetQueryObjectui64v(render_skybox_query_, GL_QUERY_RESULT, &result);
      HISTOGRAM_METRIC("render_skybox", result / 1000);

      glGetQueryObjectui64v(render_text_query_, GL_QUERY_RESULT, &result);
      HISTOGRAM_METRIC("render_text", result / 1000);
    }
  }

  void Scene::UpdateViewport() {
    if (js_Window.IsEmpty()) {
      return;
    }

    glm::ivec2 size = js_Window->GetSize();
    UpdateViewport(size.x, size.y);
  }

  void Scene::UpdateViewport(int width, int height) {
    glViewport(0, 0, width, height);
    if (!js_Camera.IsEmpty()) {
      js_Camera->UpdateViewportRatio(width, height);
    }

    InitializeGLBuffers(width, height);
  }
  
  void Scene::InitializeGLBuffers(int width, int height) {
    
    buffer_size_.x = width;
    buffer_size_.y = height;

    js_GBufferPass->UpdateBufferSize(width, height);
    js_SSAOPass->UpdateBufferSize(width, height);
    js_SSAOBlurPass->UpdateBufferSize(width, height);
    js_LightingPass->UpdateBufferSize(width, height);
    js_TextRenderer->UpdateBufferSize(width, height);
    js_GameOverlayRenderer->UpdateBufferSize(width, height);
  }



  /////////////////////////////////////////////////////////////////////////////
  // World ////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

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
      .collisionfree_movement{ movement },
      .outstanding_movement{ math::fvec3 {0.f, 0.f, 0.f} }
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
}