
#include "GameOverlay.hxx"

namespace chunklands::game {

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


} // namespace chunklands::game