#version 330 core

layout(location = 0) in vec3 in_vertex_position;

uniform mat4 u_proj;
uniform mat4 u_view;

void main() {
  gl_Position = u_proj * u_view * vec4(in_vertex_position, 1.f);
}