#include "engine_module.hxx"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>
#include <random>

#include <chunklands/modules/misc/misc_module.hxx>
#include <chunklands/jsmath.hxx>

namespace chunklands::modules::engine {

  RenderPass::~RenderPass() {
    // empty
  }
  
  /////////////////////////////////////////////////////////////////////////////
  // Environment //////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  JS_DEF_WRAP(Environment)

  void Environment::JSCall_initialize(JSCbi info) {
    const int init = glfwInit();
    JS_ASSERT_MSG(info.Env(), init == GLFW_TRUE, "could not initialize GLFW");
  }

  void Environment::JSCall_loadProcs(JSCbi info) {
    JS_ASSERT_MSG(info.Env(), glfwGetCurrentContext() != nullptr, "call after `window.makeCurrentContext()`");
    
    const int load = gladLoadGL((GLADloadfunc)glfwGetProcAddress);
    JS_ASSERT_MSG(info.Env(), load != 0, "could not load GL procs");
    
    CHECK_GL();
  }

  void Environment::JSCall_terminate(JSCbi) {
    glfwTerminate();
  }



  /////////////////////////////////////////////////////////////////////////////
  // GameLoop /////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  JS_DEF_WRAP(GameLoop)

  void GameLoop::JSCall_start(JSCbi info) {
    JS_ASSERT(info.Env(), !running_);

    js_Scene->Prepare();

    running_ = true;
    last_update_ = glfwGetTime();
    last_render_ = last_update_;
  }

  void GameLoop::JSCall_stop(JSCbi info) {
    JS_ASSERT(info.Env(), running_);
    running_ = false;
  }

  // constexpr double update_threshold = 1.0 / 30.0; // Hz

  void GameLoop::JSCall_loop(JSCbi info) {
    DURATION_METRIC("gameloop_loop");

    JS_ASSERT(info.Env(), running_);

    const double time = glfwGetTime();
    // { // update
    //   double update_diff = time - last_update_;
    //   while (update_diff >= update_threshold) {
    //     update_diff -= update_threshold;
    //     js_Scene->Update(update_threshold);
    //   }

    //   last_update_ = time - update_diff;
    // }

    { // update
      double update_diff = time - last_update_;
      js_Scene->Update(update_diff);

      last_update_ = time;
    }

    { // render
      const double render_diff = time - last_render_;
      last_render_ = time;
      js_Scene->Render(render_diff);
    }

    glfwPollEvents();
  }



  /////////////////////////////////////////////////////////////////////////////
  // GBufferPass //////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  JS_DEF_WRAP(GBufferPass)

  void GBufferPass::InitializeProgram() {
    gl::Uniform texture {"u_texture"};
    *js_Program >> uniforms_.proj >> uniforms_.view >> texture;

    texture.Update(0);
  }

  void GBufferPass::UpdateBufferSize(int width, int height) {
    DeleteBuffers();

    glGenFramebuffers(1, &framebuffer_);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);

