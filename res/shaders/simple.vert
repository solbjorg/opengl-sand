#version 430 core

in layout(location = 0) vec3 position;
in layout(location = 1) vec3 normal_in;
in layout(location = 2) vec2 textureCoordinates_in;

uniform layout(location = 3) mat4 model;
uniform layout(location = 4) mat4 VP;

layout(binding=1) uniform sampler2D normalShallowX;
layout(binding=2) uniform sampler2D normalShallowZ;
layout(binding=3) uniform sampler2D normalSteepX;
layout(binding=4) uniform sampler2D normalSteepZ;

out layout(location = 0) vec3 normal_out;
out layout(location = 1) vec2 textureCoordinates_out;
out layout(location = 2) vec3 world_position_out;

void main()
{
    vec3 norm = normalize(normal_in);
    normal_out = norm;
    textureCoordinates_out = textureCoordinates_in;
    vec3 world_position = vec3(model * vec4(position, 1.0f));
    world_position_out = world_position;

    // triplanar mapping of heightmaps
    //vec3 blending = abs(norm);
    //blending = normalize(max(blending, 0.00001)); // Force weights to sum to 1.0
    //float b = (blending.x + blending.y + blending.z);
    //blending /= b;
    //vec4 xaxis = texture(heightmapSteepX, world_position.yz);
    //vec4 zaxis = texture(heightmapSteepZ, world_position.xy);
    //vec4 tex = xaxis * blending.x + zaxis * blending.z;

    gl_Position = VP * model * vec4(position, 1.0f);
}
