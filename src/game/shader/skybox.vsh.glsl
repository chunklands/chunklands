#version 330 core

                      out vec3 frag_position;
layout (location = 0) in  vec3 position;

uniform mat4 u_proj;
uniform mat4 u_view;

void main() {
  frag_position = position;
  vec4 pos = u_proj * u_view * vec4(position, 1);
  gl_Position = pos.xyww;
}