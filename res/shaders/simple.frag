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
uniform layout(location = 7) int shininess;
uniform layout(location = 8) int dot_degree;
uniform layout(location = 9) int show_normal_map;

layout(binding=0) uniform sampler2D diffuseSand;
layout(binding=1) uniform sampler2D normalShallowX;
layout(binding=2) uniform sampler2D normalShallowZ;
layout(binding=3) uniform sampler2D normalSteepX;
layout(binding=4) uniform sampler2D normalSteepZ;

out vec4 color;

const vec3 sand_color = vec3(0.929f, 0.788f, 0.686f);
const vec3 light_position = vec3(0, 10000, 1000);
const float light_intensity = 1.0f;

float rand(vec2 co) { return fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453); }
float dither(vec2 uv) { return (rand(uv)*2.0-1.0) / 256.0; }

// Reoriented Normal Mapping for Unity3d
// obviously adapted for GLSL
// http://discourse.selfshadow.com/t/blending-in-detail/21/18
vec3 rnmBlendUnpacked(vec3 n1, vec3 n2) {
    n1 += vec3( 0,  0, 1);
    n2 *= vec3(-1, -1, 1);
    return n1*dot(n1, n2)/n1.z - n2;
}

void main()
{
    vec3 norm = normalize(normal);
    vec3 map_norm;
    // it's noon ok
    vec3 light_direction = normalize(light_position - world_position);
    vec3 camera_direction = normalize(camera_position - world_position);

    vec3 diffuse_color;
    float specular_intensity;

    // triplanar mapping; thanks to https://gamedevelopment.tutsplus.com/articles/use-tri-planar-texture-mapping-for-better-terrain--gamedev-13821
    //vec3 blending = abs(norm);
    //blending = normalize(max(blending, 0.00001)); // Force weights to sum to 1.0
    //float b = (blending.x + blending.y + blending.z);
    //blending /= b;
    //vec3 blending = clamp(pow(norm, vec3(4)), 0.0f, 1.0f);
    //vec3 blending = clamp(pow(norm, vec3(4)), 0.0f, 1.0f);
    vec3 blending = pow(abs(norm), vec3(4));
    blending /= dot(blending, vec3(1,1,1));
    vec4 xaxis = texture(diffuseSand, world_position.yz);
    vec4 yaxis = texture(diffuseSand, world_position.xz);
    vec4 zaxis = texture(diffuseSand, world_position.xy);
    // blend the results of the 3 planar projections.
    vec4 tex = xaxis * blending.x + yaxis * blending.y + zaxis * blending.z;

    // triplanar normal maps using a whiteout blend
    // thanks to https://medium.com/@bgolus/normal-mapping-for-a-triplanar-shader-10bf39dca05a
    vec3 steep_norm_x = texture(normalSteepX, world_position.yz).xyz * 2.0 - 1.0;
    vec3 steep_norm_z = texture(normalSteepZ, world_position.xy).xyz * 2.0 - 1.0;
    steep_norm_x = vec3(steep_norm_x.xy + norm.zy, abs(steep_norm_x.z) * norm.x);
    steep_norm_z = vec3(steep_norm_z.xy + norm.xy, abs(steep_norm_z.z) * norm.z);

    vec3 shallow_norm_x = texture(normalShallowX, world_position.yz * 2).xyz * 2.0 - 1.0;
    vec3 shallow_norm_z = texture(normalShallowZ, world_position.xy * 2).xyz * 2.0 - 1.0;
    shallow_norm_x = vec3(shallow_norm_x.xy + norm.zy, abs(shallow_norm_x.z) * norm.x);
    shallow_norm_z = vec3(shallow_norm_z.xy + norm.xy, abs(shallow_norm_z.z) * norm.z);

    float steepness = 1- abs(norm.y);
    vec3 steep_map_norm = normalize(steep_norm_x.zyx * blending.x + /* norm.xyz * blending.y + */ steep_norm_z.xyz * blending.z).xyz;
    vec3 shallow_map_norm = normalize(shallow_norm_x.zyx * blending.x + shallow_norm_z.xyz * blending.z).xyz;
    map_norm = steepness * 2 * steep_map_norm + (1 - steepness) * norm;
    //map_norm = steep_map_norm;

    // diffuse
    // this kind of weird-looking version of lambert is lifted straight from the GDC talk
    // canonically, dot_degree should be 4 and N_y should be 0.3
    diffuse_color += clamp(dot_degree * dot(light_direction, map_norm * vec3(1,N_y,1)), 0.f, 1.f);

    // specular
    if (specular) {
        //float spec_dot = max(dot(reflect(-light_direction, map_norm), camera_direction), 0);
        vec3 H = normalize(light_direction + camera_direction);
        //specular_intensity += clamp(.5f * spec_dot, 0.f, 1.f);//+= clamp(pow(max(0.0f, spec_dot), shininess), 0.0f, 1.0f);
        specular_intensity = clamp(pow(max(dot(norm, H), 0.0), shininess), 0, 1);
    }
    vec3 spec = specular_intensity * vec3(0.3);
    vec3 light = (diffuse_color + spec) * light_intensity;
    if (show_normal_map == 1)
        color = vec4(abs(norm), 1.0f);
    else if (show_normal_map == 2)
        color = vec4(abs(map_norm), 1.0f);
    else
        color = vec4(tex.xyz * light, 1.0f);
}
