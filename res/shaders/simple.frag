#version 430 core

in layout(location = 0) vec3 normal;
in layout(location = 1) vec2 textureCoordinates;
in layout(location = 2) vec3 world_position;

uniform layout(location = 3) mat4 model;
uniform layout(location = 4) mat4 VP;

out vec4 color;

const vec3 sand_color = vec3(150, 113, 23);
const vec3 light_source = vec3(100, 100, 100);

float rand(vec2 co) { return fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453); }
float dither(vec2 uv) { return (rand(uv)*2.0-1.0) / 256.0; }

void main()
{
    vec3 norm = normalize(normal);
    vec3 light_direction = normalize(light_source - world_position);
    float light_intensity = max(0.0f, dot(light_direction, norm));

    color = vec4(light_intensity, light_intensity, light_intensity, 1.0);
}
