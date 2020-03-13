#version 430 core

in layout(location = 0) vec3 position;
in layout(location = 1) vec3 normal_in;
in layout(location = 2) vec2 textureCoordinates_in;

uniform layout(location = 3) mat4 model;
uniform layout(location = 4) mat4 VP;

out layout(location = 0) vec3 normal_out;
out layout(location = 1) vec2 textureCoordinates_out;
out layout(location = 2) vec3 world_position_out;

void main()
{
    normal_out = normalize(normal_in);
    textureCoordinates_out = textureCoordinates_in;
    world_position_out = vec3(model * vec4(position, 1.0f));

    gl_Position = VP * model * vec4(position, 1.0f);
}
