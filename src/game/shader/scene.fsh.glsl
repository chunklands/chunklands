#version 330 core

in vec3 vertex_normal;
in vec2 vertex_uv;
in float z;
out vec3 color;

uniform sampler2D u_texture;

const vec3 light_dir = normalize(vec3(3, 10, -2));
const vec3 fog_color = vec3(.2f, .2f, .2f);

const float sun = .9f;
const float ambient = .6f;
const float diffuse = sun * (1.f - ambient);

void main() {
  
  vec4 tex_color   = texture(u_texture, vertex_uv).rgba;
  if (tex_color.a < 0.5) {
    discard;
  }

  vec3 tex_ambient = tex_color.rgb * ambient;
  vec3 tex_diffuse = tex_color.rgb * diffuse * max(dot(vertex_normal, light_dir), 0.f);
  vec3 fog_amount = smoothstep(20.f, 60.f, z) * fog_color;

  color = tex_ambient + tex_diffuse + fog_amount;
}
