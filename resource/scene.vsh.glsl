#version 330 core

layout(location = 0) in vec3 in_vertex_position;
layout(location = 1) in vec3 in_vertex_normal;

uniform mat4 u_proj;
uniform mat4 u_view;

out vec3 vertex_normal;

void main() {
  gl_Position = u_proj * u_view
    * vec4(in_vertex_position.xy, -in_vertex_position.z, 1.f);

  vertex_normal = in_vertex_normal;
}