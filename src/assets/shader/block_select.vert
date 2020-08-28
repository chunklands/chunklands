#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 u_proj;
uniform mat4 u_view;

void main() { gl_Position = u_proj * u_view * vec4(position, 1); }