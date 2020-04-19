#version 330 core

// GBuffer VertexShader

                      out vec3 frag_position;
                      out vec3 frag_normal;
                      out vec2 frag_uv;
layout (location = 0) in  vec3 position;
layout (location = 1) in  vec3 normal;
layout (location = 2) in  vec2 uv;

uniform mat4 u_view;
uniform mat4 u_proj;

void main() {
  vec4 pos = u_view * vec4(position, 1.f);
  frag_position = pos.xyz;
  frag_uv = uv;

  mat3 normal_matrix = transpose(inverse(mat3(u_view)));
  frag_normal = normal_matrix * normal;
  gl_Position = u_proj * pos;
}
