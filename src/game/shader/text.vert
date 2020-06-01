#version 330 core

                      out vec2 frag_coord;
layout (location = 0) in  vec2 position;
layout (location = 1) in  vec2 coord;

uniform mat4 u_proj;

void main() {
  frag_coord = coord;
  gl_Position = u_proj * vec4(position, 0.f, 1.f);
}
