#version 330 core

out vec4 color;
in  vec3 frag_position;

uniform samplerCube u_skybox_texture;

void main() {
  color = texture(u_skybox_texture, frag_position);
}