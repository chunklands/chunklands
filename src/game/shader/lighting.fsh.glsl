#version 330 core

// Lighting FragmentShader

out vec4 color;
in  vec2 frag_uv;

uniform sampler2D u_position_texture;
uniform sampler2D u_normal_texture;
uniform sampler2D u_color_texture;
uniform float     u_render_distance;

const vec3  light_dir         = normalize(vec3(-5, 2, 5));
const vec3  fog_color         = vec3(.2f, .2f, .2f);
const float darkness_division = .1f;

const float sun = .9f;
const float ambient = .3f;
const float diffuse = sun * (1.f - ambient);

void luminate(vec3 normal) {
  vec4 tex_ambient = color * ambient;
  vec4 tex_diffuse = color * diffuse * max(dot(normal, light_dir), 0.f);

  color = tex_ambient + tex_diffuse;
}

void add_fog(vec3 position) {
  float fog_factor = smoothstep(-10, 0, position.y); // maybe view.y
  color = color + vec4(fog_factor * fog_color, 0);
}

void add_darkness(vec3 position) {
  float darkness_factor = 1 - smoothstep(-30, -10, position.y); // maybe view.y
  color = color * mix(1, darkness_division, darkness_factor);
}

void main() {
  vec3 g_position = texture(u_position_texture, frag_uv).xyz;
  vec3 g_normal   = texture(u_normal_texture, frag_uv).xyz;
  vec4 g_color    = texture(u_color_texture, frag_uv);

  if (g_normal == vec3(0, 0, 0)) {
    discard;
    return;
  }

  color = g_color + 0.0000000000001 * u_render_distance;

  luminate(g_normal);
  add_fog(g_position);
  add_darkness(g_position);
}
