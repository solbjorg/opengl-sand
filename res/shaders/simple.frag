#version 430 core

in layout(location = 0) vec3 normal;
in layout(location = 1) vec2 textureCoordinates;
in layout(location = 2) vec3 world_position;

uniform layout(location = 3) mat4 model;
uniform layout(location = 4) mat4 VP;

layout(binding=0) uniform sampler2D diffuseSand;

out vec4 color;

const vec3 sand_color = vec3(0.929f, 0.788f, 0.686f);
const vec3 light_source = vec3(0, 100, 0);
const float light_intensity = 0.9f;

float rand(vec2 co) { return fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453); }
float dither(vec2 uv) { return (rand(uv)*2.0-1.0) / 256.0; }

void main()
{
    vec3 norm = normalize(normal);

    // triplanar mapping; thanks to https://gamedevelopment.tutsplus.com/articles/use-tri-planar-texture-mapping-for-better-terrain--gamedev-13821
    vec3 blending = abs(norm);
    blending = normalize(max(blending, 0.00001)); // Force weights to sum to 1.0
    float b = (blending.x + blending.y + blending.z);
    blending /= b;
    vec4 xaxis = texture(diffuseSand, world_position.yz);
    vec4 yaxis = texture(diffuseSand, world_position.xz);
    vec4 zaxis = texture(diffuseSand, world_position.xy);
    // blend the results of the 3 planar projections.
    vec4 tex = xaxis * blending.x + yaxis * blending.y + zaxis * blending.z;
    vec3 light_direction = normalize(light_source - world_position);
    norm.y *= 0.3;
    float diffuse_intensity = clamp(max(0.0f, 4 * dot(light_direction, norm)), 0.f, 1.f);
    float light = diffuse_intensity * light_intensity;
    color = vec4(tex.xyz * light, 1.0f);
}
