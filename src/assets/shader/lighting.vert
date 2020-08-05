#version 330 core

                     out vec2 frag_uv;
layout(location = 0) in  vec3 position;
layout(location = 1) in  vec2 uv;

void main() {
  frag_uv = uv;
  gl_Position = vec4(position, 1);
}