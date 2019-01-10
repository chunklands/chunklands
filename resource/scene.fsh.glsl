#version 330 core

out vec3 color;
in vec3 vertex_normal;

vec3 light_dir = normalize(vec3(3, 10, -2));

vec3 ambient = vec3(0.6, 0, 0);
vec3 diffuse = vec3(0.4, 0, 0);

void main() {
  color = ambient + diffuse * max(dot(vertex_normal, light_dir), 0.f);
}