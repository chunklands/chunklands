#version 330 core

out float color;
in  vec2  frag_coord;

uniform sampler2D u_position;
uniform sampler2D u_normal;
uniform sampler2D u_noise;

uniform vec3 u_samples[64];

uniform mat4 u_proj;

int kernelSize = 64;
float radius = 0.5;
float bias = 0.025;

// TODO(daaitch): update screen size
const vec2 noise_scale = vec2(1280.0 / 4.0, 720.0 / 4.0);

void main() {
  vec3 position   = texture(u_position, frag_coord).xyz;
  vec3 normal     = normalize(texture(u_normal, frag_coord).xyz);
  vec3 random_vec = normalize(texture(u_noise, frag_coord * noise_scale).xyz);

  vec3 tangent   = normalize(random_vec - normal * dot(random_vec, normal));
  vec3 bitangent = cross(normal, tangent);
  mat3 tbn = mat3(tangent, bitangent, normal);

  float occlusion = 0.f;
  for (int i = 0; i < kernelSize; i++) {
    vec3 sample = tbn * u_samples[i];
    sample = position + sample * radius;

    vec4 offset = vec4(sample, 1.f);
    offset = u_proj * offset;
    offset.xyz /= offset.w;
    offset.xyz = offset.xyz * 0.5 + 0.5;

    float sample_depth = texture(u_position, offset.xy).z;

    float range_check = smoothstep(0.f, 1.f, radius / abs(position.z - sample_depth));
    occlusion += (sample_depth >= sample.z + bias ? 1.f : 0.f) * range_check;
  }

  occlusion = 1.f - (occlusion / kernelSize);

  color = occlusion;
}