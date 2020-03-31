#version 430 core

in layout(location = 0) vec3 normal;
in layout(location = 1) vec2 textureCoordinates;
in layout(location = 2) vec3 world_position;

uniform layout(location = 2) vec3 camera_position;
uniform layout(location = 3) mat4 model;
uniform layout(location = 4) mat4 VP;
uniform layout(location = 5) float N_y;
// is specular on
uniform layout(location = 6) bool specular;
uniform layout(location = 7) unsigned int shininess;

layout(binding=0) uniform sampler2D diffuseSand;
layout(binding=1) uniform sampler2D heightmapShallowX;
layout(binding=2) uniform sampler2D heightmapShallowZ;
layout(binding=3) uniform sampler2D heightmapSteepX;
layout(binding=4) uniform sampler2D heightmapSteepZ;

out vec4 color;

const vec3 sand_color = vec3(0.929f, 0.788f, 0.686f);
const vec3 light_source = vec3(100, 1000, 0);
const float light_intensity = 0.9f;

float rand(vec2 co) { return fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453); }
float dither(vec2 uv) { return (rand(uv)*2.0-1.0) / 256.0; }

void main()
{
    vec3 norm = normalize(normal);
    // it's noon ok
    vec3 light_direction = vec3(0.1, 1, -0.1);
    vec3 camera_direction = normalize(world_position - camera_position);

    vec3 diffuse_color;
    vec3 specular_color;

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

    vec4 norm_xaxis = texture(heightmapSteepX, world_position.yz);
    vec4 norm_zaxis = texture(heightmapSteepZ, world_position.xy);
    //vec3 map_norm = normalize((blending.x * norm_xaxis + blending.z * norm_zaxis).xyz);
    vec3 map_norm = norm;

    // diffuse
    // this kind of weird-looking version of lambert is lifted straight from the GDC talk
    map_norm.y *= N_y;
    //map_norm = normalize(map_norm);
    diffuse_color += clamp(4 * dot(light_direction, map_norm), 0.f, 1.f);

    // specular
    if (specular) {
        float spec_dot = dot(reflect(-light_direction, map_norm), camera_direction);
        specular_color += clamp(pow(max(0.0f, spec_dot), shininess), 0.0f, 1.0f);
    }
    vec3 light = (diffuse_color + specular_color) * light_intensity;
    color = vec4(tex.xyz * light, 1.0f);
}
