#version 330 core

out vec2 frag_coord;
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 coord;

uniform mat4 u_proj;
uniform mat4 u_view;

void main() {
  frag_coord = coord;
  gl_Position = u_proj * u_view * vec4(position.xy, 0.f, 1.f);
}
