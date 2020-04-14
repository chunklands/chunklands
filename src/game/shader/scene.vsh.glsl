#version 330 core

layout(location = 0) in vec3 in_vertex_position;
layout(location = 1) in vec3 in_vertex_normal;
layout(location = 2) in vec2 in_vertex_uv;

uniform mat4 u_proj;
uniform mat4 u_view;

out vec3 vertex_normal;
out vec2 vertex_uv;
out float z;
out float y;

void main() {
  gl_Position = u_proj * u_view * vec4(in_vertex_position.xyz, 1.f);
  vertex_normal = in_vertex_normal;
  vertex_uv = in_vertex_uv;
  z = gl_Position.z;
  y = in_vertex_position.y;
}