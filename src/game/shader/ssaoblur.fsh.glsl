#version 330 core

out float color;
in  vec2 frag_coord;

uniform sampler2D u_ssao;

const int blur_size = 10;

void main() {
  vec2 texel_size = 1.f / vec2(textureSize(u_ssao, 0));
  float result = 0.f;

  for (int x = -blur_size; x <= blur_size; x++) {
    for (int y = -blur_size; y <= blur_size; ++y) {
      vec2 offset = vec2(float(x), float(y)) * texel_size;
      result += texture(u_ssao, frag_coord + offset).r;
    }
  }
  int s = (2 * blur_size) + 1;
  color = result / float(s * s);
}
