#version 330 core

out vec4 color;
in vec2 frag_coord;

uniform sampler2D u_texture;

void main() {
  color = // vec4(1.f, 0.f, 0.f, 1.f) + 0.000000001 *
      texture(u_texture, frag_coord);
}
