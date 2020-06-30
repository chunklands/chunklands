
#include "TextRenderer.hxx"

namespace chunklands::modules::engine {

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

} // namespace chunklands::modules::engine