    glGenTextures(1, &textures_.position);
    glBindTexture(GL_TEXTURE_2D, textures_.position);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures_.position, 0);

    glGenTextures(1, &textures_.normal);
    glBindTexture(GL_TEXTURE_2D, textures_.normal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, textures_.normal, 0);

    glGenTextures(1, &textures_.color);
    glBindTexture(GL_TEXTURE_2D, textures_.color);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, textures_.color, 0);

    const GLuint attachments[3] = {
      GL_COLOR_ATTACHMENT0,
      GL_COLOR_ATTACHMENT1,
      GL_COLOR_ATTACHMENT2
    };

    glDrawBuffers(3, attachments);

    glGenRenderbuffers(1, &renderbuffer_);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer_);

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void GBufferPass::DeleteBuffers() {
    glDeleteTextures(1, &textures_.position);
    glDeleteTextures(1, &textures_.normal);
    glDeleteTextures(1, &textures_.color);
    glDeleteRenderbuffers(1, &renderbuffer_);
    glDeleteFramebuffers(1, &framebuffer_);
  }



  /////////////////////////////////////////////////////////////////////////////
  // LightingPass /////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  JS_DEF_WRAP(LightingPass)

  void LightingPass::InitializeProgram() {
    gl::Uniform position_texture{"u_position_texture"},
              normal_texture{"u_normal_texture"},
              color_texture{"u_color_texture"},
              ssao_texture{"u_ssao_texture"};

    *js_Program
      >> uniforms_.render_distance
      >> uniforms_.sun_position
      >> position_texture
      >> normal_texture
      >> color_texture
      >> ssao_texture;

    position_texture.Update(0);
    normal_texture.Update(1);
    color_texture.Update(2);
    ssao_texture.Update(3);
  }



  /////////////////////////////////////////////////////////////////////////////
  // SkyboxPass ///////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  JS_DEF_WRAP(SkyboxPass)
  
  void SkyboxPass::InitializeProgram() {
    CHECK_GL();

    gl::Uniform skybox_texture{"u_skybox_texture"};

    *js_Program
      >> skybox_texture
      >> uniforms_.proj
      >> uniforms_.view;

    skybox_texture.Update(0);
  }



  /////////////////////////////////////////////////////////////////////////////
  // SkyboxTexture ////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  void SkyboxTexture::LoadTexture(const std::string& prefix) {
    CHECK_GL();

    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_);

    {
      CHECK_GL_HERE();
      gl::TextureLoader loader(prefix + "right.png");
      glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, loader.width, loader.height, 0, loader.format, GL_UNSIGNED_BYTE, loader.data);
    }

    {
      CHECK_GL_HERE();
      gl::TextureLoader loader(prefix + "left.png");
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, loader.width, loader.height, 0, loader.format, GL_UNSIGNED_BYTE, loader.data);
    }

    {
      CHECK_GL_HERE();
      gl::TextureLoader loader(prefix + "top.png");
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, loader.width, loader.height, 0, loader.format, GL_UNSIGNED_BYTE, loader.data);
    }

    {
      CHECK_GL_HERE();
      gl::TextureLoader loader(prefix + "bottom.png");
      glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, loader.width, loader.height, 0, loader.format, GL_UNSIGNED_BYTE, loader.data);
    }

    {
      CHECK_GL_HERE();
      gl::TextureLoader loader(prefix + "front.png");
      glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, loader.width, loader.height, 0, loader.format, GL_UNSIGNED_BYTE, loader.data);
    }

    {
      CHECK_GL_HERE();
      gl::TextureLoader loader(prefix + "back.png");
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, loader.width, loader.height, 0, loader.format, GL_UNSIGNED_BYTE, loader.data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  }



  /////////////////////////////////////////////////////////////////////////////
  // SSAOBlurPass /////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  JS_DEF_WRAP(SSAOBlurPass)

  void SSAOBlurPass::UpdateBufferSize(int width, int height) {
    glGenFramebuffers(1, &framebuffer_);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
    glGenTextures(1, &textures_.color);
    glBindTexture(GL_TEXTURE_2D, textures_.color);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures_.color, 0);

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void SSAOBlurPass::DeleteBuffers() {
    glDeleteTextures(1, &textures_.color);
    glDeleteFramebuffers(1, &framebuffer_);
  }



  /////////////////////////////////////////////////////////////////////////////
  // SSAOPass /////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  JS_DEF_WRAP(SSAOPass)

  void SSAOPass::InitializeProgram() {

    gl::Uniform position{"u_position"},
                normal{"u_normal"},
                noise{"u_noise"};

    *js_Program >> uniforms_.proj >> position >> normal >> noise;
    
    position.Update(0);
    normal.Update(1);
    noise.Update(2);

    std::uniform_real_distribution<GLfloat> random_floats(0.f, 1.f);
    std::default_random_engine generator;
    const int kernel_size = 8;
    for (int i = 0; i < kernel_size; i++) {
      glm::vec3 sample(
        random_floats(generator) * 2.f - 1.f,
        random_floats(generator) * 2.f - 1.f,
        random_floats(generator)
      );

      sample = glm::normalize(sample);
      sample *= random_floats(generator);
      float scale = float(i) / (float)kernel_size;

      scale = glm::lerp(.1f, 1.f, scale * scale);
      sample *= scale;

      gl::Uniform sample_uniform{"u_samples", i};
      *js_Program >> sample_uniform;
      sample_uniform.Update(sample);
    }
  }

  void SSAOPass::UpdateBufferSize(int width, int height) {
    DeleteBuffers();

    glGenFramebuffers(1, &framebuffer_);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);

    glGenTextures(1, &textures_.color);
    glBindTexture(GL_TEXTURE_2D, textures_.color);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures_.color, 0);
    
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    std::uniform_real_distribution<GLfloat> random_floats(0.f, 1.f);
    std::default_random_engine generator;

    std::array<glm::vec3, 16> ssao_noise;
    for (int i = 0; i < 16; i++) {
      glm::vec3 noise(
        random_floats(generator) * 2.f - 1.f,
        random_floats(generator) * 2.f - 1.f,
        0.f
      );
      ssao_noise[i] = std::move(noise);
    }

    glGenTextures(1, &textures_.noise);
    glBindTexture(GL_TEXTURE_2D, textures_.noise);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, ssao_noise.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  }

  void SSAOPass::DeleteBuffers() {
    glDeleteTextures(1, &textures_.color);
    glDeleteTextures(1, &textures_.noise);
    glDeleteFramebuffers(1, &framebuffer_);
  }



  /////////////////////////////////////////////////////////////////////////////
  // GameOverlayRenderer //////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
  
  JS_DEF_WRAP(GameOverlayRenderer)

  

  /////////////////////////////////////////////////////////////////////////////
  // Window ///////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  namespace detail {
    Window* Unwrap(GLFWwindow* window) {
      return static_cast<Window*>(glfwGetWindowUserPointer(window));
    }
  }
  
  JS_DEF_WRAP(Window)

  void Window::JSCall_initialize(JSCbi info) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #endif

    auto&& config = info[0].ToObject();
    window_ = glfwCreateWindow(
      config.Get("width").ToNumber(),
      config.Get("height").ToNumber(),
      config.Get("title").ToString().Utf8Value().c_str(),
      nullptr,
      nullptr
    );

    JS_ASSERT(info.Env(), window_ != nullptr);

    glfwSetWindowUserPointer(window_, this);

    glfwSetFramebufferSizeCallback(window_, [](GLFWwindow* window, int width, int height) {
      detail::Unwrap(window)->on_resize(width, height);
    });

    events_ = Napi::Persistent(Value().Get("events").ToObject());

    glfwSetKeyCallback(window_, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
      auto&& events = detail::Unwrap(window)->events_;
      auto&& env = events.Env();

      JSHandleScope scope(env);
      {
        auto&& event = JSObject::New(env);
        event["key"] = JSNumber::New(env, key);
        event["scancode"] = JSNumber::New(env, scancode);
        event["action"] = JSNumber::New(env, action);
        event["mods"] = JSNumber::New(env, mods);

        events.Get("emit").As<JSFunction>().Call(events.Value(), {JSString::New(events.Env(), "key"), event});
      }
    });

    glfwSetMouseButtonCallback(window_, [](GLFWwindow* window, int button, int action, int mods) {
      auto&& events = detail::Unwrap(window)->events_;
      auto&& env = events.Env();

      JSHandleScope scope(env);
      {
        auto&& event = JSObject::New(env);
        event["button"] = JSNumber::New(env, button);
        event["action"] = JSNumber::New(env, action);
        event["mods"] = JSNumber::New(env, mods);

        events.Get("emit").As<JSFunction>().Call(events.Value(), {JSString::New(events.Env(), "mousebutton"), event});
      }
    });

    // no sticky keys: we get strange behavior of pressing e.g. "F" emits: F down, F up, F up
    // glfwSetInputMode(window_, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwShowWindow(window_);

    glfwSwapInterval(1);
  }

  void Window::JSCall_makeContextCurrent(JSCbi) {
    glfwMakeContextCurrent(window_);
  }

  JSValue Window::JSCall_shouldClose(JSCbi info) {
    if (!window_) {
      return JSBoolean::New(info.Env(), true);
    }

    const int should_close = glfwWindowShouldClose(window_);
    return JSBoolean::New(info.Env(), should_close == GLFW_TRUE);
  }

  void Window::JSCall_close(JSCbi) {
    if (!window_) {
      return;
    }

    glfwDestroyWindow(window_);
    window_ = nullptr;
  }

  void Window::SwapBuffers() {
    if (!window_) {
      return;
    }
    
    glfwSwapBuffers(window_);
  }

  int Window::GetKey(int key) {
    return glfwGetKey(window_, key);
  }

  int Window::GetMouseButton(int button) {
    return glfwGetMouseButton(window_, button);
  }

  glm::dvec2 Window::GetCursorPos() const {
    glm::dvec2 pos;
    glfwGetCursorPos(window_, &pos.x, &pos.y);
    return pos;
  }

  glm::ivec2 Window::GetSize() const {
    glm::ivec2 size;
    glfwGetWindowSize(window_, &size.x, &size.y);
    return size;
  }

  void Window::CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    auto&& me = detail::Unwrap(window);

    glm::dvec2 pos(xpos, ypos);
    glm::dvec2 cursor_diff = me->game_control_last_cursor_pos_ - pos;
    me->game_control_last_cursor_pos_ = pos;
    me->on_game_control_look(cursor_diff.x, cursor_diff.y);
  }

  void Window::StartMouseGrab() {
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window_, CursorPosCallback);
    game_control_last_cursor_pos_ = GetCursorPos();
  }

  void Window::StopMouseGrab() {
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window_, nullptr);
  }



  /////////////////////////////////////////////////////////////////////////////
  // Skybox ///////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  GLfloat skybox_vertices[] = {
    -1.f,  1.f, -1.f,
    -1.f, -1.f, -1.f,
     1.f, -1.f, -1.f,
     1.f, -1.f, -1.f,
     1.f,  1.f, -1.f,
    -1.f,  1.f, -1.f,

    -1.f, -1.f,  1.f,
    -1.f, -1.f, -1.f,
    -1.f,  1.f, -1.f,
    -1.f,  1.f, -1.f,
    -1.f,  1.f,  1.f,
    -1.f, -1.f,  1.f,

     1.f, -1.f, -1.f,
     1.f, -1.f,  1.f,
     1.f,  1.f,  1.f,
     1.f,  1.f,  1.f,
     1.f,  1.f, -1.f,
     1.f, -1.f, -1.f,

    -1.f, -1.f,  1.f,
    -1.f,  1.f,  1.f,
     1.f,  1.f,  1.f,
     1.f,  1.f,  1.f,
     1.f, -1.f,  1.f,
    -1.f, -1.f,  1.f,

    -1.f,  1.f, -1.f,
     1.f,  1.f, -1.f,
     1.f,  1.f,  1.f,
     1.f,  1.f,  1.f,
    -1.f,  1.f,  1.f,
    -1.f,  1.f, -1.f,

    -1.f, -1.f, -1.f,
    -1.f, -1.f,  1.f,
     1.f, -1.f, -1.f,
     1.f, -1.f, -1.f,
    -1.f, -1.f,  1.f,
     1.f, -1.f,  1.f
  };

  JS_DEF_WRAP(Skybox)

  void Skybox::JSCall_initialize(JSCbi info) {
    CHECK_GL();
    DeleteGLArrays();

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *)0);

    assert(info[0].IsString());
    texture_.LoadTexture(info[0].ToString().Utf8Value());
  }

  void Skybox::Render() {
    texture_.ActiveAndBind(GL_TEXTURE0);

    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
  }



  /////////////////////////////////////////////////////////////////////////////
  // Camera ///////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  JS_DEF_WRAP(Camera)

  void Camera::AddLook(float yaw_rad, float pitch_rad) {
    constexpr float pitch_break = (.5f * M_PI) - .1;
    
    look_.x += yaw_rad;
    look_.x = fmodf(look_.x, 2.f * M_PI);

    look_.y += pitch_rad;
    look_.y = std::max(std::min(look_.y, pitch_break), -pitch_break);
  }

  void Camera::UpdateViewportRatio(int width, int height) {
    constexpr float fovy_degree = 75.f;
    
    proj_ = glm::perspective(glm::radians(fovy_degree), (float)width / height, 0.1f, 1000.0f);
  }

  void Camera::Update(double) {
    glm::vec3 look_center(-sinf(look_.x) * cosf(look_.y),
                          sinf(look_.y),
                          -cosf(look_.x) * cosf(look_.y));

    view_        = glm::lookAt(pos_, pos_ + look_center, glm::vec3(0.f, 1.f, 0.f));
    view_skybox_ = glm::lookAt(glm::vec3(0, 0, 0), look_center, glm::vec3(0.f, 1.f, 0.f));
  }

  void Camera::JSCall_SetPosition(JSCbi info) {
    pos_.x = info[0].ToNumber().FloatValue();
    pos_.y = info[1].ToNumber().FloatValue();
    pos_.z = info[2].ToNumber().FloatValue();
  }

  JSValue Camera::JSCall_GetPosition(JSCbi info) {
    return jsmath::vec3(info.Env(), pos_);
  }

  JSValue Camera::JSCall_GetLook(JSCbi info) {
    return jsmath::vec2(info.Env(), look_);
  }



  /////////////////////////////////////////////////////////////////////////////
  // collision_result /////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  std::ostream& operator<<(std::ostream& os, const collision_result& c) {
    return os << "collision_result { ctime=" << c.ctime << ", collisionfree_movement=" << c.collisionfree_movement
      << ", outstanding_movement=" << c.outstanding_movement << " }";
  }



  /////////////////////////////////////////////////////////////////////////////
  // MovementController ///////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  JS_DEF_WRAP(MovementController)

  int MovementController::AddMovement(math::fvec3 outstanding_movement) {
    DURATION_METRIC("movement_time");

    int axis = math::CollisionAxis::kNone;

    int next_collision_index = 0;
    while(math::chess_distance(outstanding_movement, math::fvec3(0, 0, 0)) > 0.f) {

      if (DEBUG_COLLISION) {
        std::cout << "Next collision #" << next_collision_index << std::endl;
      }

      collision_result result = js_CollisionSystem->ProcessNextCollision(player_box_ + js_Camera->GetPosition(), outstanding_movement);
      js_Camera->AddPos(result.collisionfree_movement);
      outstanding_movement = result.outstanding_movement;
      axis |= result.axis;

      ++next_collision_index;
    }

    return axis;
  }



  /////////////////////////////////////////////////////////////////////////////
  // FontLoader ///////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  JS_DEF_WRAP(FontLoader)

  void FontLoader::JSCall_load(JSCbi info) {
    JS_ASSERT(info.Env(), info.Length() == 2);

    JSHandleScope scope(info.Env());
    
    // texture
    std::string image_path = info[1].ToString();
    texture_.LoadTexture(image_path.c_str());

    // meta data
    meta_.clear();

    auto&& js_meta = info[0].ToObject();
    auto&& js_chars = js_meta.Get("characters").ToObject();
    auto&& js_char_keys = js_chars.GetPropertyNames();
    for (unsigned i = 0; i < js_char_keys.Length(); i++) {
      auto&& js_char_key = js_char_keys.Get(i);
      std::string k = js_char_key.ToString();
      // e.g. "0":{"x":135,"y":35,"width":20,"height":28,"originX":1,"originY":25,"advance":18},
      auto&& js_char_value = js_chars.Get(js_char_key).ToObject();

      std::string key = js_char_key.ToString();

      meta_.insert({key, {
        .pos {
          js_char_value.Get("x").ToNumber().Int32Value(),
          js_char_value.Get("y").ToNumber().Int32Value()
        },
        .size {
          js_char_value.Get("width").ToNumber().Int32Value(),
          js_char_value.Get("height").ToNumber().Int32Value()
        },
        .origin {
          js_char_value.Get("originX").ToNumber().Int32Value(),
          js_char_value.Get("originY").ToNumber().Int32Value()
        },
        .advance = js_char_value.Get("advance").ToNumber().Int32Value(),
      }});
    }
  }



  /////////////////////////////////////////////////////////////////////////////
  // TextRenderer /////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
  
  JS_DEF_WRAP(TextRenderer)

  void TextRenderer::JSCall_write(JSCbi info) {
    std::string text = info[0].ToString();

    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    const int LINE_HEIGHT = 32;

    std::vector<GLfloat> v;
    v.reserve(text.length() * 12 * 2);
    glm::ivec2 offset(10, -10);
    glm::ivec2 pos(0, height_ - LINE_HEIGHT);
    auto&& size = js_FontLoader->GetTexture().GetSize();
    
    for (char c : text) {
      if (c == '\n') {
        pos.x = 0;
        pos.y -= LINE_HEIGHT; // TODO(daaitch): size + spacer
        continue;
      }

      std::string s(&c, 1);
      auto&& meta = js_FontLoader->Get(s);
      if (!meta) {
        continue;
      }

      auto&& value = meta.value();

      glm::ivec2 v0 = glm::ivec2(pos.x - value.origin.x, pos.y - value.size.y + value.origin.y) + offset;
      glm::ivec2 v1 = v0 + value.size;
      
      glm::vec2 uv0(float(value.pos.x) / size.x, 1.f - float(-value.pos.y - value.size.y) / size.y);
      glm::vec2 uv1 = uv0 + glm::vec2(float(value.size.x) / size.x, float(-value.size.y) / size.y);

      v.push_back(v0.x);
      v.push_back(v0.y);
      v.push_back(uv0.x);
      v.push_back(uv0.y);

      v.push_back(v0.x);
      v.push_back(v1.y);
      v.push_back(uv0.x);
      v.push_back(uv1.y);

      v.push_back(v1.x);
      v.push_back(v1.y);
      v.push_back(uv1.x);
      v.push_back(uv1.y);

      v.push_back(v0.x);
      v.push_back(v0.y);
      v.push_back(uv0.x);
      v.push_back(uv0.y);

      v.push_back(v1.x);
      v.push_back(v1.y);
      v.push_back(uv1.x);
      v.push_back(uv1.y);

      v.push_back(v1.x);
      v.push_back(v0.y);
      v.push_back(uv1.x);
      v.push_back(uv0.y);

      pos.x += value.advance;
      count_ += 6;
    }
    
    glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(decltype(v)::value_type), v.data(), GL_DYNAMIC_DRAW);

    constexpr GLsizei stride = (2 + 2) * sizeof(GLfloat);

    // position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)(0 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    
    // uv
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    CHECK_GL();
  }

  void TextRenderer::InitializeProgram() {
    CHECK_GL();
    gl::Uniform texture {"u_texture"};
    *js_Program >> texture >> uniforms_.proj;

    texture.Update(0);
  }

  void TextRenderer::Render() {
    if (vao_ > 0) {
      CHECK_GL();
      js_FontLoader->GetTexture().ActiveAndBind(GL_TEXTURE0);

      uniforms_.proj.Update(proj_);

      glBindVertexArray(vao_);
      glDrawArrays(GL_TRIANGLES, 0, count_);
    }
  }
}
