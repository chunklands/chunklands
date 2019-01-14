#version 330 core

in vec3 vertex_normal;
in float z;
out vec3 color;

vec3 light_dir = normalize(vec3(3, 10, -2));

vec3 ambient = vec3(0.6, 0, 0);
vec3 diffuse = vec3(0.4, 0, 0);

void main() {
  float linear_fog = smoothstep(30.f, 60.f, z);
  color = ambient
          + (diffuse * max(dot(vertex_normal, light_dir), 0.f))
          + (linear_fog * vec3(1.f, 1.f, 1.f));
}