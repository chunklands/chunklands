#version 330 core

// GBuffer FragmentShader

layout (location = 0) out vec3 position;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec4 color;
                      in  vec3 frag_position;
                      in  vec3 frag_normal;
                      in  vec2 frag_uv;

uniform sampler2D u_texture;

void main() {
  position = frag_position;
  normal = frag_normal;
  color = vec4(
    texture(u_texture, frag_uv).rgb
    // * 0.0000001
    // + vec3(1, 0.2, 0.2)
    , 1);
}
