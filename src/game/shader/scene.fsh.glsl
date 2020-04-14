#version 330 core

in vec3 vertex_normal;
in vec2 vertex_uv;
in float z;
in float y;
out vec3 color;

uniform sampler2D u_texture;
uniform float     u_y;

const vec3  light_dir         = normalize(vec3(3, 10, -2));
const vec3  fog_color         = vec3(.2f, .2f, .2f);
const vec3  background_color  = vec3(.70f, .92f, .97f);
const float darkness_division = .2f;

const float sun = .9f;
const float ambient = .6f;
const float diffuse = sun * (1.f - ambient);

void frag_luminate() {
  vec3 tex_ambient = color * ambient;
  vec3 tex_diffuse = color * diffuse * max(dot(vertex_normal, light_dir), 0.f);

  color = tex_ambient + tex_diffuse;
}

void frag_add_fog() {
  float fog_factor = smoothstep(-5, 20, y);
  color = color + (fog_factor * fog_color);
}

void frag_add_darkness() {
  float darkness_factor = 1 - smoothstep(-30, 0, y);
  color = color * mix(1, darkness_division, darkness_factor);
}

void frag_pseudo_transparency() {
  float transparency = smoothstep(58.f, 60.f, z);
  color = mix(color, background_color, transparency);
}

void main() {
  
  vec4 frag4   = texture(u_texture, vertex_uv).rgba;
  if (frag4.a < 0.5) {
    discard;
  }

  color = frag4.rgb;

  frag_luminate();
  frag_add_fog();
  frag_add_darkness();
  frag_pseudo_transparency();
}

