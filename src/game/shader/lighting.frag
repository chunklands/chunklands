#version 330 core

// Lighting FragmentShader

out vec4 color;
in  vec2 frag_uv;

uniform sampler2D u_position_texture;
uniform sampler2D u_normal_texture;
uniform sampler2D u_color_texture;
#ifdef SSAO
uniform sampler2D u_ssao_texture;
#endif
uniform float     u_render_distance;
uniform vec3      u_sun_position;

const vec3  fog_color         = vec3(.2f, .2f, .2f);
const float darkness_division = .1f;

const float ambient = .7f;
const float diffuse = .7f;

void luminate(vec3 normal, float occlusion) {
  vec4 tex_ambient = color * ambient * occlusion * occlusion * occlusion * occlusion;
  vec4 tex_diffuse = color * diffuse * max(dot(normal, u_sun_position), 0.f);

  color = tex_ambient + tex_diffuse;
}

void main() {
  vec3 g_position = texture(u_position_texture, frag_uv).xyz;
  vec3 g_normal   = texture(u_normal_texture, frag_uv).xyz;
  vec4 g_color    = texture(u_color_texture, frag_uv);

  #ifdef SSAO
  float ssao_occlusion = texture(u_ssao_texture, frag_uv).r;
  #else
  float ssao_occlusion = 1.f;
  #endif

  if (g_normal == vec3(0, 0, 0)) {
    discard;
    return;
  }

  color = vec4(g_position, 1) * 0.00000000001 + g_color + 0.0000000000001 * u_render_distance;

  luminate(g_normal, ssao_occlusion);
}
