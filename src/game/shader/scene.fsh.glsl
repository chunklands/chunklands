#version 330 core

in vec3 vertex_normal;
in vec2 vertex_uv;
in float z;
in float y;
out vec4 frag4;
vec3 frag3;

uniform sampler2D u_texture;
uniform float     u_render_distance;

const vec3  light_dir         = normalize(vec3(3, 10, -2));
const vec3  fog_color         = vec3(.2f, .2f, .2f);
const vec3  background_color  = vec3(.70f, .92f, .97f);
const float darkness_division = .1f;

const float sun = .9f;
const float ambient = .6f;
const float diffuse = sun * (1.f - ambient);

void frag3_luminate() {
  vec3 tex_ambient = frag3 * ambient;
  vec3 tex_diffuse = frag3 * diffuse * max(dot(vertex_normal, light_dir), 0.f);

  frag3 = tex_ambient + tex_diffuse;
}

void frag3_add_fog() {
  float fog_factor = smoothstep(-10, 0, y);
  frag3 = frag3 + (fog_factor * fog_color);
}

void frag3_add_darkness() {
  float darkness_factor = 1 - smoothstep(-30, -10, y);
  frag3 = frag3 * mix(1, darkness_division, darkness_factor);

  float distance_factor = smoothstep(.6f * u_render_distance, u_render_distance, z) * (1 - smoothstep(-30, -10, y));
  frag3 = mix(frag3, vec3(0.f, 0.f, 0.f), distance_factor);
}

void frag4_pseudo_transparency() {
  float f = smoothstep(.9f * u_render_distance, u_render_distance, z);
  float transparency = 1 - f*f;
  frag4.a = transparency;
}

void main() {
  
  frag4 = texture(u_texture, vertex_uv).rgba;
  if (frag4.a < 0.5) {
    discard;
  }

  frag3 = frag4.rgb;
  frag3_luminate();
  frag3_add_fog();
  frag3_add_darkness();

  frag4 = vec4(frag3.rgb, 1.f);
  frag4_pseudo_transparency();
}
