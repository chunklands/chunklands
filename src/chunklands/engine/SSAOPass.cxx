
#include "SSAOPass.hxx"

#include <random>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

namespace chunklands::engine {

  JS_DEF_WRAP(SSAOPass)

  void SSAOPass::InitializeProgram() {

    modules::gl::Uniform position{"u_position"},
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

      modules::gl::Uniform sample_uniform{"u_samples", i};
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

} // namespace chunklands::engine