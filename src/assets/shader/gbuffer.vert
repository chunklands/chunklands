#version 330 core

// GBuffer VertexShader

out vec3 frag_position;
out vec3 frag_normal;
out vec2 frag_uv;
out float frag_distance;
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

uniform mat4 u_view;
uniform mat4 u_proj;
uniform float u_new_factor;
uniform vec3 u_camera_pos;

const float tMin = 50.f;
const float tMax = 300.f;

void main() {
  frag_distance = distance(position.xz, u_camera_pos.xz);
  float f = (clamp(frag_distance, tMin, tMax) - tMin) / (tMax - tMin);
  mat4 T = mat4(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 1, 0),
                vec4(0, u_new_factor - (50 * f * f * f * f), 0, 1));
  vec4 pos = u_view * T * vec4(position, 1.f);
  frag_position = pos.xyz;
  frag_uv = uv;

  mat3 normal_matrix = transpose(inverse(mat3(u_view)));
  frag_normal = normal_matrix * normal;
  gl_Position = u_proj * pos;
}
