#version 330 core

                      out vec2 frag_coord;
layout (location = 0) in  vec3 position;
layout (location = 1) in  vec2 coord;

void main() {
  frag_coord = coord;
  gl_Position = vec4(position, 1.f);
}
