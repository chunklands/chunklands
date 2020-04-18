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

void main()
{
  frag_position = position;
  frag_normal = normal;
  frag_uv = uv;

  gl_Position = u_proj * u_view * vec4(position, 1);
}