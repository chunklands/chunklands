#version 330 core

// GBuffer FragmentShader

layout(location = 0) out vec3 position;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec4 color;
in vec3 frag_position;
in vec3 frag_normal;
in vec2 frag_uv;
in float frag_distance;

uniform sampler2D u_texture;
uniform float u_new_factor;

const float dMin = 150.f;
const float dMax = 200.f;

void main() {
  // TODO(daaitch): more performant if done in the lighting shader
  float f = (clamp(frag_distance, dMin, dMax) - dMin) / (dMax - dMin);

  position = frag_position;
  normal = frag_normal;
  color = vec4(mix(texture(u_texture, frag_uv).rgb, vec3(1, 1, 1),
                   max(u_new_factor, f * f * f * f)),
               1);
}
