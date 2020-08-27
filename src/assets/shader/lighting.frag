#version 330 core

out vec4 color;
in vec2 frag_uv;

uniform sampler2D u_color_texture;

void main() { color = texture(u_color_texture, frag_uv); }
